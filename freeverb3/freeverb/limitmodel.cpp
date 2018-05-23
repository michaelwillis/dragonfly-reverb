/**
 *  Limiter model implementation
 *
 *  Copyright (C) 2006-2018 Teru Kamogashira
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

#include "freeverb/limitmodel.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(limitmodel)::FV3_(limitmodel)()
{
  currentfs = 48000;
  setRMS(0); setLookahead(5); setLookaheadRatio(1); setAttack(0); setRelease(10);
  setThreshold(-1); setCeiling(0); stereoLink = true;
  mute();
}

fv3_float_t FV3_(limitmodel)::getSampleRate()
{
  return currentfs;
}

void FV3_(limitmodel)::setSampleRate(fv3_float_t fs)
{
  if(fs <= 0) return;
  currentfs = fs;
  setRMS(getRMS());
  setLookahead(getLookahead());
  setAttack(getAttack());
  setRelease(getRelease());
  mute();
}

fv3_float_t FV3_(limitmodel)::getRMS()
{
  return RMS;
}

void FV3_(limitmodel)::setRMS(fv3_float_t msec)
{
  RMS = msec;
  limitL.setRMS(FV3_(utils)::ms2sample(RMS,currentfs));
  limitR.setRMS(FV3_(utils)::ms2sample(RMS,currentfs));
}

fv3_float_t FV3_(limitmodel)::getLookahead()
{
  return Lookahead;
}

void FV3_(limitmodel)::setLookahead(fv3_float_t msec)
{
  Lookahead = msec;
  lookaL.setsize(FV3_(utils)::ms2sample(Lookahead,currentfs));
  lookaR.setsize(FV3_(utils)::ms2sample(Lookahead,currentfs));
  limitL.setLookahead(FV3_(utils)::ms2sample(Lookahead,currentfs));
  limitR.setLookahead(FV3_(utils)::ms2sample(Lookahead,currentfs));
}

fv3_float_t FV3_(limitmodel)::getLookaheadRatio()
{
  return LookaheadRatio;
}

void FV3_(limitmodel)::setLookaheadRatio(fv3_float_t value)
{
  LookaheadRatio = value;
  limitL.setLookaheadRatio(LookaheadRatio);
  limitR.setLookaheadRatio(LookaheadRatio);
}

fv3_float_t FV3_(limitmodel)::getAttack()
{
  return Attack;
}

void FV3_(limitmodel)::setAttack(fv3_float_t msec)
{
  Attack = msec;
  limitL.setAttack(FV3_(utils)::ms2sample(Attack,currentfs));
  limitR.setAttack(FV3_(utils)::ms2sample(Attack,currentfs));
}

fv3_float_t FV3_(limitmodel)::getRelease()
{
  return Release;
}

void FV3_(limitmodel)::setRelease(fv3_float_t msec)
{
  Release = msec;
  limitL.setRelease(FV3_(utils)::ms2sample(Release,currentfs));
  limitR.setRelease(FV3_(utils)::ms2sample(Release,currentfs));
}

fv3_float_t FV3_(limitmodel)::getThreshold()
{
  return Threshold;
}

void FV3_(limitmodel)::setThreshold(fv3_float_t dB)
{
  Threshold = dB;
  fv3_float_t threshold_r = FV3_(utils)::dB2R(Threshold);
  limitL.setThreshold(threshold_r);
  limitR.setThreshold(threshold_r);
  mute();
}

fv3_float_t FV3_(limitmodel)::getCeiling()
{
  return Ceiling;
}

void FV3_(limitmodel)::setCeiling(fv3_float_t dB)
{
  Ceiling = dB;
  ceiling_r = FV3_(utils)::dB2R(Ceiling);
  ceiling_m = -1. * ceiling_r;
  limitL.setCeiling(ceiling_r);
  limitR.setCeiling(ceiling_r);
}

void FV3_(limitmodel)::setStereoLink(bool value)
{
  stereoLink = value;
}

long FV3_(limitmodel)::getLatency()
{
  return 0;
}

void FV3_(limitmodel)::mute()
{
  currentGain = 1;
  limitL.mute();
  limitR.mute();
  lookaL.mute();
  lookaR.mute();
}

fv3_float_t FV3_(limitmodel)::getCGain(){ return currentGain; }
fv3_float_t FV3_(limitmodel)::getDGain(){ return currentGain2; }

//! main process
/*!
  output is valid even inputL/R == outputL/R.
*/
void FV3_(limitmodel)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  float gainL = 1, gainR = 1;
  for(long i = 0;i < numsamples;i ++)
    {
      gainL = limitL.process(inputL[i]);
      gainR = limitR.process(inputR[i]);
      if(stereoLink)
	{
	  if(gainL > gainR) gainL = gainR;
	  else gainR = gainL;
	}
      if(Lookahead > 0)
	{
	  outputL[i] = lookaL.process(inputL[i])*gainL;
	  outputR[i] = lookaR.process(inputR[i])*gainR;
	}
      else
	{
	  outputL[i] = inputL[i]*gainL;
	  outputR[i] = inputR[i]*gainR;
	}
      if(outputL[i] > ceiling_r) outputL[i] = ceiling_r;
      if(outputL[i] < ceiling_m) outputL[i] = ceiling_m;
      if(outputR[i] > ceiling_r) outputR[i] = ceiling_r;
      if(outputR[i] < ceiling_m) outputR[i] = ceiling_m;
    }
  currentGain = gainL, currentGain2 = gainR;
}

fv3_float_t FV3_(limitmodel)::getEnv()
{
  return limitL.getEnv();
}

void FV3_(limitmodel)::printconfig()
{
  std::fprintf(stderr, "*** limitmodel config ***\n");
  std::fprintf(stderr, "Fs=%f[Hz]\n",currentfs);
  std::fprintf(stderr, "Attack %1.2fms Release %1.2fms Threshold %1.2fdB Ceiling %1.2fdB",
	       (double)Attack, (double)Release, (double)Threshold, (double)Ceiling);
  std::fprintf(stderr, "Attack %1.2f Release %1.2f Threshold %1.2f",
	       (double)limitL.getAttack(), (double)limitL.getRelease(), (double)limitL.getThreshold());
}

#include "freeverb/fv3_ns_end.h"
