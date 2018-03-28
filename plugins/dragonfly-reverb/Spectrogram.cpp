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

Spectrogram::Spectrogram(uint32_t width, uint32_t height) :
        Thread("Spectrogram"), dsp(SPECTROGRAM_SAMPLE_RATE) {
  this->width = width;
  this->height = height;

  raster = new char[width * height * 4];
  image = new Image(raster, width, height, GL_BGRA);

  srand(time(NULL));

  // fill with white
  for (uint32_t pixel = 0; pixel < width * height; pixel++) {
    raster[pixel * 4]     = (char) 255;
    raster[pixel * 4 + 1] = (char) 255;
    raster[pixel * 4 + 2] = (char) 255;
    raster[pixel * 4 + 3] = (char) (rand() % 256); // alpha random
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
    white_noise[0][i] = (float)((rand() % 4096) - 2048) / 2048.0f;
    white_noise[1][i] = (float)((rand() % 4096) - 2048) / 2048.0f;

    // Hann window function, per https://en.wikipedia.org/wiki/Hann_function
    window_multiplier[i] = pow(sin((M_PI * i) / (SPECTROGRAM_WINDOW_SIZE - 1)), 2);
  }

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
    signal.wait();

    dsp.mute();

    // Run the DSP with one window's worth of white noise.
    // Output isn't needed, but the function needs a buffer to put it
    dsp.run((const float**)white_noise, dsp_result, SPECTROGRAM_WINDOW_SIZE);

    // Now run the DSP with no sound input and collect the reverb tail.
    dsp.run((const float**)no_noise, dsp_result, SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS);

    for (uint32_t offset = 0; offset < SPECTROGRAM_SAMPLE_RATE * SPECTROGRAM_MAX_SECONDS; offset += SPECTROGRAM_WINDOW_SIZE) {

      // No window overlap, to keep things simple
      for (uint32_t window_sample = 0; window_sample < SPECTROGRAM_WINDOW_SIZE; window_sample++) {
        fftw_in[window_sample] = dsp_result[0][offset + window_sample] * window_multiplier[window_sample];
      }

      fftwf_execute(fftw_plan);

      // TOTAL HACK NEED TO BE REPLACED
      for (uint32_t i = 0; i < 120; i++) {
          uint32_t pixel = i * width + offset / SPECTROGRAM_WINDOW_SIZE;
          char alpha = (char)((fftw_out[i * 20][0] + 1.0f) * 127.0f);
          raster[pixel * 4 + 3] = alpha;
      }
    }

    // for (uint32_t pixel = 0; pixel < width * height; pixel++) {
    //  raster[pixel * 4 + 3] = rand() % 256; // alpha random
    // }

    image->loadFromMemory(raster, width, height, GL_BGRA);
  }
}

void Spectrogram::update() {
  signal.signal();
  /*for (uint32_t pixel = 0; pixel < width * height; pixel++) {
    raster[pixel * 4 + 3] = rand() % 256; // alpha random
  }*/
}

void Spectrogram::drawAt(uint32_t x, uint32_t y) {
  image->drawAt(x, y);
}
