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
#include "DragonflyReverbDSP.hpp"

DragonflyReverbDSP::DragonflyReverbDSP(double sampleRate) {
  early.setSampleRate(sampleRate);
  early.setMuteOnChange(false);
  early.setdryr(0); // mute dry signal
  early.setwet(0); // 0dB
  early_send = 0.20;

  late.setSampleRate(sampleRate);
  late.setMuteOnChange(false);
  late.setdryr(0); // mute dry signal
  late.setwet(0); // 0dB

  for (uint32_t param = 0; param < paramCount; param++) {
    newParams[param] = presets[DEFAULT_PRESET].params[param];
    oldParams[param] = FP_NAN;
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

/*
void ProGRev::setParameter(VstInt32 index, float value, bool vstp)
{
  // The VST host stores parameters in float(0.0-1.0).
  if(vstp) value = param2model(index, value);
  // stores parameters to current Program.
  ProGRevProgram * p = &programs[curProgram];
  long factorp = 1;
  switch (index)
    {
    case KOFactor:
      p->fOFactor = value;
      factorp = std::ceil(value); if(factorp == 0) factorp = 1;
      if((value-std::floor(value)) > 0.5||value == 4)
        {
          ModeProG2 = false;
          dsp_prev->setReverbType(FV3_REVTYPE_PROG);
        }
      else
        {
          ModeProG2 = true;
          dsp_prev->setReverbType(FV3_REVTYPE_PROG2);
        }

      if(factorp != dsp_prev->getOSFactor())
        {
          lock();
          dsp_prev->setOSFactor((int)factorp, converter_type);
          unlock();
        }
      break;
    case KDry:
      dryDB = (p->fDry = value);
      break;
    case KERefWet:
      erDB = (p->fERefWet = value);
      break;
    case KERefWidth:
      dsp_eref->setwidth(p->fERefWidth = value);
      break;
    case KERefFactor:
      lock();
      dsp_eref->setRSFactor(p->fERefFactor = value);
      unlock();
      break;
    case KERtoLate:
      vERtoLate = (p->fERtoLate = value);
      break;

    case KWet:
      dsp_prev->setwet(p->fWet = value);
      break;
    case KWidth:
      dsp_prev->setwidth(p->fWidth = value);
      break;
    case KDelay:
      lock();
      dsp_prev->setPreDelay(p->fDelay = value);
      unlock();
      break;
    case KRT60:
      p->fRT60 = value;
      // for exception
      if(value >= 0.1) dsp_prev->setrt60(value);
      else dsp_prev->setrt60(0.1);
      break;
    case KInputLPF:
      dsp_prev->setinputdamp(p->fInputLPF = value);
      break;
    case KDampLPF:
      dsp_prev->setdamp(p->fDampLPF = value);
      break;
    case KBassLPF:
      dsp_prev->setdamp2(p->fBassLPF = value);
      break;
    case KBassFactor:
      dsp_prev->setbassboost(p->fBassFactor = value);
      break;
    case KOutputLPF:
      dsp_prev->setoutputdamp(p->fOutputLPF = value);
      break;
    case KOutputBW:
      dsp_prev->setoutputdampbw(p->fOutputBW = value);
      break;
    case KSpin:
      dsp_prev->setspin(p->fSpin = value);
      dsp_prev->setspin2(std::sqrt(100.-(10.-value)*(10.-value))/2.);
      break;
    case KWander:
      // The values are limited to reduce buggy signal clips.
      dsp_prev->setwander(0.1 + (p->fWander = value)/2.);
      dsp_prev->setwander2(0.1 + value/2.);
      break;
    default:
      break;
    }
}
*/

void DragonflyReverbDSP::run(const float** inputs, float** outputs, uint32_t frames) {
  for (uint32_t index = 0; index < paramCount; index++) {
    if (d_isNotEqual(oldParams[index], newParams[index])) {
      oldParams[index] = newParams[index];
      float value = newParams[index];

      switch(index) {
        case     paramDry_level: dry_level        = (value / 100.0); break;
        case   paramEarly_level: early_level      = (value / 100.0); break;
        case    paramLate_level: late_level       = (value / 100.0); break;
        case    paramOversample: early.setOSFactor  (value);
                                 late.setOSFactor   (value);         break;
        case         paramWidth: early.setwidth     (value / 100.0);
                                 late.setwidth      (value / 100.0); break;
        case      paramPredelay: late.setPreDelay   (value);         break;
        case         paramDecay: late.setrt60       (value);         break;
        case       paramDiffuse: late.setidiffusion1(value / 120.0);
                                 late.setodiffusion1(value / 120.0); break;
        case    paramDampenFreq: late.setdamp       (value);         break;
        case          paramSpin: late.setspin       (value);         break;
        case        paramWander: late.setwander     (value / 200.0 + 0.1);
                                 late.setwander2    (value / 200.0 + 0.1);
        case     paramBoostFreq: late.setdamp2      (value);         break;
        case   paramBoostFactor: late.setbassboost  (value / 100.0); break;
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
        buffer_frames
    );

    for (uint32_t i = 0; i < buffer_frames; i++) {
      late_in_buffer[0][i] = early_send * early_out_buffer[0][i] + inputs[0][offset + i];
      late_in_buffer[1][i] = early_send * early_out_buffer[1][i] + inputs[1][offset + i];
    }

    late.processreplace(
      const_cast<float *>(late_in_buffer[0]),
      const_cast<float *>(late_in_buffer[1]),
      late_out_buffer[0],
      late_out_buffer[1],
      buffer_frames
    );

    for (uint32_t i = 0; i < buffer_frames; i++) {
      outputs[0][offset + i] =
        dry_level   * inputs[0][offset + i]  +
        early_level * early_out_buffer[0][i] +
        late_level  * late_out_buffer[0][i];

      outputs[1][offset + i] =
        dry_level   * inputs[1][offset + i]  +
        early_level * early_out_buffer[1][i] +
        late_level  * late_out_buffer[1][i];
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
