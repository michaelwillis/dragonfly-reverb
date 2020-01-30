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

#include "DistrhoPlugin.hpp"
#include "DistrhoPluginInfo.h"
#include "DSP.hpp"
#include "math.h"

DragonflyReverbDSP::DragonflyReverbDSP(double sampleRate) {
  model.setMuteOnChange(false);
  model.setdccutfreq(6);
  model.setspinlimit(12);
  model.setspindiff(0.15);
  model.setSampleRate(sampleRate);

  for (uint32_t param = 0; param < paramCount; param++) {
    newParams[param] = presets[DEFAULT_PRESET].params[param];
    oldParams[param] = FP_NAN;
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
  for (uint32_t index = 0; index < paramCount; index++) {
    if (d_isNotEqual(oldParams[index], newParams[index])) {
      oldParams[index] = newParams[index];
      float value = newParams[index];

      switch(index) {
        case           paramDry: model.setdryr       (value / 100.0); break;
        case           paramWet: model.setwetr       (value / 100.0); break;
        case         paramWidth: model.setwidth      (value / 120.0); break;
        case      paramPredelay: model.setPreDelay   (value);         break;
        case         paramDecay: model.setrt60       (value);         break;
// Maybe implement this?
//      case       paramDiffuse: model.setidiffusion1(value / 120.0);
//                               model.setidiffusion2(value / 144.0);
//                               model.setdiffusion1 (value / 128.0);
//                               model.setdiffusion2 (value / 180.0); break;
        case          paramSpin: model.setspin       (value);
        case        paramWander: model.setwander     (value);

        case       paramHighCut: model.setinputdamp  (value);         break;
        case          paramDamp: model.setdamp       (value);
                                 model.setoutputdamp (fmax(value * 2.0, 16000)); break;
      }
    }
  }

  model.processreplace(
    const_cast<float *>(inputs[0]),
    const_cast<float *>(inputs[1]),
    outputs[0],
    outputs[1],
    frames
  );
}

void DragonflyReverbDSP::sampleRateChanged(double newSampleRate) {
  sampleRate = newSampleRate;
  model.setSampleRate(newSampleRate);
}

void DragonflyReverbDSP::mute() {
  model.mute();
}
