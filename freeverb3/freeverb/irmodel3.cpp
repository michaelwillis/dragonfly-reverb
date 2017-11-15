/**
 *  Impulse Response Processor model implementation
 *  Low Latency Version
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

#include "freeverb/irmodel3.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(irmodel3)::FV3_(irmodel3)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel3::irmodel3()\n");
#endif
  Scursor = Lcursor = Lstep = 0;
  setFragmentSize(FV3_IR3_DFragmentSize, FV3_IR3_DefaultFactor);
}

FV3_(irmodel3)::FV3_(~irmodel3)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel3::~irmodel3()\n");
#endif
  FV3_(irmodel3)::unloadImpulse();
}

long FV3_(irmodel3)::getSFragmentSize()
{
  return sFragmentSize;
}

long FV3_(irmodel3)::getLFragmentSize()
{
  return lFragmentSize;
}

void FV3_(irmodel3)::mute()
{
  if(impulseSize == 0) return;
  ZLdelayL.mute();
  ZLdelayR.mute();
  ZLdelayWL.mute();
  ZLdelayWR.mute();
  sBlockDelayL.mute();
  sBlockDelayR.mute();
  lBlockDelayL.mute();
  lBlockDelayR.mute();
  filter.mute();
  Scursor = Lcursor = Lstep = 0;
  sReverseSlot.mute();
  lReverseSlot.mute();
  sIFFTSlot.mute();
  lIFFTSlot.mute();
  sSwapSlot.mute();
  lSwapSlot.mute();
  restSlot.mute();
  fifoSlot.mute();
  lFrameSlot.mute();
  sOnlySlot.mute();
}

void FV3_(irmodel3)::setFragmentSize(long size, long factor)
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel3::setFragmentSize(%ld,%ld)\n", size, factor);
#endif
  if(size <= 0||factor <= 0||size < FV3_IR_Min_FragmentSize||
     size != FV3_(utils)::checkPow2(size)||factor != FV3_(utils)::checkPow2(factor))
    {
      std::fprintf(stderr, "irmodel3::setFragmentSize(): invalid fragment size/factor (%ld/%ld)\n", size, factor);
      return;
    }
  if(sFragmentSize != size||lFragmentSize != size*factor)
    {
#ifdef DEBUG
      std::fprintf(stderr, "irmodel3::setFragmentSize(): %ld -> %ld %ld -> %ld\n",
		   sFragmentSize, size, lFragmentSize, size*factor);
#endif
      FV3_(irmodel3)::unloadImpulse();
      sFragmentSize = size;
      lFragmentSize = size*factor;
    }
}

void FV3_(irmodel3)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
		    throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel3::loadImpulse(%ld)\n", size);
#endif
  if(size <= 0) return;
  
  FV3_(irmodel3)::unloadImpulse();
  
  impulseSize = size;  
  long sFragmentNum = 0, lFragmentNum = 0;
  long sFragmentRest = 0, lFragmentRest = 0;
  if(size <= lFragmentSize)
    {
      sFragmentNum = size / sFragmentSize;
      sFragmentRest = size % sFragmentSize;
      lFragmentNum = 0;
      lFragmentRest = 0;
    }
  else
    {
      sFragmentNum = lFragmentSize / sFragmentSize;
      sFragmentRest = 0;
      lFragmentNum = size / lFragmentSize - 1;
      lFragmentRest = size % lFragmentSize;
    }
  
#ifdef DEBUG  
  std::fprintf(stderr, "irmodel3::loadImpulse(): {%ldx%ld+%ld+%ldx%ld+%ld}\n",
	       sFragmentSize, sFragmentNum, sFragmentRest,
	       lFragmentSize, lFragmentNum, lFragmentRest);
#endif

  try
    {
      allocSwap(sFragmentSize, lFragmentSize);
      sFragmentsFFT.allocFFT(sFragmentSize, fftflags);
      lFragmentsFFT.allocFFT(lFragmentSize, fftflags);
      sImpulseFFTBlock.alloc(sFragmentSize*2*(sFragmentNum+1), 2);
      lImpulseFFTBlock.alloc(lFragmentSize*2*(lFragmentNum+1), 2);
      allocFragments(&sFragments, inputL, inputR,
		     sFragmentSize, sFragmentNum, sFragmentRest, fftflags,
		     sImpulseFFTBlock.L, sImpulseFFTBlock.R);
      if(size > lFragmentSize)
	{
	  allocFragments(&lFragments, inputL+lFragmentSize, inputR+lFragmentSize,
			 lFragmentSize, lFragmentNum, lFragmentRest, fftflags,
			 lImpulseFFTBlock.L, lImpulseFFTBlock.R);
	}
      sBlockDelayL.setBlock(sFragmentSize*2, (long)sFragments.size());
      sBlockDelayR.setBlock(sFragmentSize*2, (long)sFragments.size());
      lBlockDelayL.setBlock(lFragmentSize*2, (long)lFragments.size());
      lBlockDelayR.setBlock(lFragmentSize*2, (long)lFragments.size());
    }
  catch(std::bad_alloc)
    {
      FV3_(irmodel3)::unloadImpulse();
      throw;
    }
  FV3_(irmodel3)::mute();
}

void FV3_(irmodel3)::unloadImpulse()
{
  if(impulseSize == 0) return;
  impulseSize = 0;
  freeFragments();
  freeSwap();
  sFragmentsFFT.freeFFT();
  lFragmentsFFT.freeFFT();
  sImpulseFFTBlock.free();
  lImpulseFFTBlock.free();
}

void FV3_(irmodel3)::freeFragments()
{
  freeFragments(&sFragments);
  freeFragments(&lFragments);
}

void FV3_(irmodel3)::freeFragments(std::vector<FV3_(frag)*> *v)
{
  for(std::vector<FV3_(frag)*>::iterator i = v->begin();
      i != v->end();i ++) delete *i;
  v->clear();
}

void FV3_(irmodel3)::allocFragments(std::vector<FV3_(frag)*> *to, const fv3_float_t *inputL, const fv3_float_t *inputR,
				    long fragSize, long num, long rest, unsigned flags,
				    fv3_float_t * preAllocatedL, fv3_float_t * preAllocatedR)
		    throw(std::bad_alloc)
{
  try
    {
      for(long i = 0;i < num;i ++)
	{
	  FV3_(frag) * f = new FV3_(frag);
	  to->push_back(f);
	  f->loadImpulse(inputL+fragSize*i, inputR+fragSize*i, fragSize, fragSize, flags,
			 preAllocatedL+fragSize*2*i, preAllocatedR+fragSize*2*i);
	}
      if(rest != 0)
	{
	  FV3_(frag) * f = new FV3_(frag);
	  to->push_back(f);
	  f->loadImpulse(inputL+fragSize*num, inputR+fragSize*num, fragSize, rest, flags,
			 preAllocatedL+fragSize*2*num, preAllocatedR+fragSize*2*num);
	}
    }
  catch(std::bad_alloc)
    {
      std::fprintf(stderr, "irmodel3::allocFragments(%ld) bad_alloc\n", fragSize);
      freeFragments(to);
      throw;
    }
}

void FV3_(irmodel3)::allocSwap(long ssize, long lsize)
		    throw(std::bad_alloc)
{
  freeSwap();
  fifoSlot.alloc(ssize, 2);
  sReverseSlot.alloc(2*ssize, 2);
  lReverseSlot.alloc(2*lsize, 2);
  restSlot.alloc(ssize, 2);
  sOnlySlot.alloc(ssize, 2);
  lFrameSlot.alloc(lsize, 2);
  // The following slots must be SIMD slots.
  sIFFTSlot.alloc(2*ssize, 2);
  lIFFTSlot.alloc(2*lsize, 2);
  sSwapSlot.alloc(2*ssize, 2);
  lSwapSlot.alloc(2*lsize, 2);
}

void FV3_(irmodel3)::freeSwap()
{
  fifoSlot.free();
  sReverseSlot.free();
  lReverseSlot.free();
  restSlot.free();
  lFrameSlot.free();
  sOnlySlot.free();
  sIFFTSlot.free();
  lIFFTSlot.free();
  sSwapSlot.free();
  lSwapSlot.free();
}

void FV3_(irmodel3)::setInitialDelay(long numsamples)
		    throw(std::bad_alloc)
{
  try
    {
      if(initialDelay >= 0)
	{
	  ZLdelayL.setsize(initialDelay);
	  ZLdelayR.setsize(initialDelay);
	  ZLdelayWL.setsize(0);
	  ZLdelayWR.setsize(0);
	}
      else
	{
	  ZLdelayWL.setsize(-initialDelay);
	  ZLdelayWR.setsize(-initialDelay);
	  ZLdelayL.setsize(0);
	  ZLdelayR.setsize(0);
	}
    }
  catch(std::bad_alloc)
    {
      std::fprintf(stderr, "irmodel3::setInitialDelay(%ld) bad_alloc\n", initialDelay);
      throw;
    }
  initialDelay = numsamples;
  ZLdelayL.mute();
  ZLdelayR.mute();
  ZLdelayWL.mute();
  ZLdelayWR.mute();
}

//! main process
/*!
  output is valid even inputL/R == outputL/R.
 */
void FV3_(irmodel3)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR,
				    fv3_float_t *outputL, fv3_float_t *outputR,
				    long numsamples, unsigned options)
{
  if(numsamples <= 0||impulseSize <= 0) return;

  long cursor = sFragmentSize - Scursor;
  if(numsamples > cursor)
    {
      processreplace(inputL, inputR, outputL, outputR, cursor, options);
      long sum = numsamples - cursor;
      long div = sum/sFragmentSize;
      for(long i = 0;i < div;i ++)
        {
          processreplace(inputL+cursor+i*sFragmentSize, inputR+cursor+i*sFragmentSize,
                         outputL+cursor+i*sFragmentSize, outputR+cursor+i*sFragmentSize,
                         sFragmentSize, options);
        }
      if(sum%sFragmentSize > 0)
        {
          processreplace(inputL+cursor+div*sFragmentSize, inputR+cursor+div*sFragmentSize,
                         outputL+cursor+div*sFragmentSize, outputR+cursor+div*sFragmentSize,
                         sum%sFragmentSize, options);
        }
      return;
    }

  // main process
  if(options & FV3_IR_SWAP_LR)
    {
      fv3_float_t * swapL = outputL;
      outputL = outputR;
      outputR = swapL;
    }
  
  processZL(inputL, inputR, fifoSlot.L, fifoSlot.R, numsamples, options);
  
  // process output loop
  if((options & FV3_IR_SKIP_FILTER) == 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  fifoSlot.L[i] = filter.processL(fifoSlot.L[i]);
	  fifoSlot.R[i] = filter.processR(fifoSlot.R[i]);
	}
    }
  for(long i = 0;i < numsamples;i ++)
    {
      fifoSlot.L[i] = ZLdelayL.process(fifoSlot.L[i]);
      fifoSlot.R[i] = ZLdelayR.process(fifoSlot.R[i]);
    }
  if((options & FV3_IR_SKIP_INIT) == 0)
    {
      FV3_(utils)::mute(outputL, numsamples);
      FV3_(utils)::mute(outputR, numsamples);
    }
  if((options & FV3_IR_MUTE_WET) == 0)
    {
      if(width == 1)
	{
	  for(long i = 0;i < numsamples;i ++)
	    {
	      outputL[i] += fifoSlot.L[i]*wet1L;
	      outputR[i] += fifoSlot.R[i]*wet1R;
	    }
	}
      else
	{
	  for(long i = 0;i < numsamples;i ++)
	    {
	      outputL[i] += fifoSlot.L[i]*wet1L + fifoSlot.R[i]*wet2L;
	      outputR[i] += fifoSlot.R[i]*wet1R + fifoSlot.L[i]*wet2R;
	    }
	}
    }
  if((options & FV3_IR_MUTE_DRY) == 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] += ZLdelayWL.process(inputL[i])*dry;
	  outputR[i] += ZLdelayWR.process(inputR[i])*dry;
	}
    }
  return;
}

void FV3_(irmodel3)::processZL(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples, unsigned options)
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel3::processZL(%ld)\n", numsamples);
#endif
  // numsamples <= sFragmentSize - Scursor
  if(Lcursor == 0&&lFragments.size() > 0)
    {
      lFrameSlot.mute();
      lReverseSlot.mute(lFragmentSize-1, lFragmentSize+1);
      lBlockDelayL.push(lIFFTSlot.L);
      lBlockDelayR.push(lIFFTSlot.R);
      lFragments[0]->MULT(lBlockDelayL.get(0), lBlockDelayR.get(0), lSwapSlot.L, lSwapSlot.R);
      lFragmentsFFT.HC2R(lSwapSlot.L, lSwapSlot.R, lReverseSlot.L, lReverseSlot.R);
      lSwapSlot.mute();
      // The calculation of the vector was moved to [LVECTOR] to
      // reduce CPU load peak.
    }
  
  if(Scursor == 0)
    {
      sFramePointerL = lFrameSlot.L+Lcursor;
      sFramePointerR = lFrameSlot.R+Lcursor;
      sSwapSlot.mute();
      sBlockDelayL.push(sIFFTSlot.L);
      sBlockDelayR.push(sIFFTSlot.R);
      for(long i = 1;i < (long)sFragments.size();i ++)
	{
	  sFragments[i]->MULT(sBlockDelayL.get(i-1), sBlockDelayR.get(i-1), sSwapSlot.L, sSwapSlot.R);
	}
    }
  sOnlySlot.mute();
  
  if(options & FV3_IR_MONO2STEREO)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  (lFrameSlot.L+Lcursor)[i] = (lFrameSlot.R+Lcursor)[i]
	    = (sOnlySlot.L+Scursor)[i] = (sOnlySlot.R+Scursor)[i]
	    = (inputL[i] + inputR[i])/2.0;
	}
    }
  else
    {
      std::memcpy(lFrameSlot.L+Lcursor, inputL, sizeof(fv3_float_t)*numsamples);
      std::memcpy(lFrameSlot.R+Lcursor, inputR, sizeof(fv3_float_t)*numsamples);
      std::memcpy(sOnlySlot.L+Scursor, inputL, sizeof(fv3_float_t)*numsamples);
      std::memcpy(sOnlySlot.R+Scursor, inputR, sizeof(fv3_float_t)*numsamples);
    }
  
  if(sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sOnlySlot.L, sOnlySlot.R, sIFFTSlot.L, sIFFTSlot.R);
      sFragments[0]->MULT(sIFFTSlot.L, sIFFTSlot.R, sSwapSlot.L, sSwapSlot.R);
      sReverseSlot.mute();
      sFragmentsFFT.HC2R(sSwapSlot.L, sSwapSlot.R, sReverseSlot.L, sReverseSlot.R);
    }
  
  if(lFragments.size() > 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i] + (lReverseSlot.L+Lcursor)[i];
	  outputR[i] = (sReverseSlot.R+Scursor)[i] + (restSlot.R+Scursor)[i] + (lReverseSlot.R+Lcursor)[i];
	}
    }
  else
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i];
	  outputR[i] = (sReverseSlot.R+Scursor)[i] + (restSlot.R+Scursor)[i];
	}
    }
  
  Scursor += numsamples;
  Lcursor += numsamples;
  
  // [LVECTOR]
  for(long i = Lstep;i < (((long)lFragments.size())-1)*Lcursor/lFragmentSize;i ++)
    {
      if(((long)lFragments.size()) > i+1)
	{
	  lFragments[i+1]->MULT(lBlockDelayL.get(i), lBlockDelayR.get(i), lSwapSlot.L, lSwapSlot.R);
	}
      Lstep ++;
    }

  if(Scursor == sFragmentSize&&sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sFramePointerL, sFramePointerR, sIFFTSlot.L, sIFFTSlot.R);
      std::memcpy(restSlot.L, sReverseSlot.L+sFragmentSize, sizeof(fv3_float_t)*(sFragmentSize-1));
      std::memcpy(restSlot.R, sReverseSlot.R+sFragmentSize, sizeof(fv3_float_t)*(sFragmentSize-1));
      Scursor = 0;
    }
  
  if(Lcursor == lFragmentSize)
    {
      if(lFragments.size() > 0)
	{
	  lFragmentsFFT.R2HC(lFrameSlot.L, lFrameSlot.R, lIFFTSlot.L, lIFFTSlot.R);
	  std::memcpy(lReverseSlot.L, lReverseSlot.L+lFragmentSize, sizeof(fv3_float_t)*(lFragmentSize-1));
	  std::memcpy(lReverseSlot.R, lReverseSlot.R+lFragmentSize, sizeof(fv3_float_t)*(lFragmentSize-1));
	}
      Lcursor = Lstep = 0;
    }
}

void FV3_(irmodel3)::printconfig()
{
  std::fprintf(stderr, "*** irmodel3 config ***\n");
  std::fprintf(stderr, "impulseSize = %ld\n", impulseSize);
  std::fprintf(stderr, "short Fragment Size = %ld\n", sFragmentSize);
  std::fprintf(stderr, "large Fragment Size = %ld\n", lFragmentSize);
  std::fprintf(stderr, "short Fragment vector Size = %ld\n", (long)sFragments.size());
  std::fprintf(stderr, "large Fragment vector Size = %ld\n", (long)lFragments.size());
}

long FV3_(irmodel3)::getSFragmentCount()
{
  return (long)sFragments.size();
}

long FV3_(irmodel3)::getLFragmentCount()
{
  return (long)lFragments.size();
}

#include "freeverb/fv3_ns_end.h"
