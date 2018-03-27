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

Spectrogram::Spectrogram(uint32_t width, uint32_t height) :
        Thread("Spectrogram"), dsp(SPECTROGRAM_SAMPLE_RATE, true) {
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

  startThread();
}

Spectrogram::~Spectrogram() {
  // delete[] fftw_in;
  // delete[] fftw_out;
  delete[] raster;
  delete image;

  stopThread(100);
}

void Spectrogram::run() {
  while(!shouldThreadExit()) {
    signal.wait();

    for (uint32_t pixel = 0; pixel < width * height; pixel++) {
      raster[pixel * 4 + 3] = rand() % 256; // alpha random
    }

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
