/**
 *  Impulse Response Processor model implementation
 *  Low Latency Version
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

#include "freeverb/irmodel2zl.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// irmodel2zlm

FV3_(irmodel2zlm)::FV3_(irmodel2zlm)()
{
  ZLstart = 0;
}

FV3_(irmodel2zlm)::FV3_(~irmodel2zlm)()
{
  ;
}

void FV3_(irmodel2zlm)::loadImpulse(const fv3_float_t * inputL, long size)
  throw(std::bad_alloc)
{
  if(size <= 0) return;
  unloadImpulse();
  try
    {
	  FV3_(irmodel2m)::loadImpulse(inputL, size);
	  zlFrameSlot.alloc(fragmentSize, 1);
	  zlOnlySlot.alloc(fragmentSize, 1);
      latency = 0;
      mute();
	}
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel2zlm::loadImpulse(%ld) bad_alloc\n", size);
      unloadImpulse();
      throw;
    }
}

void FV3_(irmodel2zlm)::unloadImpulse()
{
  if(impulseSize == 0) return;
  FV3_(irmodel2m)::unloadImpulse();
  zlFrameSlot.free();
  zlOnlySlot.free();
}

void FV3_(irmodel2zlm)::processreplace(fv3_float_t *inputL, long numsamples)
{
  if(numsamples <= 0||fragmentSize <= 0) return;
  if(numsamples > fragmentSize) // divide into fragmentSize pieces
    {
      long div = numsamples/fragmentSize;
      for(long i = 0;i < div;i ++){ processreplace(inputL+i*fragmentSize, fragmentSize); }
      processreplace(inputL+div*fragmentSize, numsamples%fragmentSize);
      return;
    }

  long cursor = fragmentSize - ZLstart;
  // numsamples <= fragmentSize
  // numsamples-cursor <= fragmentSize-cursor = ZLstart <= fragmentSize
  if(cursor >= numsamples)
    {
      processZL(inputL,        fifoSlot.L,        numsamples);
    }
  else
    {
      processZL(inputL,        fifoSlot.L,        cursor);
      processZL(inputL+cursor, fifoSlot.L+cursor, numsamples-cursor);
    }
  std::memcpy(inputL, fifoSlot.L, sizeof(fv3_float_t)*numsamples);
}

void FV3_(irmodel2zlm)::processZL(fv3_float_t *inputL, fv3_float_t *outputL, long numsamples)
{
  // numsamples <= fragmentSize - ZLstart
  if(ZLstart == 0)
    {
      zlFrameSlot.mute();
      reverseSlot.mute(fragmentSize-1, fragmentSize+1);
      swapSlot.mute();
      for(long i = 1;i < (long)fragments.size();i ++){ fragments[i]->MULT(blkdelayDL.at(ifftSlot.L, i-1), swapSlot.L); }
    }
  zlOnlySlot.mute();
  std::memcpy(zlFrameSlot.L+ZLstart, inputL, sizeof(fv3_float_t)*numsamples);
  std::memcpy(zlOnlySlot.L+ZLstart, inputL, sizeof(fv3_float_t)*numsamples);
  
  fragFFT.R2HC(zlOnlySlot.L, ifftSlot.L);
  fragments[0]->MULT(ifftSlot.L, swapSlot.L);
  reverseSlot.mute();
  fragFFT.HC2R(swapSlot.L, reverseSlot.L);
  
  for(long i = 0;i < numsamples;i ++){ outputL[i] = (reverseSlot.L+ZLstart)[i] + (restSlot.L+ZLstart)[i]; }
  ZLstart += numsamples;
  if(ZLstart == fragmentSize)
    {
      fragFFT.R2HC(zlFrameSlot.L, ifftSlot.L);
      std::memcpy(restSlot.L, reverseSlot.L+fragmentSize, sizeof(fv3_float_t)*(fragmentSize-1));
      ZLstart = 0;
    }
}

void FV3_(irmodel2zlm)::mute()
{
  FV3_(irmodel2m)::mute();
  ZLstart = 0;
  zlFrameSlot.mute();
  zlOnlySlot.mute();
}

// irmodel2zl

FV3_(irmodel2zl)::FV3_(irmodel2zl)()
{
  delete irmL, irmL = NULL;
  delete irmR, irmR = NULL;
  try
    {
      ir2mL = new FV3_(irmodel2zlm);
      ir2mR = new FV3_(irmodel2zlm);
      irmL = ir2mL;
      irmR = ir2mR;
    }
  catch(std::bad_alloc&)
    {
      delete irmL;
      delete irmR;
      throw;
    }
}

FV3_(irmodel2zl)::FV3_(~irmodel2zl)()
{
  ;
}

void FV3_(irmodel2zl)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
  throw(std::bad_alloc)
{
  if(size <= 0||fragmentSize < FV3_IR_Min_FragmentSize) return;
  unloadImpulse();
  try
    {
	  FV3_(irmodel2)::loadImpulse(inputL, inputR, size);
	  latency = 0;
      setInitialDelay(getInitialDelay());
      mute();
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel2zl::loadImpulse(%ld) bad_alloc\n", size);
      unloadImpulse();
      throw;
    }
}

#include "freeverb/fv3_ns_end.h"
