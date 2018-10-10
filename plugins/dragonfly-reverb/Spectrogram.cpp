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
#include <time.h>
#include <iostream>

Spectrogram::Spectrogram(Widget * widget, NanoVG * fNanoText, DGL::Rectangle<int> * rect) :
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

    fft_real[i] = 0.0;
    fft_imag[i] = 0.0;
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

  delete[] raster;
  delete image;
}

void Spectrogram::uiIdle() {
  uint32_t max_x = x + 50;
  if (max_x > image->getWidth()) {
    max_x = image->getWidth();
  }
  while(x < max_x) {
    // Calculate time in seconds, then determine where that is in the dsp_output buffer
    float time = pow(M_E, (float) x * logf ( (float)SPECTROGRAM_MAX_SECONDS / SPECTROGRAM_MIN_SECONDS ) / image->getWidth()) * SPECTROGRAM_MIN_SECONDS;
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
        fft_real[window_sample] = reverb_results[sample_offset + window_sample] * window_multiplier[window_sample];
        fft_imag[window_sample] = 0.0;
      }

      Fft_transform(fft_real, fft_imag, (size_t) SPECTROGRAM_WINDOW_SIZE);

      for (uint32_t y = 0; y < image->getHeight(); y++) {
          float freq = powf(M_E, (float) y * logf ( SPECTROGRAM_MAX_FREQ / SPECTROGRAM_MIN_FREQ ) / (float) image->getHeight()) * SPECTROGRAM_MIN_FREQ;
          int fft_index = freq / (float)(SPECTROGRAM_SAMPLE_RATE / SPECTROGRAM_WINDOW_SIZE) + 1;

          float val = fft_real[fft_index];
          if (val < 0.0) val = 0.0 - val;
          if (val > 8.0) val = 8.0;

          char alpha = (char)(sqrt(val * 2) * 63.0f);

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

  int freq[] = {125, 250, 500, 1000, 2000, 4000, 8000, 16000};
  std::string freqStrings[]  = {"125 Hz", "250 Hz", "500 Hz", "1 kHz", "2 kHz", "4 kHz", "8 kHz", "16 kHz"};
  float decayTime[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f };
  std::string decayTimeString [] = { ".25 s", ".5 s", "1 s", "2 s", "4 s", "8 s" };

  fNanoText->beginFrame ( this );
  fNanoText->fontSize ( 16 );
  fNanoText->textAlign ( NanoVG::ALIGN_CENTER | NanoVG::ALIGN_MIDDLE );

  for ( int i = 0 ; i < 6 ; i++ ) {
    int x = ( int ) ( image->getWidth() * logf ( decayTime[i] / SPECTROGRAM_MIN_SECONDS ) / logf ( SPECTROGRAM_MAX_SECONDS / SPECTROGRAM_MIN_SECONDS ) );
    fNanoText->textBox ( x , getHeight() - 5 , 40.0f , decayTimeString[i].c_str(), nullptr );
  }

  fNanoText->textAlign ( NanoVG::ALIGN_RIGHT | NanoVG::ALIGN_MIDDLE );

  for ( int i = 0 ; i < 8 ; i++ ) {
    int y = ( int ) ( image->getHeight() * logf ( freq[i] / SPECTROGRAM_MIN_FREQ ) / logf ( SPECTROGRAM_MAX_FREQ / SPECTROGRAM_MIN_FREQ ) );
    fNanoText->textBox ( 0, getHeight() - y - MARGIN_TOP , 40.0f , freqStrings[i].c_str(), nullptr );
  }

  fNanoText->endFrame();
}

void Spectrogram::setParameterValue(uint32_t i, float v) {
  if (i == paramDry_level) {
    v = 0.0f;
  }

  dsp.setParameterValue(i, v);
  dsp.mute();
  x = 0;
  sample_offset_processed = 0;
}
