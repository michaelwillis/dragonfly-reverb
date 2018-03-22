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

#ifndef DRAGONFLY_REVERB_DSP_HPP_INCLUDED
#define DRAGONFLY_REVERB_DSP_HPP_INCLUDED

#include "freeverb/earlyref.hpp"
#include "freeverb/zrev2.hpp"

class DragonflyReverbDSP {
public:
  DragonflyReverbDSP(double sampleRate, bool muteOnChange);
  float getParameterValue(uint32_t index) const;
  void  setParameterValue(uint32_t index, float value);
  void run(const float** inputs, float** outputs, uint32_t frames);
  void sampleRateChanged(double newSampleRate);

private:
  float oldParams[paramCount];
  float newParams[paramCount];

  float dry_level;
  float early_level;
  float early_send;
  float late_level;

  fv3::earlyref_f early;
  fv3::zrev2_f late;

  static const uint32_t BUFFER_SIZE = 256;
  float early_out_buffer[2][BUFFER_SIZE];
  float late_in_buffer[2][BUFFER_SIZE];
  float late_out_buffer[2][BUFFER_SIZE];
};

#endif
