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
  nrev.setMuteOnChange(false);
  nrev.setSampleRate(sampleRate);

  nrevb.setMuteOnChange(false);
  nrevb.setSampleRate(sampleRate);

  strev.setMuteOnChange(false);
  strev.setdccutfreq(6);
  strev.setspinlimit(12);
  strev.setspindiff(0.15);
  strev.setSampleRate(sampleRate);

  model = &nrevb;

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

      // FIXME!
      // * Remove spin/wander?
      // * Fix diffuse for nrev and nrevb or remove.
      // * Fix damp for nrev and nrevb.
      switch(index) {
        case           paramDry: strev.setdryr       (value / 100.0);             break;
        case           paramWet: strev.setwetr       (value / 100.0);             break;
        case         paramWidth: strev.setwidth      (value / 120.0);             
	                         nrev.setwidth       (value / 120.0);
				 nrevb.setwidth      (value / 120.0);             break;
        case      paramPredelay: strev.setPreDelay   (value);
	                         nrev.setPreDelay    (value);
				 nrevb.setPreDelay   (value);                     break;
        case         paramDecay: strev.setrt60       (value);
                                 nrev.setrt60        (value);
                                 nrevb.setrt60       (value);                     break;
        case       paramDiffuse: strev.setidiffusion1(value / 120.0);
                                 strev.setidiffusion2(value / 144.0);             break;
        case          paramSpin: strev.setspin       (value);                     break;
        case        paramWander: strev.setwander     (value / 100.0);             break;
        case        paramLowCut: setInputHPF         (value);                     break;
        case       paramHighCut: setInputLPF         (value);
                                 strev.setinputdamp  (value);                     break;
        case          paramDamp: strev.setdamp       (value);
                                 strev.setoutputdamp (fmax(value * 2.0, 16000));  break;
      }

      if (index == paramAlgorithm) {
	int algorithm = value;
	if (algorithm == ALGORITHM_NREV) {
	  model = &nrev;
	} else if (algorithm == ALGORITHM_NREV_B) {
	  model = &nrevb;
	} else if (algorithm == ALGORITHM_STREV) {
	  model = &strev;
	}
      }
    }
  }

  for (uint32_t offset = 0; offset < frames; offset += BUFFER_SIZE) {
    long int buffer_frames = frames - offset < BUFFER_SIZE ? frames - offset : BUFFER_SIZE;

    for (uint32_t i = 0; i < buffer_frames; i++) {
      filtered_input_buffer[0][i] = input_hpf_0.process(inputs[0][offset + i]);
      filtered_input_buffer[1][i] = input_hpf_1.process(inputs[1][offset + i]);
    }

    model->processreplace(
        const_cast<float *>(filtered_input_buffer[0]),
        const_cast<float *>(filtered_input_buffer[1]),
        output_buffer[0],
        output_buffer[1],
        buffer_frames
    );

    for (uint32_t i = 0; i < buffer_frames; i++) {
      outputs[0][offset + i] = output_buffer[0][i];
      outputs[1][offset + i] = output_buffer[1][i];
    }
  }
}

void DragonflyReverbDSP::sampleRateChanged(double newSampleRate) {
  sampleRate = newSampleRate;
  nrev.setSampleRate(newSampleRate);
  nrevb.setSampleRate(newSampleRate);
  strev.setSampleRate(newSampleRate);
//  setInputLPF(newParams[paramLowCut]); FIXME!
  setInputHPF(newParams[paramLowCut]);  
}

void DragonflyReverbDSP::mute() {
  nrev.mute();
  nrevb.mute();
  strev.mute();
}

void DragonflyReverbDSP::setInputLPF(float freq) {
  if (freq < 0) {
    freq = 0;
  } else if (freq > sampleRate / 2.0) {
    freq = sampleRate / 2.0;
  }

  input_lpf_0.setLPF_BW(freq, sampleRate);
  input_lpf_1.setLPF_BW(freq, sampleRate);
}

void DragonflyReverbDSP::setInputHPF(float freq) {
  if (freq < 0) {
    freq = 0;
  } else if (freq > sampleRate / 2.0) {
    freq = sampleRate / 2.0;
  }

  input_hpf_0.setHPF_BW(freq, sampleRate);
  input_hpf_1.setHPF_BW(freq, sampleRate);
}
