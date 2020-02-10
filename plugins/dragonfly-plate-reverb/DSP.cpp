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

NRev::NRev() : fv3::nrev_f() { }

void NRev::setDampLpf(float value) {
  dampLpf = limFs2(value);
  dampLpfL.setLPF_BW(dampLpf, getTotalSampleRate());
  dampLpfR.setLPF_BW(dampLpf, getTotalSampleRate());
}

void NRev::mute() {
  fv3::nrev_f::mute();
  dampLpfL.mute();
  dampLpfR.mute();
}

void NRev::setFsFactors() {
  fv3::nrev_f::setFsFactors();
  setDampLpf(dampLpf);
}

void NRev::processloop2(long count, float *inputL, float *inputR, float *outputL, float *outputR) {
  float outL, outR;
  while(count-- > 0)
    {
      outL = outR = 0;
      hpf = damp3_1*inDCC(*inputL + *inputR) - damp3*hpf;
      UNDENORMAL(hpf);

      hpf *= FV3_NREV_SCALE_WET;
      
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outL += combL[i]._process(hpf);
      for(long i = 0;i < 3;i ++) outL = allpassL[i]._process_ov(outL);
      lpfL = dampLpfL(damp2*lpfL + damp2_1*outL); UNDENORMAL(lpfL);
      outL = allpassL[3]._process_ov(lpfL);
      outL = allpassL[5]._process_ov(outL);
      outL = delayWL(lLDCC(outL));
      
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outR += combR[i]._process(hpf);
      for(long i = 0;i < 3;i ++) outR = allpassR[i]._process_ov(outR);
      lpfR = dampLpfR(damp2*lpfR + damp2_1*outR); UNDENORMAL(lpfR);
      outR = allpassR[3]._process_ov(lpfR);
      outR = allpassR[6]._process_ov(outR);
      outR = delayWR(lRDCC(outR));
      
      *outputL = outL*wet1 + outR*wet2 + delayL(*inputL)*dry;
      *outputR = outR*wet1 + outL*wet2 + delayR(*inputR)*dry;
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}


NRevB::NRevB() : fv3::nrevb_f() { }

void NRevB::setDampLpf(float value)
{
  dampLpf = limFs2(value);
  dampLpfL.setLPF_BW(dampLpf, getTotalSampleRate());
  dampLpfR.setLPF_BW(dampLpf, getTotalSampleRate());
}

void NRevB::mute() {
  fv3::nrevb_f::mute();
  dampLpfL.mute();
  dampLpfR.mute();
}

void NRevB::setFsFactors() {
  fv3::nrevb_f::setFsFactors();
  setDampLpf(dampLpf);
}

void NRevB::processloop2(long count, float *inputL, float *inputR, float *outputL, float *outputR) {
  float outL, outR, tmpL, tmpR;
  while(count-- > 0)
    {
      hpf = damp3_1*inDCC.process(*inputL + *inputR) - damp3*hpf; UNDENORMAL(hpf);
      outL = outR = tmpL = tmpR = hpf;

      outL += apfeedback*lastL;
      lastL += -1*apfeedback*outL;

      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outL += combL[i]._process(tmpL);
      for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++) outL += comb2L[i]._process(tmpL);
      for(long i = 0;i < 3;i ++) outL = allpassL[i]._process(outL);
      for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++) outL = allpass2L[i]._process(outL);
      lpfL = dampLpfL(damp2*lpfL + damp2_1*outL); UNDENORMAL(lpfL);
      outL = allpassL[3]._process(lpfL); outL = allpassL[5]._process(outL);
      outL = lLDCC(outL);

      outR += apfeedback*lastR;
      lastR += -1*apfeedback*outR;
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outR += combR[i]._process(tmpR);
      for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++) outR += comb2R[i]._process(tmpR);
      for(long i = 0;i < 3;i ++) outR = allpassR[i]._process(outR);
      for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++) outR = allpass2R[i]._process(outR);
      lpfR = dampLpfR(damp2*lpfR + damp2_1*outR); UNDENORMAL(lpfR);
      outR = allpassR[3]._process(lpfR); outR = allpassL[6]._process(outR);
      outR = lRDCC(outR);
      
      lastL = FV3_NREVB_SCALE_WET*delayWL(lastL);
      lastR = FV3_NREVB_SCALE_WET*delayWR(lastR);
      *outputL = lastL*wet1 + lastR*wet2 + delayL(*inputL)*dry;
      *outputR = lastR*wet1 + lastL*wet2 + delayR(*inputR)*dry;
      lastL = outL; lastR = outR;
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}


DragonflyReverbDSP::DragonflyReverbDSP(double sampleRate) {
  input_lpf_0.mute();
  input_lpf_1.mute();
  input_hpf_0.mute();
  input_hpf_1.mute();

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

      // TODO:
      // * Remove spin/wander?
      // * Try exaggerating diffuse for nrevb, tank
      switch(index) {
        case           paramDry: nrev.setdryr        (value / 100.0);
                                 nrevb.setdryr       (value / 100.0);
                                 strev.setdryr       (value / 100.0);             break;
        case           paramWet: nrev.setwetr        (value / 100.0);
				 nrevb.setwetr       (value / 100.0);
                                 strev.setwetr       (value / 100.0);             break;
        case         paramWidth: strev.setwidth      (value / 120.0);             
	                         nrev.setwidth       (value / 120.0);
				 nrevb.setwidth      (value / 120.0);             break;
        case      paramPredelay: strev.setPreDelay   (value);
	                         nrev.setPreDelay    (value);
				 nrevb.setPreDelay   (value);                     break;
        case         paramDecay: strev.setrt60       (value);
                                 nrev.setrt60        (value);
                                 nrevb.setrt60       (value);                     break;
        case       paramDiffuse: nrev.setfeedback    (value / 200.0 + 0.25);
                                 nrevb.setfeedback   (value / 180.0);
                                 strev.setidiffusion1(value / 120.0);
                                 strev.setidiffusion2(value / 144.0);             break;
        case          paramSpin: strev.setspin       (value);                     break;
        case        paramWander: strev.setwander     (value / 100.0);             break;
        case        paramLowCut: setInputHPF         (value);                     break;
        case       paramHighCut: setInputLPF         (value);
                                 strev.setinputdamp  (value);                     break;
        case          paramDamp: nrev.setDampLpf     (value);
                                 nrevb.setDampLpf    (value);
				 strev.setdamp       (value);
                                 strev.setoutputdamp (fmax(value * 2.0, 16000));  break;
      }

      if (index == paramAlgorithm) {
	fv3::revbase_f *previous = model;
	int algorithm = value;
	if (algorithm == ALGORITHM_NREV) {
	  model = &nrev;
	} else if (algorithm == ALGORITHM_NREV_B) {
	  model = &nrevb;
	} else if (algorithm == ALGORITHM_STREV) {
	  model = &strev;
	}
	if (model != previous) {
	  previous->mute();
	}
      }
    }
  }

  for (uint32_t offset = 0; offset < frames; offset += BUFFER_SIZE) {
    long int buffer_frames = frames - offset < BUFFER_SIZE ? frames - offset : BUFFER_SIZE;

    for (uint32_t i = 0; i < buffer_frames; i++) {
      filtered_input_buffer[0][i] = input_lpf_0.process(input_hpf_0.process(inputs[0][offset + i]));
      filtered_input_buffer[1][i] = input_lpf_1.process(input_hpf_1.process(inputs[1][offset + i]));
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
  setInputLPF(newParams[paramLowCut]);
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
