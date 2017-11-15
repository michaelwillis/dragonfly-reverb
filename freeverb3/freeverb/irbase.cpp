/**
 *  Impulse Response Processor model abstract class
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

#include "freeverb/irbase.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(irbase)::FV3_(irbase)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irbase::irbase()\n");
#endif
  setwet(0); setdry(0);
  setwidth(1); setLRBalance(0);
  setLPF(0); setHPF(0);
  impulseSize = 0;
  setFFTFlags(FFTW_ESTIMATE);
  setInitialDelay(0);
  processoptions = FV3_IR_DEFAULT;
}

FV3_(irbase)::FV3_(~irbase)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irbase::~irbase()\n");
#endif
}

long FV3_(irbase)::getSampleSize()
{
  return impulseSize;
}

long FV3_(irbase)::getLatency()
{
  return 0;
}

void FV3_(irbase)::setInitialDelay(long numsamples)
		  throw(std::bad_alloc)
{
  initialDelay = numsamples;
}

long FV3_(irbase)::getInitialDelay()
{
  return initialDelay;
}

unsigned FV3_(irbase)::setFFTFlags(unsigned flags)
{
  return (fftflags = flags);
}

unsigned FV3_(irbase)::getFFTFlags()
{
  return fftflags;
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

void FV3_(irbase)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  processreplace(inputL, inputR, outputL, outputR, numsamples, processoptions);
}

void FV3_(irbase)::setprocessoptions(unsigned options)
{
  processoptions = options;
}

unsigned FV3_(irbase)::getprocessoptions()
{
  return processoptions;
}

void FV3_(irbase)::resume(){;}
void FV3_(irbase)::suspend(){;}

#include "freeverb/fv3_ns_end.h"
