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

#include "freeverb/irmodel2.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(irmodel2)::FV3_(irmodel2)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::irmodel2()\n");
#endif
  fifoSize = 0;
  setFragmentSize(FV3_IR2_DFragmentSize);
}

FV3_(irmodel2)::FV3_(~irmodel2)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::~irmodel2()\n");
#endif
  FV3_(irmodel2)::unloadImpulse();
}

void FV3_(irmodel2)::mute()
{
  if(impulseSize == 0) return;
  fifoSize = fragmentSize;
  delayL.mute();
  delayR.mute();
  delayWL.mute();
  delayWR.mute();
  filter.mute();
  blkdelayL.mute();
  blkdelayR.mute();
  fifoSlot.mute();
  reverseSlot.mute();
  ifftSlot.mute();
  restSlot.mute();
  swapSlot.mute();
}

void FV3_(irmodel2)::setFragmentSize(long size)
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::setFragmentSize(%ld)\n", size);
#endif
  if(size <= 0||size < FV3_IR_Min_FragmentSize||
     size != FV3_(utils)::checkPow2(size))
    {
      std::fprintf(stderr, "irmodel2::setFragmentSize(): invalid fragment size (%ld)\n", size);      
      return;
    }
  if(fragmentSize != size)
    {
#ifdef DEBUG
      std::fprintf(stderr, "irmodel2::setFragmentSize(): %ld -> %ld\n", fragmentSize, size);
#endif
      unloadImpulse();
      fragmentSize = size;
    }
  return;
}

long FV3_(irmodel2)::getFragmentSize()
{
  return fragmentSize;
}

void FV3_(irmodel2)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
		    throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::loadImpulse(%ld)\n", size);
#endif
  if(size <= 0||fragmentSize <= 1) return;
  
  // optimize fragmentSize if fragSize >> impulsesize
  // if(FV3_(utils)::checkPow2(impulsesize)/2 < size)
  // fragmentSize = FV3_(utils)::checkPow2(size)/2;
  
  FV3_(irmodel2)::unloadImpulse();
  
  long fragment_num = size / fragmentSize;
  long fragment_rest = size % fragmentSize;

#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::loadImpulse(): {%ldx%ld+%ld}\n",
	       fragmentSize, fragment_num, fragment_rest);
#endif
  try
    {
      allocSwap(fragmentSize);
      fragmentsFFT.allocFFT(fragmentSize, fftflags);
      for(long i = 0;i < fragment_num;i ++)
	{
	  FV3_(frag) * f = new FV3_(frag);
	  fragments.push_back(f);
	  f->loadImpulse(inputL+fragmentSize*i, inputR+fragmentSize*i,
			 fragmentSize, fragmentSize, fftflags);
	}
      if(fragment_rest != 0)
	{
	  FV3_(frag) * f = new FV3_(frag);
	  fragments.push_back(f);
	  f->loadImpulse(inputL+fragmentSize*fragment_num,
			 inputR+fragmentSize*fragment_num,
			 fragmentSize, fragment_rest, fftflags);
	}
      blkdelayL.setBlock(fragmentSize*2, (long)fragments.size());
      blkdelayR.setBlock(fragmentSize*2, (long)fragments.size());
    }
  catch(std::bad_alloc)
    {
      std::fprintf(stderr, "irmodel2::loadImpulse(%ld) bad_alloc\n", size);
      FV3_(irmodel2)::unloadImpulse();
      throw;
    }
  impulseSize = size;
  FV3_(irmodel2)::mute();
  setInitialDelay(getInitialDelay());
}

void FV3_(irmodel2)::unloadImpulse()
{
  if(impulseSize == 0) return;
  impulseSize = 0;
  freeSwap();
  freeFragments();
  fragmentsFFT.freeFFT();
}

void FV3_(irmodel2)::freeFragments()
{
  for(std::vector<FV3_(frag)*>::iterator i = fragments.begin();
      i != fragments.end();i ++) delete *i;
  fragments.clear();
}

void FV3_(irmodel2)::allocSwap(long size)
		    throw(std::bad_alloc)
{
  freeSwap();
  fifoSlot.alloc(3*size, 2);
  reverseSlot.alloc(2*size, 2);
  restSlot.alloc(size, 2);
  ifftSlot.alloc(2*size, 2);
  swapSlot.alloc(2*size, 2);
}

void FV3_(irmodel2)::freeSwap()
{
  fifoSlot.free();
  reverseSlot.free();
  ifftSlot.free();
  swapSlot.free();
  restSlot.free();
}

void FV3_(irmodel2)::setInitialDelay(long numsamples)
		    throw(std::bad_alloc)
{
  initialDelay = numsamples;
  if(initialDelay >= 0)
    {
#ifdef DEBUG
      std::fprintf(stderr, "irmodel2::setInitialDelay(%ld) delayW(%ld)\n",
		   numsamples, initialDelay);
#endif
      delayL.setsize(fragmentSize);
      delayR.setsize(fragmentSize);
      delayWL.setsize(initialDelay);
      delayWR.setsize(initialDelay);
    }
  else if(fragmentSize - initialDelay >= 0)
    {
      long dryD = fragmentSize - initialDelay;
#ifdef DEBUG
      std::fprintf(stderr, "irmodel2::setInitialDelay(%ld) delayD(%ld)\n",
		   numsamples, dryD);
#endif
      delayL.setsize(dryD);
      delayR.setsize(dryD);
      delayWL.setsize(0);
      delayWR.setsize(0);
    }
  else
    {
      std::fprintf(stderr, "irmodel2::setInitialDelay(%ld) unknown samples\n",
		   numsamples);
      delayL.setsize(0);
      delayR.setsize(0);
      delayWL.setsize(0);
      delayWR.setsize(0);
    }
  delayL.mute();
  delayR.mute();
  delayWL.mute();
  delayWR.mute();
}

long FV3_(irmodel2)::getLatency()
{
  return fragmentSize;
}

//! main process
/*!
  output is valid even inputL/R == outputL/R.
 */
void FV3_(irmodel2)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR,
				    fv3_float_t *outputL, fv3_float_t *outputR,
				    long numsamples, unsigned options)
{
  if(numsamples <= 0||impulseSize <= 0||(long)fragments.size() <= 0) return;
  
  // If the fragment size is too big, split the data.
  if(numsamples > fragmentSize)
    {
      long div = numsamples/fragmentSize;
      for(long i = 0;i < div;i ++)
	{
	  processreplace(inputL+i*fragmentSize, inputR+i*fragmentSize,
			 outputL+i*fragmentSize, outputR+i*fragmentSize,
			 fragmentSize, options);
	}
      if(numsamples%fragmentSize > 0)
	{
	  processreplace(inputL+div*fragmentSize, inputR+div*fragmentSize,
			 outputL+div*fragmentSize, outputR+div*fragmentSize,
			 numsamples%fragmentSize, options);
	}
      return;
    }

  // main process
  if((options & FV3_IR_SWAP_LR) != 0)
    {
      fv3_float_t * swapT = outputL;
      outputL = outputR;
      outputR = swapT;
    }
  
  fv3_float_t *fpLSp, *fpRSp;  
  std::memcpy(fifoSlot.L+fifoSize+fragmentSize, inputL, sizeof(fv3_float_t)*numsamples);
  std::memcpy(fifoSlot.R+fifoSize+fragmentSize, inputR, sizeof(fv3_float_t)*numsamples);
  if(fifoSize+numsamples >= fragmentSize)
    {
      processSquare(fifoSlot.L+fragmentSize, fifoSlot.R+fragmentSize,
		    fifoSlot.L+fragmentSize, fifoSlot.R+fragmentSize,
		    options);
    }
  fpLSp = fifoSlot.L+fifoSize;
  fpRSp = fifoSlot.R+fifoSize;
  
  // process output loop
  if((options & FV3_IR_SKIP_FILTER) == 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  fpLSp[i] = filter.processL(fpLSp[i]);
	  fpRSp[i] = filter.processR(fpRSp[i]);
	}
    }
  
  for(long i = 0;i < numsamples;i ++)
    {
      fpLSp[i] = delayWL.process(fpLSp[i]);
      fpRSp[i] = delayWR.process(fpRSp[i]);
    }
  if((options & FV3_IR_SKIP_INIT) == 0)
    {
      FV3_(utils)::mute(outputL, numsamples);
      FV3_(utils)::mute(outputR, numsamples);
    }
  if((options & FV3_IR_MUTE_WET) == 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] += fpLSp[i]*wet1L + fpRSp[i]*wet2L;
	  outputR[i] += fpRSp[i]*wet1R + fpLSp[i]*wet2R;
	}
    }
  if((options & FV3_IR_MUTE_DRY) == 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] += delayL.process(inputL[i])*dry;
	  outputR[i] += delayR.process(inputR[i])*dry;
	}
    }
  
  fifoSize += numsamples;
  if(fifoSize >= fragmentSize)
    {
      std::memmove(fifoSlot.L, fifoSlot.L+fragmentSize, sizeof(fv3_float_t)*2*fragmentSize);
      std::memmove(fifoSlot.R, fifoSlot.R+fragmentSize, sizeof(fv3_float_t)*2*fragmentSize);
      fifoSize -= fragmentSize;
    }
  return;
}

void FV3_(irmodel2)::processSquare(fv3_float_t *inputL, fv3_float_t *inputR,
				   fv3_float_t *outputL, fv3_float_t *outputR,
				   unsigned options)
{
  if((options & FV3_IR_MONO2STEREO) != 0)
    {
      for(long i = 0;i < fragmentSize;i ++)
	inputL[i] = inputR[i] = (inputL[i] + inputR[i])/2.0;
    }
  fragmentsFFT.R2HC(inputL, inputR, ifftSlot.L, ifftSlot.R);
  swapSlot.mute();
  for(long i = 0;i < (long)fragments.size();i ++)
    {
      fragments[i]->MULT(blkdelayL.at(ifftSlot.L, i), blkdelayR.at(ifftSlot.R, i), swapSlot.L, swapSlot.R);
    }
  fragmentsFFT.HC2R(swapSlot.L, swapSlot.R, reverseSlot.L, reverseSlot.R);
  std::memcpy(outputL, reverseSlot.L, sizeof(fv3_float_t)*fragmentSize);
  std::memcpy(outputR, reverseSlot.R, sizeof(fv3_float_t)*fragmentSize);
  std::memcpy(reverseSlot.L, reverseSlot.L+fragmentSize, sizeof(fv3_float_t)*(fragmentSize-1));
  std::memcpy(reverseSlot.R, reverseSlot.R+fragmentSize, sizeof(fv3_float_t)*(fragmentSize-1));
  reverseSlot.mute(fragmentSize-1, fragmentSize+1);
}

#include "freeverb/fv3_ns_end.h"
