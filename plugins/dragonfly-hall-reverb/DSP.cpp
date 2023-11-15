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

#include "DistrhoPlugin.hpp"
#include "DistrhoPluginInfo.h"
#include "extra/ScopedDenormalDisable.hpp"
#ifdef USE_PLUGIN_SIMD
#include "optimization/optimization.hpp"
#endif

#include "DSP.hpp"

DragonflyReverbDSP::DragonflyReverbDSP(double sampleRate) {
  early.loadPresetReflection(FV3_EARLYREF_PRESET_1);
  early.setMuteOnChange(false);
  early.setdryr(0); // mute dry signal
  early.setwet(0); // 0dB
  early.setwidth(0.8);
  early.setLRDelay(0.3);
  early.setLRCrossApFreq(750, 4);
  early.setDiffusionApFreq(150, 4);
  early.setSampleRate(sampleRate);
  early_send = 0.20;

  late.setMuteOnChange(false);
  late.setwet(0); // 0dB
  late.setdryr(0); // mute dry signal
  late.setwidth(1.0);
  late.setSampleRate(sampleRate);

  for (uint32_t param = 0; param < paramCount; param++) {
    newParams[param] = banks[DEFAULT_BANK].presets[DEFAULT_PRESET].params[param];
    oldParams[param] = -1.0;
  }
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
        case           paramDry: dryLevel        = (value / 100.0); break;
        case         paramEarly: earlyLevel      = (value / 100.0); break;
        case          paramLate: lateLevel       = (value / 100.0); break;
        case          paramSize: early.setRSFactor  (value / 10.0);
                                 late.setRSFactor   (value / 80.0);  break;
        case         paramWidth: early.setwidth     (value / 100.0);
                                 late.setwidth      (value / 100.0); break;
        case      paramPredelay:
          // Freeverb doesn't handle zero predelay properly
          // Instead of modifying the library, avoid it here
          if (value < 0.1) {
            value = 0.1;
          }
          late.setPreDelay   (value);
          break;
        case       paramDiffuse: late.setidiffusion1(value / 140.0);
                                 late.setapfeedback (value / 140.0); break;
        case       paramLowCut:  early.setoutputhpf (value);
                                 late.setoutputhpf  (value);         break;
        case     paramLowXover:  late.setxover_low  (value);         break;
        case      paramLowMult:  late.setrt60_factor_low(value);     break;
        case      paramHighCut:  early.setoutputlpf (value);
                                 late.setoutputlpf  (value);         break;
        case    paramHighXover:  late.setxover_high (value);         break;
        case     paramHighMult:  late.setrt60_factor_high(value);    break;
        case          paramSpin: late.setspin       (value);         break;
        case        paramWander: late.setwander     (value);         break;
        case         paramDecay: late.setrt60       (value);         break;
        case     paramEarlySend: early_send       = (value / 100.0); break;
        case    paramModulation:
          // Avoid ill effects of zero modulation, set to one tenth of a percent instead
          float mod = value == 0.0 ? 0.001 : value / 100.0;
          late.setspinfactor (mod);
          late.setlfofactor  (mod);
          break;
      }
    }
  }

  for (uint32_t offset = 0; offset < frames; offset += BUFFER_SIZE) {
    long int buffer_frames = frames - offset < BUFFER_SIZE ? frames - offset : BUFFER_SIZE;

    early.processreplace(
      const_cast<float *>(inputs[0] + offset),
      const_cast<float *>(inputs[1] + offset),
      early_out_buffer[0],
      early_out_buffer[1],
      buffer_frames);
    
#ifdef USE_PLUGIN_SIMD
    VMADD32FLOAT_V(early_send, early_out_buffer[0], &inputs[0][offset], late_in_buffer[0], buffer_frames );
    VMADD32FLOAT_V(early_send, early_out_buffer[1], &inputs[1][offset], late_in_buffer[1], buffer_frames );
#else
    for (uint32_t i = 0; i < buffer_frames; i++) {
      late_in_buffer[0][i] = early_send * early_out_buffer[0][i] + inputs[0][offset + i];
      late_in_buffer[1][i] = early_send * early_out_buffer[1][i] + inputs[1][offset + i];
    }
#endif
    
    late.processreplace(
      const_cast<float *>(late_in_buffer[0]),
      const_cast<float *>(late_in_buffer[1]),
      late_out_buffer[0],
      late_out_buffer[1],
      buffer_frames);

#ifdef USE_PLUGIN_SIMD      
    VMUL32FLOAT_V(dryLevel, &inputs[0][offset], &outputs[0][offset], buffer_frames);
    VMUL32FLOAT_V(dryLevel, &inputs[1][offset], &outputs[1][offset], buffer_frames);
#else
    for (uint32_t i = 0; i < buffer_frames; i++) {
      outputs[0][offset + i] = dryLevel   * inputs[0][offset + i];
      outputs[1][offset + i] = dryLevel   * inputs[1][offset + i];
    }
#endif

    if( earlyLevel > 0.0 ){
#ifdef USE_PLUGIN_SIMD      
      VMADD32FLOAT_V(earlyLevel, early_out_buffer[0], &outputs[0][offset], &outputs[0][offset], buffer_frames );
      VMADD32FLOAT_V(earlyLevel, early_out_buffer[1], &outputs[1][offset], &outputs[1][offset], buffer_frames );
#else
      for (uint32_t i = 0; i < buffer_frames; i++) {
        outputs[0][offset + i] += earlyLevel * early_out_buffer[0][i];
        outputs[1][offset + i] += earlyLevel * early_out_buffer[1][i];
      }
#endif
    }
    
    if( lateLevel > 0.0 ){
#ifdef USE_PLUGIN_SIMD
      VMADD32FLOAT_V(lateLevel, late_out_buffer[0], &outputs[0][offset], &outputs[0][offset], buffer_frames );
      VMADD32FLOAT_V(lateLevel, late_out_buffer[1], &outputs[1][offset], &outputs[1][offset], buffer_frames );
#else
      for (uint32_t i = 0; i < buffer_frames; i++) {
        outputs[0][offset + i] += lateLevel  * late_out_buffer[0][i];
        outputs[1][offset + i] += lateLevel  * late_out_buffer[1][i];
      }
#endif
    }
  }
}

void DragonflyReverbDSP::sampleRateChanged(double newSampleRate) {
    early.setSampleRate(newSampleRate);
    late.setSampleRate(newSampleRate);
}

void DragonflyReverbDSP::mute() {
  early.mute();
  late.mute();
}
