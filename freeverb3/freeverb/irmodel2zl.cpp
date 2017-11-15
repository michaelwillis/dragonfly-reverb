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

#include "freeverb/irmodel2zl.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(irmodel2zl)::FV3_(irmodel2zl)()

{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2zl::irmodel2zl()\n");
#endif
  ZLstart = 0;
}

FV3_(irmodel2zl)::FV3_(~irmodel2zl)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2zl::~irmodel2zl()\n");
#endif
}

void FV3_(irmodel2zl)::mute()
{
  if(impulseSize == 0) return;
  FV3_(irmodel2)::mute();
  ZLstart = 0;
  ZLdelayL.mute();
  ZLdelayR.mute();
  ZLdelayWL.mute();
  ZLdelayWR.mute();
  zlFrameSlot.mute();
  zlOnlySlot.mute();
}

void FV3_(irmodel2zl)::allocSwap(long size)
		      throw(std::bad_alloc)
{
  freeSwap();
  FV3_(irmodel2)::allocSwap(size);
  zlOnlySlot.alloc(size, 2);
  zlFrameSlot.alloc(size, 2);
}

void FV3_(irmodel2zl)::freeSwap()
{
  FV3_(irmodel2)::freeSwap();
  zlFrameSlot.free();
  zlOnlySlot.free();
}

void FV3_(irmodel2zl)::setInitialDelay(long numsamples)
		      throw(std::bad_alloc)
{
  initialDelay = numsamples;
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
  ZLdelayL.mute();
  ZLdelayR.mute();
  ZLdelayWL.mute();
  ZLdelayWR.mute();
}

long FV3_(irmodel2zl)::getLatency()
{
  return 0;
}

//! main process
/*!
  output is valid even inputL/R == outputL/R.
 */
void FV3_(irmodel2zl)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR,
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
  long cursor = fragmentSize - ZLstart;
  // numsamples <= fragmentSize
  // numsamples-cursor <= fragmentSize-cursor = ZLstart <= fragmentSize
  if(cursor >= numsamples)
    {
      processZL(inputL, inputR, fifoSlot.L, fifoSlot.R, numsamples, options);
    }
  else
    {
      processZL(inputL, inputR, fifoSlot.L, fifoSlot.R, cursor, options);
      processZL(inputL+cursor, inputR+cursor, fifoSlot.L+cursor, fifoSlot.R+cursor,
		numsamples-cursor, options);
    }
  fpLSp = fifoSlot.L;
  fpRSp = fifoSlot.R;
  
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
      fpLSp[i] = ZLdelayL.process(fpLSp[i]);
      fpRSp[i] = ZLdelayR.process(fpRSp[i]);
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
	  outputL[i] += ZLdelayWL.process(inputL[i])*dry;
	  outputR[i] += ZLdelayWR.process(inputR[i])*dry;
	}
    }
  return;
}

void FV3_(irmodel2zl)::processZL(fv3_float_t *inputL, fv3_float_t *inputR,
				 fv3_float_t *outputL, fv3_float_t *outputR,
				 long numsamples, unsigned options)
{
  // numsamples <= fragmentSize - ZLstart
  if(ZLstart == 0)
    {
      zlFrameSlot.mute();
      reverseSlot.mute(fragmentSize-1, fragmentSize+1);
      swapSlot.mute();
      for(long i = 1;i < (long)fragments.size();i ++)
	{
	  fragments[i]->MULT(blkdelayL.at(ifftSlot.L, i-1), blkdelayR.at(ifftSlot.R, i-1), swapSlot.L, swapSlot.R);
	}
    }
  zlOnlySlot.mute();
  if((options & FV3_IR_MONO2STEREO) != 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  (zlFrameSlot.L+ZLstart)[i] = (zlFrameSlot.R+ZLstart)[i]
	    = (zlOnlySlot.L+ZLstart)[i] = (zlOnlySlot.R+ZLstart)[i]
	    = (inputL[i] + inputR[i])/2.0;
	}
    }
  else
    {
      std::memcpy(zlFrameSlot.L+ZLstart, inputL, sizeof(fv3_float_t)*numsamples);
      std::memcpy(zlFrameSlot.R+ZLstart, inputR, sizeof(fv3_float_t)*numsamples);
      std::memcpy(zlOnlySlot.L+ZLstart, inputL, sizeof(fv3_float_t)*numsamples);
      std::memcpy(zlOnlySlot.R+ZLstart, inputR, sizeof(fv3_float_t)*numsamples);
    }

  fragmentsFFT.R2HC(zlOnlySlot.L, zlOnlySlot.R, ifftSlot.L, ifftSlot.R);
  fragments[0]->MULT(ifftSlot.L, ifftSlot.R, swapSlot.L, swapSlot.R);
  reverseSlot.mute();
  fragmentsFFT.HC2R(swapSlot.L, swapSlot.R, reverseSlot.L, reverseSlot.R);
  
  for(long i = 0;i < numsamples;i ++)
    {
      outputL[i] = (reverseSlot.L+ZLstart)[i] + (restSlot.L+ZLstart)[i];
      outputR[i] = (reverseSlot.R+ZLstart)[i] + (restSlot.L+ZLstart)[i];
    }
  ZLstart += numsamples;
  if(ZLstart == fragmentSize)
    {
      fragmentsFFT.R2HC(zlFrameSlot.L, zlFrameSlot.R, ifftSlot.L, ifftSlot.R);
      std::memcpy(restSlot.L, reverseSlot.L+fragmentSize, sizeof(fv3_float_t)*(fragmentSize-1));
      std::memcpy(restSlot.R, reverseSlot.R+fragmentSize, sizeof(fv3_float_t)*(fragmentSize-1));
      ZLstart = 0;
    }
}

#include "freeverb/fv3_ns_end.h"
