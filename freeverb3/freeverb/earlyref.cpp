/**
 *  Early Reflection Class
 *
 *  Copyright (C) 2006-2014 Teru Kamogashira
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "freeverb/earlyref.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

const long FV3_(earlyref)::preset0_size = 18;
const fv3_float_t FV3_(earlyref)::preset0_delayL[] =
{ .0043, .0215, .0225, .0268, .0270, .0298, .0458, .0485, .0572, .0587, .0595, .0612, .0707, .0708, .0726, .0741, .0753, .0797, };
const fv3_float_t FV3_(earlyref)::preset0_delayDiff[] =
{ 0.001, 0.001, 0.001, 0.001, 0.002,-0.001, 0.001,-0.001, 0.001,-0.001,-0.002, 0.001,-0.001, 0.001, 0.001, 0.001, 0.001, 0.002, };
const fv3_float_t FV3_(earlyref)::preset0_gainL[] =
{ 0.841, 0.504, 0.491, 0.379, 0.380, 0.346, 0.289, 0.272, 0.192, 0.193, 0.217, 0.181, 0.180, 0.181, 0.176, 0.142, 0.167, 0.134, };
const fv3_float_t FV3_(earlyref)::preset0_gainDiff[] =
{ 0.001, 0.002,-0.002, 0.003,-0.080, 0.000, 0.001,-0.001, 0.001,-0.001, 0.000, 0.014, 0.012,-0.015, 0.010,-0.011, 0.001,-0.001, };

const long FV3_(earlyref)::preset1_size = 6;
const fv3_float_t FV3_(earlyref)::preset1_delayL[] =
{ .0199, .0354, .0389, .0414, .0699, .0796, };
const fv3_float_t FV3_(earlyref)::preset1_delayDiff[] =
{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, };
const fv3_float_t FV3_(earlyref)::preset1_gainL[] =
{ 1.020, 0.818, 0.635, 0.719, 0.267, 0.242, };
const fv3_float_t FV3_(earlyref)::preset1_gainDiff[] =
{ 0.001, 0.002,-0.002, 0.003,-0.080, 0.001, };

const long FV3_(earlyref)::preset2_size = 4;
const fv3_float_t FV3_(earlyref)::preset2_delayL[] =
{ .009,  .0118, .0213, .0205, };
const fv3_float_t FV3_(earlyref)::preset2_delayDiff[] =
{ .0055, -0.002, -0.001, 0.0025, };
const fv3_float_t FV3_(earlyref)::preset2_gainL[] =
{ 1.35, -1.15, -1.14,  1.15, };
const fv3_float_t FV3_(earlyref)::preset2_gainDiff[] =
{ 0.01, -0.01,  0.14, -0.01, };

FV3_(earlyref)::FV3_(earlyref)()
               throw(std::bad_alloc)
{
  tapLength = 0;
  gainTableL = gainTableR = delayTableL = delayTableR = NULL;
  setdryr(0.8); setwetr(0.5); setwidth(0.2);
  setLRDelay(0.3);
  setLRCrossApFreq(750, 4);
  setDiffusionApFreq(150, 4);
  loadPresetReflection(FV3_EARLYREF_PRESET_DEFAULT);
  setoutputlpf(20000);
  setoutputhpf(4);
  mute();
}

FV3_(earlyref)::FV3_(~earlyref)()
{
  unloadReflection();
}

void FV3_(earlyref)::mute()
{
  FV3_(revbase)::mute();
  delayLineL.mute(); delayLineR.mute(); delayLtoR.mute(); delayRtoL.mute();
  allpassXL.mute(); allpassXR.mute(); allpassL2.mute(); allpassR2.mute();
}

void FV3_(earlyref)::loadPresetReflection(long program)
{
  switch(program)
    {
    case FV3_EARLYREF_PRESET_2:
      currentPreset = FV3_EARLYREF_PRESET_2;
      loadReflection(preset2_delayL, preset2_gainL, preset2_delayDiff, preset2_gainDiff, preset2_size);
      break;
    case FV3_EARLYREF_PRESET_1:
      currentPreset = FV3_EARLYREF_PRESET_1;
      loadReflection(preset1_delayL, preset1_gainL, preset1_delayDiff, preset1_gainDiff, preset1_size);
      break;
    case FV3_EARLYREF_PRESET_0:
    default:
      currentPreset = FV3_EARLYREF_PRESET_0;
      loadReflection(preset0_delayL, preset0_gainL, preset0_delayDiff, preset0_gainDiff, preset0_size);
      break;
    }
}

long FV3_(earlyref)::getCurrentPreset()
{
  return currentPreset;
}

void FV3_(earlyref)::loadUserReflection(const fv3_float_t * delayL, const fv3_float_t * gainL,
					const fv3_float_t * delayDiff, const fv3_float_t * gainDiff, long size)
		    throw(std::bad_alloc)
{
  if(delayL == NULL||gainL == NULL||delayDiff == NULL||gainDiff == NULL||size <= 0) return;
  currentPreset = -1;
  loadReflection(delayL, gainL, delayDiff, gainDiff, size);
}

void FV3_(earlyref)::loadReflection(const fv3_float_t * delayL, const fv3_float_t * gainL,
				    const fv3_float_t * delayDiff, const fv3_float_t * gainDiff, long size)
		    throw(std::bad_alloc)
{
  unloadReflection();
  try
    {
      gainTableL = new fv3_float_t[size]; gainTableR = new fv3_float_t[size];
      delayTableL = new fv3_float_t[size]; delayTableR = new fv3_float_t[size];
    }
  catch(std::bad_alloc)
    {
      std::fprintf(stderr, "earlyref::load(%ld) bad_alloc\n", size);
      delete[] gainTableL; delete[] gainTableR; delete[] delayTableL; delete[] delayTableR;
      throw;
    }
  tapLength = size;
  for(long i = 0;i < size;i ++)
    {
      gainTableL[i] = gainL[i];
      gainTableR[i] = gainL[i]+gainDiff[i];
      delayTableL[i] = getTotalFactorFs()*delayL[i];
      delayTableR[i] = getTotalFactorFs()*(delayL[i]+delayDiff[i]);
    }
  long maxLengthL = (long)(delayTableL[tapLength-1])+10;
  long maxLengthR = (long)(delayTableR[tapLength-1])+10;
  delayLineL.setsize(maxLengthL);
  delayLineR.setsize(maxLengthR);
  mute();
}

void FV3_(earlyref)::unloadReflection()
{
  if(tapLength == 0) return;
  delete[] gainTableL; delete[] gainTableR;
  delete[] delayTableL; delete[] delayTableR;
  tapLength = 0;
}

void FV3_(earlyref)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
                    throw(std::bad_alloc)
{
  if(numsamples <= 0) return;

  while(numsamples-- > 0)
    {
      *outputL = delayL(*inputL)*dry;
      *outputR = delayR(*inputR)*dry;
      fv3_float_t wetL = 0, wetR = 0;
      delayLineL.process(*inputL); delayLineR.process(*inputR);
      for(long i = 0;i < tapLength;i ++)
	{
	  wetL += gainTableL[i]*delayLineL.at(delayTableL[i]);
	  wetR += gainTableR[i]*delayLineR.at(delayTableR[i]);
	}
      // width = -1 ~ +1
      wetL = delayWL(wetL); wetR = delayWR(wetR);
      *outputL += out1_lpf(out1_hpf(allpassL2(wet1 * wetL + wet2 * allpassXL(delayRtoL(*inputR + wetR)))));
      *outputR += out2_lpf(out2_hpf(allpassR2(wet1 * wetR + wet2 * allpassXR(delayLtoR(*inputL + wetL)))));
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(earlyref)::setLRDelay(fv3_float_t value_ms)
{
  lrDelay = (long)((fv3_float_t)currentfs*value_ms/1000.0f);
  delayRtoL.setsize(lrDelay);
  delayLtoR.setsize(lrDelay);
}

fv3_float_t FV3_(earlyref)::getLRDelay()
{
  return (fv3_float_t)lrDelay*1000.0f/(fv3_float_t)currentfs;
}

void FV3_(earlyref)::setLRCrossApFreq(fv3_float_t fc, fv3_float_t bw)
{
  lrCrossApFq = fc, lrCrossApBw = bw;
  allpassXL.setAPF_RBJ(fc, bw, currentfs, FV3_BIQUAD_RBJ_BW);
  allpassXR.setAPF_RBJ(fc, bw, currentfs, FV3_BIQUAD_RBJ_BW);
}

fv3_float_t FV3_(earlyref)::getLRCrossApFreq()
{
  return lrCrossApFq;
}

void FV3_(earlyref)::setDiffusionApFreq(fv3_float_t fc, fv3_float_t bw)
{
  diffApFq = fc, diffApBw = bw;
  allpassL2.setAPF_RBJ(fc, bw, currentfs, FV3_BIQUAD_RBJ_BW);
  allpassR2.setAPF_RBJ(fc, bw, currentfs, FV3_BIQUAD_RBJ_BW);
}

fv3_float_t FV3_(earlyref)::getDiffusionApFreq()
{
  return diffApFq;
}

void FV3_(earlyref)::setoutputlpf(fv3_float_t value)
{
  long factor = getOSFactor();
  if(value < 0) value = 0;
  if(value > factor*currentfs/2) value = factor*currentfs/2;
  outputlpf = value;
  out1_lpf.setLPF_BW(outputlpf, factor*currentfs);
  out2_lpf.setLPF_BW(outputlpf, factor*currentfs);
}

fv3_float_t FV3_(earlyref)::getoutputlpf()
{
  return outputlpf;
}

void FV3_(earlyref)::setoutputhpf(fv3_float_t value)
{
  long factor = getOSFactor();
  if(value < 0) value = 0;
  if(value > factor*currentfs/2) value = factor*currentfs/2;
  outputhpf = value;
  out1_hpf.setHPF_BW(outputhpf, factor*currentfs);
  out2_hpf.setHPF_BW(outputhpf, factor*currentfs);
}

fv3_float_t FV3_(earlyref)::getoutputhpf()
{
  return outputhpf;
}

void FV3_(earlyref)::setFsFactors()
{
  FV3_(revbase)::setFsFactors();
  setLRDelay(0.2);
  setLRCrossApFreq(lrCrossApFq, lrCrossApBw);
  setDiffusionApFreq(diffApFq, diffApBw);
  loadPresetReflection(currentPreset);
}

#include "freeverb/fv3_ns_end.h"
