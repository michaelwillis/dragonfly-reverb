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
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include<time.h>

Spectrogram::Spectrogram(Widget * widget, NanoVG * fNanoText, Rectangle<int> * rect) :
        Widget(widget->getParentWindow()),
        Thread("Spectrogram"),
        dsp(SPECTROGRAM_SAMPLE_RATE) {
  setWidth(rect->getWidth());
  setHeight(rect->getHeight());
  setAbsolutePos(rect->getPos());
  // setWidth(width);
  // setHeight(height);
  this->fNanoText = fNanoText;

  raster = new char[getWidth() * getHeight() * 4];
  image = new Image(raster, getWidth(), getHeight(), GL_BGRA);

  srand(time(NULL));

  // fill with transparent white
  for (uint32_t pixel = 0; pixel < getWidth() * getHeight(); pixel++) {
    raster[pixel * 4]     = (char) 255;
    raster[pixel * 4 + 1] = (char) 255;
    raster[pixel * 4 + 2] = (char) 255;
    raster[pixel * 4 + 3] = (char) 0;
  }

  white_noise = new float*[2];
  white_noise[0] = new float[SPECTROGRAM_WINDOW_SIZE];
  white_noise[1] = new float[SPECTROGRAM_WINDOW_SIZE];

  no_noise = new float*[2];
  no_noise[0] = new float[SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS];
  no_noise[1] = new float[SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS];

  dsp_result = new float*[2];
  dsp_result[0] = new float[SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS];
  dsp_result[1] = new float[SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS];

  for (uint32_t i = 0; i < SPECTROGRAM_WINDOW_SIZE; i++) {
    // Random stereo white noise from -1.0 to 1.0
    // white_noise[0][i] = (float)((rand() % 4096) - 2048) / 2048.0f;
    // white_noise[1][i] = (float)((rand() % 4096) - 2048) / 2048.0f;

    white_noise[0][i] = (float)(rand() % 4096) / 4096.0f;
    white_noise[1][i] = (float)(rand() % 4096) / 4096.0f;

    // Hann window function, per https://en.wikipedia.org/wiki/Hann_function
    window_multiplier[i] = pow(sin((M_PI * i) / (SPECTROGRAM_WINDOW_SIZE - 1)), 2);
  }

  std::cout << "White noise! "
       << white_noise[0][0] << ", "
       << white_noise[0][1] << ", "
       << white_noise[0][2] << ", "
       << white_noise[0][3] << ", "
       << white_noise[0][4] << ", "
       << white_noise[0][5] << ", "
       << white_noise[0][6] << ", "
       << white_noise[0][7] << ", "
       << white_noise[0][8] << ", "
       << white_noise[0][9] << ", "
       << white_noise[0][10];

  std::cout << "Window mult! "
       << window_multiplier[0] << ", "
       << window_multiplier[2000] << ", "
       << window_multiplier[4000];

  for (uint32_t i = 0; i < SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS; i++) {
    no_noise[0][i] = 0.0f;
    no_noise[1][i] = 0.0f;
  }

  startThread();
}

Spectrogram::~Spectrogram() {
  delete[] white_noise[0];
  delete[] white_noise[1];
  delete[] white_noise;
  delete[] no_noise[0];
  delete[] no_noise[1];
  delete[] no_noise;
  delete[] dsp_result[0];
  delete[] dsp_result[1];
  delete[] dsp_result;
  delete[] fftw_in;
  delete[] fftw_out;
  delete[] raster;
  delete image;

  stopThread(100);
}

void Spectrogram::run() {
  while(!shouldThreadExit()) {
    std::cout << "Waiting for signal!";
    signal.wait();
    std::cout << "Got signal!";

    dsp.mute();

    clock_t t1, t2, t3;

    t1 = clock();

    // Run the DSP with one window's worth of white noise.
    // Output isn't needed, but the function needs a buffer to put it
    dsp.run((const float**)white_noise, dsp_result, SPECTROGRAM_WINDOW_SIZE);

    // Now run the DSP with no sound input and collect the reverb tail.
    dsp.run((const float**)no_noise, dsp_result, SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS);

    t2 = clock();

    for (uint32_t offset = 0; offset < SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS; offset += SPECTROGRAM_WINDOW_SIZE) {

      // No window overlap, to keep things simple
      for (uint32_t window_sample = 0; window_sample < SPECTROGRAM_WINDOW_SIZE; window_sample++) {
        fftw_in[window_sample] = dsp_result[0][offset + window_sample] * window_multiplier[window_sample];
      }

      fftwf_execute(fftw_plan);

      // TOTAL HACK NEED TO BE REPLACED
      for (uint32_t y = 0; y < getHeight(); y++) {
          float val = fftw_out[y * 12][0];
          if (val < 0.0) val = 0.0 - val;
          if (val > 1.0) val = 1.0;

          char alpha = (char)(val * 200.0f);

          // char alpha = (char)((fftw_out[i * 20][0]) * 20.0f);
          uint32_t pixel = (getHeight() - y) * getWidth() + (offset * 4) / SPECTROGRAM_WINDOW_SIZE;

          raster[pixel * 4 + 3] = alpha;
          raster[pixel * 4 + 7] = alpha;
          raster[pixel * 4 + 11] = alpha;
          raster[pixel * 4 + 15] = alpha;
      }
    }

    t3 = clock();

    std::cout << "Timestamps: " << t1 << ", " << t2 << ", " << t3 << "\n";

    image->loadFromMemory(raster, getWidth(), getHeight(), GL_BGRA);
  }
}

void Spectrogram::update() {
  signal.signal();
}

void Spectrogram::onDisplay() {
  image->draw();
/*
  int freq[] = {50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
  std::string freqStrings[]  = {"50", "100", "200", "500", "1k", "2k", "5k", "10k", "20k"};
  float decayTime[] = { 0.1f,0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f };
  std::string decayTimeString [] = { "0.1", "0.2", "0.5", "1", "2", "5", "10" };

  fNanoText->beginFrame ( this );
  fNanoText->fontFaceId ( fNanoFont );
  fNanoText->fontSize ( 16 );
  fNanoText->textAlign ( NanoVG::ALIGN_LEFT |NanoVG::ALIGN_MIDDLE );
  // TODO Decide on colour
  r = 230.0f / 256;
  g = 230.0f / 256;
  b = 230.0f / 256;
  fNanoText->fillColor ( Color ( r, g, b ) );

  int y = height + y - 5;
  for ( int i = 0 ; i < 9 ; i++ )
    {
      int xx = ( int ) ( width * logf ( freq[i] / 50.0f ) / logf ( 20000.0f / 50.0f ) );
      fNanoText->textBox ( x + xx , y , 50.0f , freqStrings[i].c_str(), nullptr );
    }

  for ( int i = 0 ; i < 7 ; i++ )
    {
      int yy = ( int ) ( height * logf ( decayTime[i] / 0.1f ) / logf ( 10.0f / 0.1f ) );
      fNanoText->textBox ( x, y + height - yy - 20.0f , 50.0f , decayTimeString[i].c_str(), nullptr );
    }

  fNanoText->endFrame();
*/
}

void Spectrogram::setParameterValue(uint32_t i, float v) {
  dsp.setParameterValue(i, v);
}
