/**
 *  FIR 3 band splitter
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

#include "freeverb/fir3bandsplit.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(fir3bandsplit)::FV3_(fir3bandsplit)()
		    throw(std::bad_alloc)
{
  currentfs = 48000;
  fragmentSize = FV3_3BS_IR3_DFragmentSize;
  factor = FV3_3BS_IR3_DefaultFactor;
  WINDOW = FV3_W_BLACKMAN;
  fcLow = 145;
  fcHigh = 2500;
  trBand = 500;
  filterLength = 0;
  lpfF = bpfF = hpfF = NULL;
  lpfM = hpfM = NULL;
  allocIR(FV3_3BSPLIT_IR_IR3);
  update();
  mute();
}

FV3_(fir3bandsplit)::FV3_(~fir3bandsplit)()
{
  freeFilter();
  freeIR();
}

void FV3_(fir3bandsplit)::splitR(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL1, fv3_float_t *outputR1,
				 fv3_float_t *outputL2, fv3_float_t *outputR2, fv3_float_t *outputL3, fv3_float_t *outputR3,
				 long numsamples)
{
  if(lpfM == NULL||hpfM == NULL||numsamples <= 0) return;
  unsigned o = FV3_IR_DEFAULT|FV3_IR_MUTE_DRY|FV3_IR_SKIP_FILTER;
  lpfM->processreplace(inputL, inputR, outputL1, outputR1, numsamples, o);
  hpfM->processreplace(inputL, inputR, outputL3, outputR3, numsamples, o);
  // build the rest band
  for(long s = 0;s < numsamples;s ++)
    {
      outputL2[s] = groupDelayL.process(inputL[s]);
      outputL2[s] -= (outputL1[s] + outputL3[s]);
      outputR2[s] = groupDelayR.process(inputR[s]);
      outputR2[s] -= (outputR1[s] + outputR3[s]);
    }
}

void FV3_(fir3bandsplit)::mergeR(fv3_float_t *inputL1, fv3_float_t *inputR1, fv3_float_t *inputL2, fv3_float_t *inputR2,
				 fv3_float_t *inputL3, fv3_float_t *inputR3, fv3_float_t *outputL, fv3_float_t *outputR,
				 fv3_float_t f1, fv3_float_t f2, fv3_float_t f3, long numsamples)
{
  for(long i = 0;i < numsamples;i ++)
    {
      outputL[i] = inputL1[i]*f1 + inputL2[i]*f2 + inputL3[i]*f3;
      outputR[i] = inputR1[i]*f1 + inputR2[i]*f2 + inputR3[i]*f3;
    }
}

void FV3_(fir3bandsplit)::mute()
{
  if(lpfM != NULL) lpfM->mute();
  if(hpfM != NULL) hpfM->mute();
  groupDelayL.mute();
  groupDelayR.mute();
}

void FV3_(fir3bandsplit)::allocFilter(long length)
			 throw(std::bad_alloc)
{
  freeFilter();
  filterLength = length;
  try
    {
      lpfF = new fv3_float_t[length];
      bpfF = new fv3_float_t[length];
      hpfF = new fv3_float_t[length];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "fir3bandsplit::allocFilter(%ld) bad_alloc\n", length);
      freeFilter();
      throw;
    }
}

void FV3_(fir3bandsplit)::freeFilter()
{
  if(lpfM != NULL) lpfM->unloadImpulse();
  if(hpfM != NULL) hpfM->unloadImpulse();
  if(filterLength > 0)
    {
      delete[] lpfF;
      delete[] bpfF;
      delete[] hpfF;
      lpfF = bpfF = hpfF = NULL;
    }
}

void FV3_(fir3bandsplit)::setSampleRate(long fs)
			 throw(std::bad_alloc)
{
  currentfs = fs;
  update();
}

long FV3_(fir3bandsplit)::getSampleRate()
{
  return currentfs;
}

void FV3_(fir3bandsplit)::setWindow(long w)
			 throw(std::bad_alloc)
{
  WINDOW = w;
  update();
}

long FV3_(fir3bandsplit)::getWindow()
{
  return WINDOW;
}

void FV3_(fir3bandsplit)::setLowFreqDivider(fv3_float_t fc)
			 throw(std::bad_alloc)
{
  fcLow = fc;
  update();
}

fv3_float_t FV3_(fir3bandsplit)::getLowFreqDivider()
{
  return fcLow;
}

void FV3_(fir3bandsplit)::setHighFreqDivider(fv3_float_t fc)
			 throw(std::bad_alloc)
{
  fcHigh = fc;
  update();
}

fv3_float_t FV3_(fir3bandsplit)::getHighFreqDivider()
{
  return fcHigh;
}

void FV3_(fir3bandsplit)::setTransitionBand(fv3_float_t fc)
			 throw(std::bad_alloc)
{
  trBand = fc;
  update();
}

fv3_float_t FV3_(fir3bandsplit)::getTransitionBand()
{
  return trBand;
}

void FV3_(fir3bandsplit)::setParameter(fv3_float_t p)
			 throw(std::bad_alloc)
{
  param = p;
  update();
}

fv3_float_t FV3_(fir3bandsplit)::getParameter()
{
  return param;
}

void FV3_(fir3bandsplit)::setFragmentSize(long size)
			 throw(std::bad_alloc)
{
  if(size > 0)
    {
      fragmentSize = size;
      update();
    }
}

long FV3_(fir3bandsplit)::getFragmentSize()
{
  return fragmentSize;
}

void FV3_(fir3bandsplit)::setFactor(long val)
			 throw(std::bad_alloc)
{
  if(val > 0)
    {
      factor = val;
      update();
    }
}

long FV3_(fir3bandsplit)::getFactor()
{
  return factor;
}


void FV3_(fir3bandsplit)::update()
			 throw(std::bad_alloc)
{
  fv3_float_t fc1, fc2, fcband;
  long N;
  if(fcLow < fcHigh)
    {
      fc1 = fcLow/(fv3_float_t)currentfs;
      fc2 = fcHigh/(fv3_float_t)currentfs;
    }
  else
    {
      fc2 = fcLow/(fv3_float_t)currentfs;
      fc1 = fcHigh/(fv3_float_t)currentfs;
    }
  fcband = trBand/(fv3_float_t)currentfs;
  N = FV3_(firfilter)::predictFilterLength(WINDOW, fcband);
  // N must be an odd number.
  if(N % 2 == 0) N ++;
  allocFilter(N);
  FV3_(firfilter)::lpf(lpfF, N, WINDOW, fc1, param);
  FV3_(firfilter)::hpf(hpfF, N, WINDOW, fc2, param);
  if(lpfM != NULL) lpfM->loadImpulse(lpfF, lpfF, N);
  if(hpfM != NULL) hpfM->loadImpulse(hpfF, hpfF, N);
  groupDelayL.setsize((N-1)/2+lpfM->getLatency());
  groupDelayR.setsize((N-1)/2+lpfM->getLatency());
  mute();
}

void FV3_(fir3bandsplit)::setIR(unsigned o)
			 throw(std::bad_alloc)
{
  allocIR(o);
  update();
}

void FV3_(fir3bandsplit)::allocIR(unsigned o)
			 throw(std::bad_alloc)
{
  freeIR();
  try
    {
      switch(o)
	{
	case FV3_3BSPLIT_IR_IR2:
	  lpfM = new FV3_(irmodel2);
	  hpfM = new FV3_(irmodel2);
	  ((FV3_(irmodel2)*)lpfM)->setFragmentSize(fragmentSize*factor);
	  ((FV3_(irmodel2)*)hpfM)->setFragmentSize(fragmentSize*factor);
	  break;
	case FV3_3BSPLIT_IR_IR3:
	default:
	  lpfM = new FV3_(irmodel3);
	  hpfM = new FV3_(irmodel3);
	  ((FV3_(irmodel3)*)lpfM)->setFragmentSize(fragmentSize,factor);
	  ((FV3_(irmodel3)*)hpfM)->setFragmentSize(fragmentSize,factor);
	  break;
	}
      lpfM->setwet(0);
      hpfM->setwet(0);
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "fir3bandsplit::allocIR() bad_alloc\n");
      throw;
    }
}

void FV3_(fir3bandsplit)::freeIR()
{
  if(lpfM != NULL)
    {
      delete lpfM;
      delete hpfM;
      lpfM = hpfM = NULL;
    }
}

long FV3_(fir3bandsplit)::getLatency()
{
  return (filterLength-1)/2+lpfM->getLatency();
}

long FV3_(fir3bandsplit)::getGroupDelay()
{
  return (filterLength-1)/2;
}

long FV3_(fir3bandsplit)::getFilterLength()
{
  return filterLength;
}

#include "freeverb/fv3_ns_end.h"
