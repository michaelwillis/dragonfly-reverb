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
#include "freeverb/nrev.hpp"
#include "freeverb/nrevb.hpp"
#include "freeverb/strev.hpp"


class NRev : public fv3::nrev_f {
public:
  NRev();
  void setDampLpf(float value);
  virtual void mute();
  virtual void setFsFactors();
  virtual void processloop2(long count, float *inputL,  float *inputR, float *outputL, float *outputR);
private:
  float dampLpf;
  fv3::iir_1st_f dampLpfL, dampLpfR;
};


class NRevB : public fv3::nrevb_f {
public:
  NRevB();
  void setDampLpf(float value);
  virtual void mute();
  virtual void setFsFactors();
  virtual void processloop2(long count, float *inputL,  float *inputR, float *outputL, float *outputR);
private:
  float dampLpf;
  fv3::iir_1st_f dampLpfL, dampLpfR;
};


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

  static const uint32_t BUFFER_SIZE = 256;

  fv3::iir_1st_f input_lpf_0, input_lpf_1, input_hpf_0, input_hpf_1;

  fv3::revbase_f *model; // points to one of the following:
  NRev nrev;
  NRevB nrevb;
  fv3::strev_f strev;

  float filtered_input_buffer[2][BUFFER_SIZE];
  float output_buffer[2][BUFFER_SIZE];

  void setInputLPF(float freq);
  void setInputHPF(float freq);
};

#endif
