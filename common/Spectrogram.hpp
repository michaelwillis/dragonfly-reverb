/*
 * Dragonfly Reverb, a hall-style reverb plugin
 * Copyright (c) 2018 Michael Willis, Rob van den Berg
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

#include "AbstractDSP.hpp"
#include "ImageWidgets.hpp"
#include "NanoVG.hpp"

extern "C" {
#include "kiss_fft/kiss_fftr.h"
}

#define SPECTROGRAM_SAMPLE_RATE 40960
#define SPECTROGRAM_WINDOW_SIZE 8192
#define SPECTROGRAM_MIN_SECONDS 0.2f
#define SPECTROGRAM_MAX_SECONDS 8.0f
#define SPECTROGRAM_MIN_FREQ    100.0f
#define SPECTROGRAM_MAX_FREQ    16000.0f

#define MARGIN_TOP    10
#define MARGIN_RIGHT  15
#define MARGIN_BOTTOM 20
#define MARGIN_LEFT   50

class Spectrogram : public SubWidget {
  public:
  Spectrogram(Widget* widget, NanoVG * fNanoText, DGL::Rectangle<int> * rect, AbstractDSP * dsp);
    ~Spectrogram();

    void update();

    void setParameterValue(uint32_t i, float v);
    void onDisplay();
    void uiIdle();

  private:
    AbstractDSP * dsp;
    char * raster;
    Image * image;

    NanoVG * fNanoText;

    float ** white_noise;
    float ** silence;
    float ** dsp_output;

    // Mono for now, but maybe should change this to stereo
    float reverb_results[SPECTROGRAM_SAMPLE_RATE * (int) SPECTROGRAM_MAX_SECONDS + SPECTROGRAM_WINDOW_SIZE * 2];
    float window_multiplier[SPECTROGRAM_WINDOW_SIZE];

    // x coordinate of current column being rendered
    uint32_t x;

    // index of last sample that was processed through the reverb dsp
    // in other words, how much of reverb_results is accurately populated
    uint32_t sample_offset_processed;

    kiss_fftr_cfg   fft_cfg;
    kiss_fft_scalar fft_in[SPECTROGRAM_WINDOW_SIZE];
    kiss_fft_cpx    fft_out[SPECTROGRAM_WINDOW_SIZE];
};

#endif
