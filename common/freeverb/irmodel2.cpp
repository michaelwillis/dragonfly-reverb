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

#include "freeverb/irmodel2.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// irmodel2m

FV3_(irmodel2m)::FV3_(irmodel2m)()
{
  setFragmentSize(FV3_IR2_DFragmentSize);
  fifoSize = 0;
}

FV3_(irmodel2m)::FV3_(~irmodel2m)()
{
  ;
}

void FV3_(irmodel2m)::loadImpulse(const fv3_float_t * inputL, long size)
  throw(std::bad_alloc)
{
  if(size <= 0) return;
  unloadImpulse();
  
  // For optimization, fragmentSize should be overriden if fragmentSize >>> impulsesize:
  // if(FV3_(utils)::checkPow2(impulsesize)/2 < size) fragmentSize = FV3_(utils)::checkPow2(size)/2;
  long fragment_num = size / fragmentSize;
  long fragment_mod = size % fragmentSize;
  try
    {
      fifoSlot.alloc(3*fragmentSize, 1);
      reverseSlot.alloc(2*fragmentSize, 1);
      restSlot.alloc(fragmentSize, 1);
      ifftSlot.alloc(2*fragmentSize, 1);
      swapSlot.alloc(2*fragmentSize, 1);
	  
      fragFFT.setSIMD(simdFlag1, simdFlag2);
      fragFFT.allocFFT(fragmentSize, fftflags);
      setSIMD(fragFFT.getSIMD(0),fragFFT.getSIMD(1));
      
      for(long i = 0;i < fragment_num;i ++)
		{
		  FV3_(frag) * f = new FV3_(frag);
		  fragments.push_back(f);
		  f->setSIMD(simdFlag1, simdFlag2);
		  f->loadImpulse(inputL+fragmentSize*i, fragmentSize, fragmentSize, fftflags);
		}
      if(fragment_mod != 0)
		{
		  FV3_(frag) * f = new FV3_(frag);
		  fragments.push_back(f);
		  f->setSIMD(simdFlag1, simdFlag2);
		  f->loadImpulse(inputL+fragmentSize*fragment_num, fragmentSize, fragment_mod, fftflags);
		}
      blkdelayDL.setBlock(fragmentSize*2, (long)fragments.size());
      impulseSize = size;
      latency = fragmentSize;
      mute();
#ifdef DEBUG
      std::fprintf(stderr, "irmodel2m::loadImpulse(): {%ldx%ld+%ld}\n", fragmentSize, fragment_num, fragment_mod);
#endif
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel2m::loadImpulse(%ld) bad_alloc\n", size);
      unloadImpulse();
      throw;
    }
}

void FV3_(irmodel2m)::unloadImpulse()
{
  if(impulseSize == 0) return;
  impulseSize = 0;
  fifoSlot.free();
  reverseSlot.free();
  ifftSlot.free();
  swapSlot.free();
  restSlot.free();
  fragFFT.freeFFT();
  for(std::vector<FV3_(frag)*>::iterator i = fragments.begin();i != fragments.end();i ++) delete *i;
  fragments.clear();
}

void FV3_(irmodel2m)::processreplace(fv3_float_t *inputL, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  if(numsamples > fragmentSize) // divide into fragmentSize pieces
    {
      long div = numsamples/fragmentSize;
      for(long i = 0;i < div;i ++){ processreplace(inputL+i*fragmentSize, fragmentSize); }
      processreplace(inputL+div*fragmentSize, numsamples%fragmentSize);
      return;
    }

  std::memcpy(fifoSlot.L+fifoSize+fragmentSize, inputL, sizeof(fv3_float_t)*numsamples);
  if(fifoSize+numsamples >= fragmentSize)
    {
      fragFFT.R2HC(fifoSlot.L+fragmentSize, ifftSlot.L);
      swapSlot.mute();
      for(long i = 0;i < (long)fragments.size();i ++){ fragments[i]->MULT(blkdelayDL.at(ifftSlot.L, i), swapSlot.L); }
      fragFFT.HC2R(swapSlot.L, reverseSlot.L);
      std::memcpy(fifoSlot.L+fragmentSize, reverseSlot.L, sizeof(fv3_float_t)*fragmentSize);
      std::memcpy(reverseSlot.L, reverseSlot.L+fragmentSize, sizeof(fv3_float_t)*(fragmentSize-1));
      reverseSlot.mute(fragmentSize-1, fragmentSize+1);
    }
  std::memcpy(inputL, fifoSlot.L+fifoSize, sizeof(fv3_float_t)*numsamples);  
  fifoSize += numsamples;
  if(fifoSize >= fragmentSize)
    {
      std::memmove(fifoSlot.L, fifoSlot.L+fragmentSize, sizeof(fv3_float_t)*2*fragmentSize);
      fifoSize -= fragmentSize;
    }
}

long FV3_(irmodel2m)::getFragmentSize()
{
  return fragmentSize;
}

void FV3_(irmodel2m)::setFragmentSize(long size)
{
  if(size <= 0||size < FV3_IR_Min_FragmentSize||size != FV3_(utils)::checkPow2(size))
    {
      std::fprintf(stderr, "irmodel2m::setFragmentSize(): invalid fragment size (%ld)\n", size);      
      return;
    }
  fragmentSize = size;
}

void FV3_(irmodel2m)::mute()
{
  fifoSize = fragmentSize;
  blkdelayDL.mute();
  blkdelayDR.mute();
  fifoSlot.mute();
  reverseSlot.mute();
  ifftSlot.mute();
  restSlot.mute();
  swapSlot.mute();
}

// irmodel2

FV3_(irmodel2)::FV3_(irmodel2)()
{
  delete irmL, irmL = NULL;
  delete irmR, irmR = NULL;
  try
    {
      ir2mL = new FV3_(irmodel2m);
      ir2mR = new FV3_(irmodel2m);
      irmL = ir2mL;
      irmR = ir2mR;
    }
  catch(std::bad_alloc&)
    {
      delete irmL;
      delete irmR;
      throw;
    }
  setFragmentSize(FV3_IR2_DFragmentSize);
}

FV3_(irmodel2)::FV3_(~irmodel2)()
{
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::~irmodel2()\n");
#endif
  FV3_(irmodel2)::unloadImpulse();
}

void FV3_(irmodel2)::setFragmentSize(long size)
{
  if(size <= 0||size < FV3_IR_Min_FragmentSize||size != FV3_(utils)::checkPow2(size))
    {
      std::fprintf(stderr, "irmodel2::setFragmentSize(): invalid fragment size (%ld)\n", size);      
      return;
    }
#ifdef DEBUG
  std::fprintf(stderr, "irmodel2::setFragmentSize(): %ld -> %ld\n", fragmentSize, size);
#endif
  unloadImpulse();
  ir2mL->setFragmentSize(size), ir2mR->setFragmentSize(size);
  fragmentSize = size;
}

void FV3_(irmodel2)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
  throw(std::bad_alloc)
{
  if(size <= 0||fragmentSize < FV3_IR_Min_FragmentSize) return;
  unloadImpulse();
  setSIMD(irmL->getSIMD(0),irmL->getSIMD(1));
  try
    {
      irmL->loadImpulse(inputL, size), irmR->loadImpulse(inputR, size);
      impulseSize = size;
      latency = fragmentSize;
      inputW.alloc(fragmentSize, 2);
      inputD.alloc(fragmentSize, 2);
      setInitialDelay(getInitialDelay());
      mute();
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel2::loadImpulse(%ld) bad_alloc\n", size);
      unloadImpulse();
      throw;
    }
}

void FV3_(irmodel2)::processreplace(const fv3_float_t *inputL, const fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  long div = numsamples/fragmentSize;
  for(long i = 0;i < div;i ++)
    processreplaceS(inputL+i*fragmentSize, inputR+i*fragmentSize, outputL+i*fragmentSize, outputR+i*fragmentSize, fragmentSize);
  processreplaceS(inputL+div*fragmentSize, inputR+div*fragmentSize, outputL+div*fragmentSize, outputR+div*fragmentSize, numsamples%fragmentSize);
}

#include "freeverb/fv3_ns_end.h"
