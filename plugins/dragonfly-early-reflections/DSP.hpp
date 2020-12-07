/*
 * Dragonfly Reverb, copyright (c) 2019 Michael Willis, Rob van den Berg
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

#include "AbstractDSP.hpp"
#include "freeverb/earlyref.hpp"


class DragonflyReverbDSP : public AbstractDSP {
public:
  DragonflyReverbDSP(double sampleRate);
  float getParameterValue(uint32_t index) const;
  void  setParameterValue(uint32_t index, float value);
  void  run(const float** inputs, float** outputs, uint32_t frames);
  void  sampleRateChanged(double newSampleRate);
  void  mute();

private:
  float oldParams[paramCount];
  float newParams[paramCount];

  double sampleRate;

  float dry_level = 0.0;
  float wet_level = 0.0;

  static const uint32_t BUFFER_SIZE = 256;

  fv3::earlyref_f model;

  float input_buffer[2][BUFFER_SIZE];
  float output_buffer[2][BUFFER_SIZE];

  void setInputLPF(float freq);
  void setInputHPF(float freq);
};

#endif
