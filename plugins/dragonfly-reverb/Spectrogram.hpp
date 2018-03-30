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

#ifndef SPECTROGRAM_HPP_INCLUDED
#define SPECTROGRAM_HPP_INCLUDED

#include "DistrhoPluginInfo.h"
#include "DragonflyReverbDSP.hpp"
#include "ImageWidgets.hpp"
#include "NanoVG.hpp"
#include "extra/Thread.hpp"
#include "extra/Mutex.hpp"
#include <fftw3.h>

#define SPECTROGRAM_SAMPLE_RATE 48000
#define SPECTROGRAM_WINDOW_SIZE 4800
#define SPECTROGRAM_MAX_SECONDS 10
#define MARGIN 20

class Spectrogram : public Thread, public Widget {
  public:
    Spectrogram(Widget* widget, NanoVG * fNanoText, Rectangle<int> * rect);
    ~Spectrogram();

    void run();
    void update();

    void setParameterValue(uint32_t i, float v);
    void onDisplay();

  private:
    DragonflyReverbDSP dsp;
    Signal signal;
    char * raster;
    Image * image;

    NanoVG * fNanoText;

    float ** white_noise;
    float ** no_noise;
    float ** dsp_result;
    float window_multiplier[SPECTROGRAM_WINDOW_SIZE];

    float         *fftw_in  = (float*)         fftwf_malloc(SPECTROGRAM_WINDOW_SIZE * sizeof(float));
    fftwf_complex *fftw_out = (fftwf_complex*) fftwf_malloc(SPECTROGRAM_WINDOW_SIZE * sizeof(fftwf_complex));
    fftwf_plan    fftw_plan = fftwf_plan_dft_r2c_1d(SPECTROGRAM_WINDOW_SIZE, fftw_in, fftw_out, FFTW_ESTIMATE);
};

#endif
