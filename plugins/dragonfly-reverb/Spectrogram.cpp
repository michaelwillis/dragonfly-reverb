/*
 * Dragonfly Reverb, a hall-style reverb plugin
 * Copyright (c) 2018 Michael Willis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#include "Spectrogram.hpp"
#include <stdlib.h>
#include <math.h>
#include <iostream>

Spectrogram::Spectrogram(Widget * widget, NanoVG * fNanoText, Rectangle<int> * rect) :
        Widget(widget->getParentWindow()),
        dsp(SPECTROGRAM_SAMPLE_RATE) {
  setWidth(rect->getWidth());
  setHeight(rect->getHeight());
  setAbsolutePos(rect->getPos());
  this->fNanoText = fNanoText;

  int imageWidth = getWidth() - MARGIN_LEFT - MARGIN_RIGHT;
  int imageHeight = getHeight() - MARGIN_TOP - MARGIN_BOTTOM;

  raster = new char[imageWidth * imageHeight * 4];

  // fill with transparent white
  for (int32_t pixel = 0; pixel < imageWidth * imageHeight; pixel++) {
    raster[pixel * 4]     = (char) 255;
    raster[pixel * 4 + 1] = (char) 255;
    raster[pixel * 4 + 2] = (char) 255;
    raster[pixel * 4 + 3] = (char) 0;
  }

  image = new Image(raster, imageWidth, imageHeight, GL_BGRA);

  srand(time(NULL));

  white_noise = new float*[2];
  white_noise[0] = new float[SPECTROGRAM_WINDOW_SIZE];
  white_noise[1] = new float[SPECTROGRAM_WINDOW_SIZE];

  silence = new float*[2];
  silence[0] = new float[SPECTROGRAM_WINDOW_SIZE];
  silence[1] = new float[SPECTROGRAM_WINDOW_SIZE];

  dsp_output = new float*[2];
  dsp_output[0] = new float[SPECTROGRAM_WINDOW_SIZE];
  dsp_output[1] = new float[SPECTROGRAM_WINDOW_SIZE];

  for (uint32_t i = 0; i < SPECTROGRAM_WINDOW_SIZE; i++) {
    // Fill one window's time of the input buffer with white noise
    white_noise[0][i] = (float)((rand() % 4096) - 2048) / 2048.0f;
    white_noise[1][i] = (float)((rand() % 4096) - 2048) / 2048.0f;

    silence[0][i] = 0.0f;
    silence[1][i] = 0.0f;

    // Hann window function, per https://en.wikipedia.org/wiki/Hann_function
    window_multiplier[i] = pow(sin((M_PI * i) / (SPECTROGRAM_WINDOW_SIZE - 1)), 2);
  }

  x = 0;
  sample_offset_processed = 0;
}

Spectrogram::~Spectrogram() {

  delete[] white_noise[0];
  delete[] white_noise[1];
  delete[] white_noise;

  delete[] silence[0];
  delete[] silence[1];
  delete[] silence;

  delete[] dsp_output[0];
  delete[] dsp_output[1];
  delete[] dsp_output;

  delete[] fftw_in;
  delete[] fftw_out;
  delete[] raster;
  delete image;
}

void Spectrogram::uiIdle() {
  uint32_t max_x = x + 20;
  if (max_x > image->getWidth()) {
    max_x = image->getWidth();
  }
  while(x < max_x) {
    // Calculate time in seconds, then determine where that is in the dsp_output buffer
    float time = pow(M_E, (float) x * logf ( 10.0f / 0.1f ) / image->getWidth()) * 0.1f;
    uint32_t sample_offset = (time * (float) SPECTROGRAM_SAMPLE_RATE);

    if (sample_offset_processed < sample_offset + SPECTROGRAM_WINDOW_SIZE * 2) {
      if (sample_offset_processed == 0) {
        dsp.run((const float **)white_noise, dsp_output, SPECTROGRAM_WINDOW_SIZE);
      } else {
        dsp.run((const float **)silence, dsp_output, SPECTROGRAM_WINDOW_SIZE);
      }

      for (uint32_t i = 0; i < SPECTROGRAM_WINDOW_SIZE; i++) {
        reverb_results[sample_offset_processed + i] = dsp_output[0][i];
      }

      sample_offset_processed += SPECTROGRAM_WINDOW_SIZE;
    }
    else {
      for (uint32_t window_sample = 0; window_sample < SPECTROGRAM_WINDOW_SIZE; window_sample++) {
        fftw_in[window_sample] = reverb_results[sample_offset + SPECTROGRAM_WINDOW_SIZE + window_sample] * window_multiplier[window_sample];
      }
      fftwf_execute(fftw_plan);

      for (uint32_t y = 0; y < image->getHeight(); y++) {
          float freq = powf(M_E, (float) y * logf ( 20000.0f / 50.0f ) / (float) image->getHeight()) * 50.0f;
          int fftw_out_index = freq / (float)(SPECTROGRAM_SAMPLE_RATE / SPECTROGRAM_WINDOW_SIZE) + 1;

          float val = fftw_out[fftw_out_index][0];
          if (val < 0.0) val = 0.0 - val;
          if (val > 4.0) val = 4.0;

          char alpha = (char)(val * 63.0f);

          uint32_t pixel = (image->getHeight() - y - 1) * image->getWidth() + x;

          raster[pixel * 4 + 3] = alpha;
      }

      image->loadFromMemory(raster, image->getWidth(), image->getHeight(), GL_BGRA);
      repaint();
      x++;
    }
  }
}

void Spectrogram::onDisplay() {
  image->drawAt(MARGIN_LEFT, MARGIN_TOP);

  int freq[] = {50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
  std::string freqStrings[]  = {"50Hz", "100Hz", "200Hz", "500Hz", "1kHz", "2kHz", "5kHz", "10kHz", "20kHz"};
  float decayTime[] = { 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 5.0f, 10.0f };
  std::string decayTimeString [] = { "0.2s", "0.5s", "1s", "2s", "3s", "5s", "10s" };

  fNanoText->beginFrame ( this );
  fNanoText->fontSize ( 16 );
  fNanoText->textAlign ( NanoVG::ALIGN_CENTER |NanoVG::ALIGN_MIDDLE );

  for ( int i = 0 ; i < 7 ; i++ ) {
    int x = ( int ) ( image->getWidth() * logf ( decayTime[i] / 0.1f ) / logf ( 10.0f / 0.1f ) );
    fNanoText->textBox ( x , getHeight() - 5 , 40.0f , decayTimeString[i].c_str(), nullptr );
  }

  fNanoText->textAlign ( NanoVG::ALIGN_RIGHT | NanoVG::ALIGN_MIDDLE );

  for ( int i = 0 ; i < 9 ; i++ ) {
    int y = ( int ) ( image->getHeight() * logf ( freq[i] / 50.0f ) / logf ( 20000.0f / 50.0f ) );
    fNanoText->textBox ( 0, getHeight() - y - MARGIN_TOP , 40.0f , freqStrings[i].c_str(), nullptr );
  }

  fNanoText->endFrame();
}

void Spectrogram::setParameterValue(uint32_t i, float v) {
  dsp.setParameterValue(i, v);
  dsp.mute();
  x = 0;
  sample_offset_processed = 0;
}
