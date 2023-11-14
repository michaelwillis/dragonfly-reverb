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

#include "math.h"

#include "DistrhoPlugin.hpp"
#include "DistrhoPluginInfo.h"
#include "extra/ScopedDenormalDisable.hpp"
#include "optimization/optimization.hpp"

#include "DSP.hpp"

DragonflyReverbDSP::DragonflyReverbDSP(double sampleRate) {
  model.setMuteOnChange(false);
  model.setdryr(0); // mute dry signal
  model.setwet(0); // 0dB
  model.setwidth(0.8);
  model.setLRDelay(0.3);
  model.setLRCrossApFreq(750, 4);
  model.setDiffusionApFreq(150, 4);

  for (uint32_t param = 0; param < paramCount; param++) {
    newParams[param] = DEFAULTS[param];
    oldParams[param] = -1.0;
  }

  sampleRateChanged(sampleRate);
}

float DragonflyReverbDSP::getParameterValue(uint32_t index) const {
  if (index < paramCount) {
    return newParams[index];
  }

  return 0.0;
}

void DragonflyReverbDSP::setParameterValue(uint32_t index, float value) {
  if (index < paramCount) {
    newParams[index] = value;
  }
}

void DragonflyReverbDSP::run(const float** inputs, float** outputs, uint32_t frames) {
  const ScopedDenormalDisable sdd;
  for (uint32_t index = 0; index < paramCount; index++) {
    if (d_isNotEqual(oldParams[index], newParams[index])) {
      oldParams[index] = newParams[index];
      float value = newParams[index];

      switch(index) {
        case           paramDry: dryLevel         = (value / 100.0); break;
        case           paramWet: wetLevel         = (value / 100.0); break;
        case          paramSize: model.setRSFactor   (value / 10.0);  break;
        case         paramWidth: model.setwidth      (value / 100.0); break;
        case        paramLowCut: model.setoutputhpf  (value);         break;
        case       paramHighCut: model.setoutputlpf  (value);         break;
      }

      if (index == paramProgram) {
        model.loadPresetReflection(programs[(int)value].number);
      }
    }
  }

  for (uint32_t offset = 0; offset < frames; offset += BUFFER_SIZE) {
    long int buffer_frames = frames - offset < BUFFER_SIZE ? frames - offset : BUFFER_SIZE;

    for (uint32_t i = 0; i < buffer_frames; i++) {
      input_buffer[0][i] = inputs[0][offset + i];
      input_buffer[1][i] = inputs[1][offset + i];
    }

    model.processreplace(
        const_cast<float *>(input_buffer[0]),
        const_cast<float *>(input_buffer[1]),
        output_buffer[0],
        output_buffer[1],
        buffer_frames
    );

    VMUL32FLOAT_V( dryLevel, &inputs[0][offset], dry_buffer, buffer_frames );
    VMUL32FLOAT_V( wetLevel, output_buffer[0], wet_buffer, buffer_frames );
    VSUM32FLOAT(dry_buffer, wet_buffer, &outputs[0][offset], buffer_frames );

    VMUL32FLOAT_V( dryLevel, &inputs[1][offset], dry_buffer, buffer_frames );
    VMUL32FLOAT_V( wetLevel, output_buffer[1], wet_buffer, buffer_frames );
    VSUM32FLOAT(dry_buffer, wet_buffer, &outputs[1][offset], buffer_frames );
  }
}

void DragonflyReverbDSP::sampleRateChanged(double newSampleRate) {
  sampleRate = newSampleRate;
  model.setSampleRate(newSampleRate);
}

void DragonflyReverbDSP::mute() {
  model.mute();
}
