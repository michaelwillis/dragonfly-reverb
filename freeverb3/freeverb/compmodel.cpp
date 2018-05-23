/**
 *  Compressor Model Implementation
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

#include "freeverb/compmodel.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(compmodel)::FV3_(compmodel)()
{
  currentfs = 48000;
  setRMS(0); setLookahead(0); setAttack(0); setRelease(0);
  setThreshold(-10); setSoftKnee(10); setRatio(2);
  mute();
}

fv3_float_t FV3_(compmodel)::getSampleRate()
{
  return currentfs;
}

void FV3_(compmodel)::setSampleRate(fv3_float_t fs)
{
  if(fs <= 0) return;
  currentfs = fs;
  setRMS(getRMS());
  setLookahead(getLookahead());
  setAttack(getAttack());
  setRelease(getRelease());
  mute();
}

fv3_float_t FV3_(compmodel)::getRMS()
{
  return RMS;
}

void FV3_(compmodel)::setRMS(fv3_float_t msec)
{
  RMS = msec;
  compL.setRMS(FV3_(utils)::ms2sample(RMS,currentfs));
  compR.setRMS(FV3_(utils)::ms2sample(RMS,currentfs));
}

fv3_float_t FV3_(compmodel)::getLookahead()
{
  return Lookahead;
}

void FV3_(compmodel)::setLookahead(fv3_float_t msec)
{
  Lookahead = msec;
  lookaL.setsize(FV3_(utils)::ms2sample(Lookahead,currentfs));
  lookaR.setsize(FV3_(utils)::ms2sample(Lookahead,currentfs));
}

fv3_float_t FV3_(compmodel)::getAttack()
{
  return Attack;
}

void FV3_(compmodel)::setAttack(fv3_float_t msec)
{
  Attack = msec;
  compL.setAttack(FV3_(utils)::ms2sample(Attack,currentfs));
  compR.setAttack(FV3_(utils)::ms2sample(Attack,currentfs));
}

fv3_float_t FV3_(compmodel)::getRelease()
{
  return Release;
}

void FV3_(compmodel)::setRelease(fv3_float_t msec)
{
  Release = msec;
  compL.setRelease(FV3_(utils)::ms2sample(Release,currentfs));
  compR.setRelease(FV3_(utils)::ms2sample(Release,currentfs));
}

fv3_float_t FV3_(compmodel)::getThreshold()
{
  return Threshold;
}

void FV3_(compmodel)::setThreshold(fv3_float_t dB)
{
  Threshold = dB;
  compL.setThreshold(FV3_(utils)::dB2R(Threshold));
  compR.setThreshold(FV3_(utils)::dB2R(Threshold));
}

fv3_float_t FV3_(compmodel)::getSoftKnee()
{
  return SoftKnee;
}

void FV3_(compmodel)::setSoftKnee(fv3_float_t dB)
{
  SoftKnee = dB;
  compL.setSoftKnee(SoftKnee);
  compR.setSoftKnee(SoftKnee);
}

fv3_float_t FV3_(compmodel)::getRatio()
{
  return Ratio;
}

void FV3_(compmodel)::setRatio(fv3_float_t value)
{
  Ratio = value;
  compL.setRatio(Ratio);
  compR.setRatio(Ratio);
}

long FV3_(compmodel)::getLatency()
{
  return 0;
}

void FV3_(compmodel)::mute()
{
  currentGain = 1;
  compL.mute();
  compR.mute();
  lookaL.mute();
  lookaR.mute();
}

fv3_float_t FV3_(compmodel)::getCGain()
{
  return currentGain;
}

//! main process
/*!
  output is valid even inputL/R == outputL/R.
 */
void FV3_(compmodel)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  float gainL, gainR;
  for(long i = 0;i < numsamples;i ++)
    {
      gainL = compL.process(inputL[i]);
      gainR = compR.process(inputR[i]);
      if(gainL > gainR) currentGain = gainR;
      else currentGain = gainL;
      outputL[i] = lookaL.process(inputL[i])*currentGain;
      outputR[i] = lookaR.process(inputR[i])*currentGain;
    }
}

fv3_float_t FV3_(compmodel)::getEnv()
{
  return compL.getEnv();
}

void FV3_(compmodel)::printconfig()
{
  std::fprintf(stderr, "*** compmodel config ***\n");
  std::fprintf(stderr, "Fs=%f[Hz]\n",currentfs);
  std::fprintf(stderr, "Attack %1.2fms Release %1.2fms Threshold %1.2fdB Ratio %1.2f:1 ",
	       (double)Attack, (double)Release, (double)Threshold, (double)Ratio);
  std::fprintf(stderr, "SoftKnee %1.2f\n", (double)SoftKnee);
  std::fprintf(stderr, "Attack %1.2f Release %1.2f Threshold %1.2f Ratio %1.2f:1 ",
	       (double)compL.getAttack(), (double)compL.getRelease(), (double)compL.getThreshold(), (double)compL.getRatio());
  std::fprintf(stderr, "SoftKnee %1.2f\n", (double)compL.getSoftKnee());
}

#include "freeverb/fv3_ns_end.h"
