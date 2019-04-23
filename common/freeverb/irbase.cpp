/**
 *  Impulse Response Processor model abstract class
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

#include "freeverb/irbase.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// irbasem

FV3_(irbasem)::FV3_(irbasem)()
{
  impulseSize = latency = 0;
  setFFTFlags(FFTW_ESTIMATE);
  setSIMD(FV3_X86SIMD_FLAG_NULL,FV3_X86SIMD_FLAG_NULL);
}

FV3_(irbasem)::FV3_(~irbasem)()
{
  unloadImpulse();
}

void FV3_(irbasem)::unloadImpulse()
{
  impulseSize = 0;
}

unsigned FV3_(irbasem)::setFFTFlags(unsigned flags)
{
  return (fftflags = flags);
}

unsigned FV3_(irbasem)::getFFTFlags()
{
  return fftflags;
}

void FV3_(irbasem)::setSIMD(uint32_t flag1, uint32_t flag2)
{
  simdFlag1 = flag1;
  simdFlag2 = flag2;
}

uint32_t FV3_(irbasem)::getSIMD(uint32_t select)
{
  if(select == 0) return simdFlag1;
  if(select == 1) return simdFlag2;
  return 0;
}

long FV3_(irbasem)::getImpulseSize()
{
  return impulseSize;
}

long FV3_(irbasem)::getLatency()
{
  return latency;
}

void FV3_(irbasem)::resume(){;}
void FV3_(irbasem)::suspend(){;}

// irbase

FV3_(irbase)::FV3_(irbase)()
{
  setwet(0); setdry(0);
  setwidth(1); setLRBalance(0);
  setLPF(0); setHPF(0);
  impulseSize = 0;
  latency = 0;
  setInitialDelay(0);
  processoptions = FV3_IR_DEFAULT;
  irmL = irmR = NULL;
}

FV3_(irbase)::FV3_(~irbase)()
{
  unloadImpulse();
}

void FV3_(irbase)::unloadImpulse()
{
  impulseSize = 0;
}

long FV3_(irbase)::getImpulseSize()
{
  return impulseSize;
}

long FV3_(irbase)::getLatency()
{
  if(impulseSize == 0) return 0;
  return latency;
}

void FV3_(irbase)::setInitialDelay(long numsamples)
  throw(std::bad_alloc)
{
  initialDelay = numsamples;
  delayDL.free(), delayWL.free(), delayDR.free(), delayWR.free(); // delay class does not accept size=0
  if(initialDelay >= 0)
    {
      delayDL.setsize(latency), delayWL.setsize(initialDelay);
      delayDR.setsize(latency), delayWR.setsize(initialDelay);
    }
  else // delay dry signal if initialDelay < 0
    {
      delayDL.setsize(latency-initialDelay), delayWL.setsize(0);
      delayDR.setsize(latency-initialDelay), delayWR.setsize(0);
    }
  FV3_(irbase)::mute();
}

long FV3_(irbase)::getInitialDelay()
{
  return initialDelay;
}

unsigned FV3_(irbase)::setFFTFlags(unsigned flags)
{
  if(irmL != NULL) irmL->setFFTFlags(flags);
  if(irmR != NULL) irmR->setFFTFlags(flags);
  return (fftflags = flags);
}

unsigned FV3_(irbase)::getFFTFlags()
{
  return fftflags;
}

void FV3_(irbase)::setSIMD(uint32_t flag1, uint32_t flag2)
{
  if(irmL != NULL) irmL->setSIMD(flag1, flag2);
  if(irmR != NULL) irmR->setSIMD(flag1, flag2);
  simdFlag1 = flag1;
  simdFlag2 = flag2;
}

uint32_t FV3_(irbase)::getSIMD(uint32_t select)
{
  if(select == 0) return simdFlag1;
  if(select == 1) return simdFlag2;
  return 0;
}

void FV3_(irbase)::update()
{
  wet1 = wet*(width/2 + 0.5f);
  wet2 = wet*((1-width)/2);
  wet1L = lrbalance < 0 ? wet1 : wet1*(1-lrbalance);
  wet2L = lrbalance < 0 ? wet2 : wet2*(1-lrbalance);
  wet1R = lrbalance > 0 ? wet1 : wet1*(1+lrbalance);
  wet2R = lrbalance > 0 ? wet2 : wet2*(1+lrbalance);
}

void FV3_(irbase)::setwet(fv3_float_t db)
{
  wet = FV3_(utils)::dB2R(wetdB = db);
  update();
}

fv3_float_t FV3_(irbase)::getwet()
{
  return wetdB;
}

void FV3_(irbase)::setwetr(fv3_float_t value)
{
  if(value == 0)
    {
      wet = value;
      wetdB = FP_NAN;
    }
  else
    {
      wetdB = FV3_(utils)::R2dB(wet = value);
    }
  update();
}

fv3_float_t FV3_(irbase)::getwetr()
{
  return wet;
}

void FV3_(irbase)::setdry(fv3_float_t db)
{
  dry = FV3_(utils)::dB2R(drydB = db);
}

fv3_float_t FV3_(irbase)::getdry()
{
  return drydB;
}

void FV3_(irbase)::setdryr(fv3_float_t value)
{
  if(value == 0)
    {
      dry = value;
      drydB = FP_NAN;
    }
  else
    {
      drydB = FV3_(utils)::R2dB(dry = value);
    }
}

fv3_float_t FV3_(irbase)::getdryr()
{
  return dry;
}

void FV3_(irbase)::setwidth(fv3_float_t value)
{
  width = value;
  update();
}

fv3_float_t FV3_(irbase)::getwidth()
{
  return width;
}

void FV3_(irbase)::setLPF(fv3_float_t value)
{
  filter.setLPF(value);
}

fv3_float_t FV3_(irbase)::getLPF()
{
  return filter.getLPF();
}

void FV3_(irbase)::setHPF(fv3_float_t value)
{
  filter.setHPF(value);
}

fv3_float_t FV3_(irbase)::getHPF()
{
  return filter.getHPF();
}

void FV3_(irbase)::setLRBalance(fv3_float_t value)
{
  // Balance |-1 0 1
  // L       | 1 1~0
  // R       | 0~1 1
  if(value < -1) value = -1;
  if(value > 1) value = 1;
  lrbalance = value;
  update();
}

fv3_float_t FV3_(irbase)::getLRBalance()
{
  return lrbalance;
}

void FV3_(irbase)::processreplace(const fv3_float_t *inputL, const fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples, unsigned options)
{
  setprocessoptions(options);
  processreplace(inputL, inputR, outputL, outputR, numsamples);
}

void FV3_(irbase)::setprocessoptions(unsigned options)
{
  processoptions = options;
}

void FV3_(irbase)::processdrywetout(const fv3_float_t *dL, const fv3_float_t *dR, fv3_float_t *wL, fv3_float_t *wR, fv3_float_t *oL, fv3_float_t *oR, long numsamples)
{
  if((processoptions & FV3_IR_SKIP_FILTER) == 0)
    {
      for(long i = 0;i < numsamples;i ++){ wL[i] = filter.processL(wL[i]), wR[i] = filter.processR(wR[i]); } 
    }
  for(long i = 0;i < numsamples;i ++)
    {
      wL[i] = delayWL.process(wL[i]), wR[i] = delayWR.process(wR[i]);
    }
  if((processoptions & FV3_IR_SWAP_LR) != 0)
    {
      fv3_float_t * sT = oL; oL = oR; oR = sT;
    }
  if((processoptions & FV3_IR_SKIP_INIT) == 0)
    {
      FV3_(utils)::mute(oL, numsamples), FV3_(utils)::mute(oR, numsamples);
    }
  if((processoptions & FV3_IR_MUTE_WET) == 0)
    {
      for(long i = 0;i < numsamples;i ++) oL[i] += wL[i]*wet1L + wR[i]*wet2L;
      for(long i = 0;i < numsamples;i ++) oR[i] += wR[i]*wet1R + wL[i]*wet2R;
    }
  if((processoptions & FV3_IR_MUTE_DRY) == 0)
    {
      for(long i = 0;i < numsamples;i ++) oL[i] += delayDL.process(dL[i])*dry;
      for(long i = 0;i < numsamples;i ++) oR[i] += delayDR.process(dR[i])*dry;
    }
}

unsigned FV3_(irbase)::getprocessoptions()
{
  return processoptions;
}

void FV3_(irbase)::mute()
{
  delayDL.mute(), delayDR.mute();
  delayWL.mute(), delayWR.mute();
  filter.mute();
}

void FV3_(irbase)::resume()
{
  irmL->resume();
  irmR->resume();
}

void FV3_(irbase)::suspend()
{
  irmL->suspend();
  irmR->suspend();
}

#include "freeverb/fv3_ns_end.h"
