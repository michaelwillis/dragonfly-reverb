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

#include "freeverb/irmodel3.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// irmodel3m

FV3_(irmodel3m)::FV3_(irmodel3m)()
{
  setFragmentSize(FV3_IR3_DFragmentSize, FV3_IR3_DefaultFactor);
  Scursor = Lcursor = Lstep = 0;
}

FV3_(irmodel3m)::FV3_(~irmodel3m)()
{
  ;
}

void FV3_(irmodel3m)::loadImpulse(const fv3_float_t * inputL, long size)
  throw(std::bad_alloc)
{
  if(size <= 0) return;
  FV3_(irmodel3m)::unloadImpulse();
  
  impulseSize = size;
  long sFragmentNum = 0, lFragmentNum = 0, sFragmentMod = 0, lFragmentMod = 0;
  if(size <= lFragmentSize)
    {
      sFragmentNum = size / sFragmentSize;
      sFragmentMod = size % sFragmentSize;
      lFragmentNum = 0, lFragmentMod = 0;
    }
  else
    {
      sFragmentNum = lFragmentSize / sFragmentSize;
      sFragmentMod = 0;
      lFragmentNum = size / lFragmentSize - 1;
      lFragmentMod = size % lFragmentSize;
    }
  
#ifdef DEBUG  
  std::fprintf(stderr, "irmodel3::loadImpulse(): {L%ldx%ld+%ld/S%ldx%ld+%ld}\n", lFragmentSize, lFragmentNum, lFragmentMod,sFragmentSize, sFragmentNum, sFragmentMod);
#endif

  try
    {
      allocSlots(sFragmentSize, lFragmentSize);
      sFragmentsFFT.setSIMD(simdFlag1, simdFlag2);
      sFragmentsFFT.allocFFT(sFragmentSize, fftflags);
      lFragmentsFFT.setSIMD(simdFlag1, simdFlag2);
      lFragmentsFFT.allocFFT(lFragmentSize, fftflags);

      setSIMD(sFragmentsFFT.getSIMD(0),sFragmentsFFT.getSIMD(1));

      sImpulseFFTBlock.alloc(sFragmentSize*2*(sFragmentNum+1), 1);
      lImpulseFFTBlock.alloc(lFragmentSize*2*(lFragmentNum+1), 1);
      allocFrags(&sFragments, inputL, sFragmentSize, sFragmentNum, sFragmentMod, fftflags, sImpulseFFTBlock.L);
      if(size > lFragmentSize)
        {
          allocFrags(&lFragments, inputL+lFragmentSize, lFragmentSize, lFragmentNum, lFragmentMod, fftflags, lImpulseFFTBlock.L);
        }
      sBlockDelayL.setBlock(sFragmentSize*2, (long)sFragments.size());
      lBlockDelayL.setBlock(lFragmentSize*2, (long)lFragments.size());
      latency = 0;
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel3m::loadImpulse(%ld) bad_alloc\n", size);
      FV3_(irmodel3m)::unloadImpulse();
      throw;
    }
  FV3_(irmodel3m)::mute();
}

void FV3_(irmodel3m)::unloadImpulse()
{
  if(impulseSize == 0) return;
  impulseSize = 0;
  freeFrags(&sFragments);
  freeFrags(&lFragments);
  freeSlots();
  sFragmentsFFT.freeFFT();
  lFragmentsFFT.freeFFT();
  sImpulseFFTBlock.free();
  lImpulseFFTBlock.free();
}

void FV3_(irmodel3m)::allocFrags(std::vector<FV3_(frag)*> *to, const fv3_float_t *inputL, long fragSize, long num, long mod, unsigned fftflags, fv3_float_t * preAllocL)
  throw(std::bad_alloc)
{
  try
    {
      for(long i = 0;i < num;i ++)
        {
          FV3_(frag) * f = new FV3_(frag);
          to->push_back(f);
          f->setSIMD(simdFlag1, simdFlag2);
          f->loadImpulse(inputL+fragSize*i, fragSize, fragSize, fftflags, preAllocL+fragSize*2*i);
        }
      if(mod != 0)
        {
          FV3_(frag) * f = new FV3_(frag);
          to->push_back(f);
          f->setSIMD(simdFlag1, simdFlag2);
          f->loadImpulse(inputL+fragSize*num, fragSize, mod, fftflags, preAllocL+fragSize*2*num);
        }
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel3::allocFrags(%ld) bad_alloc\n", fragSize);
      freeFrags(to);
      throw;
    }
}

void FV3_(irmodel3m)::freeFrags(std::vector<FV3_(frag)*> *v)
{
  for(std::vector<FV3_(frag)*>::iterator i = v->begin();i != v->end();i ++) delete *i;
  v->clear();
}

void FV3_(irmodel3m)::allocSlots(long ssize, long lsize)
  throw(std::bad_alloc)
{
  fifoSlot.alloc(ssize, 1);
  sReverseSlot.alloc(2*ssize, 1);
  lReverseSlot.alloc(2*lsize, 1);
  restSlot.alloc(ssize, 1);
  sOnlySlot.alloc(ssize, 1);
  lFrameSlot.alloc(lsize, 1);
  // Following slots must be SIMD compatible slots.
  sIFFTSlot.alloc(2*ssize, 1);
  lIFFTSlot.alloc(2*lsize, 1);
  sSwapSlot.alloc(2*ssize, 1);
  lSwapSlot.alloc(2*lsize, 1);
}

void FV3_(irmodel3m)::freeSlots()
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

void FV3_(irmodel3m)::processreplace(fv3_float_t *inputL, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  long cursor = sFragmentSize - Scursor;
  if(numsamples > cursor)
    {
      processreplace(inputL, cursor);
      long div = (numsamples - cursor)/sFragmentSize;
      long mod = (numsamples - cursor)%sFragmentSize;
      for(long i = 0;i < div;i ++)
        processreplace(inputL+cursor+i*sFragmentSize, sFragmentSize);
      processreplace(inputL+cursor+div*sFragmentSize, mod);
    }

  processZL(inputL, numsamples);
}

void FV3_(irmodel3m)::processZL(fv3_float_t *inputL, long numsamples)
{
  // numsamples <= sFragmentSize - Scursor
  if(Lcursor == 0&&lFragments.size() > 0)
    {
      lFrameSlot.mute();
      lReverseSlot.mute(lFragmentSize-1, lFragmentSize+1);
      lBlockDelayL.push(lIFFTSlot.L);
      lFragments[0]->MULT(lBlockDelayL.get(0), lSwapSlot.L);
      lFragmentsFFT.HC2R(lSwapSlot.L, lReverseSlot.L);
      lSwapSlot.mute();
      // The calculation of the large fragment vector was moved from here to [LVECTOR] to reduce CPU load spike.
    }
  
  if(Scursor == 0)
    {
      sFramePointerL = lFrameSlot.L+Lcursor;
      sSwapSlot.mute();
      sBlockDelayL.push(sIFFTSlot.L);
      for(long i = 1;i < (long)sFragments.size();i ++){ sFragments[i]->MULT(sBlockDelayL.get(i-1), sSwapSlot.L); }
    }
  sOnlySlot.mute();
  
  std::memcpy(lFrameSlot.L+Lcursor, inputL, sizeof(fv3_float_t)*numsamples);
  std::memcpy(sOnlySlot.L+Scursor, inputL, sizeof(fv3_float_t)*numsamples);
  
  if(sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sOnlySlot.L, sIFFTSlot.L);
      sFragments[0]->MULT(sIFFTSlot.L, sSwapSlot.L);
      sReverseSlot.mute();
      sFragmentsFFT.HC2R(sSwapSlot.L, sReverseSlot.L);
    }
  
  if(lFragments.size() > 0)
    {
      for(long i = 0;i < numsamples;i ++){ inputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i] + (lReverseSlot.L+Lcursor)[i]; }
    }
  else
    {
      for(long i = 0;i < numsamples;i ++){ inputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i]; }
    }
  
  Scursor += numsamples, Lcursor += numsamples;
  
  // [LVECTOR] large fragment vector multiplier
  for(long i = Lstep;i < (((long)lFragments.size())-1)*Lcursor/lFragmentSize;i ++)
    {
      if(((long)lFragments.size()) > i + 1){ lFragments[i+1]->MULT(lBlockDelayL.get(i), lSwapSlot.L); }
      Lstep ++;
    }
  
  if(Scursor == sFragmentSize&&sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sFramePointerL, sIFFTSlot.L);
      std::memcpy(restSlot.L, sReverseSlot.L+sFragmentSize, sizeof(fv3_float_t)*(sFragmentSize-1));
      Scursor = 0;
    }
  
  if(Lcursor == lFragmentSize)
    {
      if(lFragments.size() > 0)
        {
          lFragmentsFFT.R2HC(lFrameSlot.L, lIFFTSlot.L);
          std::memcpy(lReverseSlot.L, lReverseSlot.L+lFragmentSize, sizeof(fv3_float_t)*(lFragmentSize-1));
        }
      Lcursor = Lstep = 0;
    }
}

void FV3_(irmodel3m)::mute()
{
  if(impulseSize == 0) return;
  Scursor = Lcursor = Lstep = 0;
  sBlockDelayL.mute();
  lBlockDelayL.mute();
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

void FV3_(irmodel3m)::setFragmentSize(long size, long factor)
{
  if(size <= 0||factor <= 0||size < FV3_IR_Min_FragmentSize||size != FV3_(utils)::checkPow2(size)||factor != FV3_(utils)::checkPow2(factor))
    {
      std::fprintf(stderr, "irmodel3::setFragmentSize(): invalid fragment size/factor (%ld/%ld)\n", size, factor);
      return;
    }
  if(sFragmentSize != size||lFragmentSize != size*factor)
    {
      FV3_(irmodel3m)::unloadImpulse();
      sFragmentSize = size;
      lFragmentSize = size*factor;
    }
}

long FV3_(irmodel3m)::getSFragmentSize(){ return sFragmentSize; }
long FV3_(irmodel3m)::getLFragmentSize(){ return lFragmentSize; }
long FV3_(irmodel3m)::getSFragmentCount(){ return (long)sFragments.size(); }
long FV3_(irmodel3m)::getLFragmentCount(){ return (long)lFragments.size(); }
long FV3_(irmodel3m)::getScursor(){ return Scursor; }

// irmodel3

FV3_(irmodel3)::FV3_(irmodel3)()
{
  fragmentSize = 0;
  delete irmL, irmL = NULL;
  delete irmR, irmR = NULL;
  try
    {
      ir3mL = new FV3_(irmodel3m);
      ir3mR = new FV3_(irmodel3m);
      irmL = ir3mL;
      irmR = ir3mR;
    }
  catch(std::bad_alloc&)
    {
      delete irmL;
      delete irmR;
      throw;
    }
  FV3_(irmodel3)::setFragmentSize(FV3_IR3_DFragmentSize, FV3_IR3_DefaultFactor);
}

FV3_(irmodel3)::FV3_(~irmodel3)()
{
  FV3_(irmodel3)::unloadImpulse();
}

void FV3_(irmodel3)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
  throw(std::bad_alloc)
{
  if(size <= 0||getSFragmentSize() < FV3_IR_Min_FragmentSize||getLFragmentSize() < FV3_IR_Min_FragmentSize) return;
  FV3_(irmodel3)::unloadImpulse();
  setSIMD(irmL->getSIMD(0),irmL->getSIMD(1));
  try
    {
      irmL->loadImpulse(inputL, size), irmR->loadImpulse(inputR, size);
      impulseSize = size;
      latency = 0;
      inputW.alloc(getSFragmentSize(), 2);
      inputD.alloc(getSFragmentSize(), 2);
      FV3_(irbase)::setInitialDelay(getInitialDelay());
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel3::loadImpulse(%ld) bad_alloc\n", size);
      FV3_(irmodel3)::unloadImpulse();
      throw;
    }
  FV3_(irmodel1)::mute();
}

void FV3_(irmodel3)::setFragmentSize(long size, long factor)
{
  if(size <= 0||factor <= 0||size < FV3_IR_Min_FragmentSize||size != FV3_(utils)::checkPow2(size)||factor != FV3_(utils)::checkPow2(factor))
    {
      std::fprintf(stderr, "irmodel3::setFragmentSize(): invalid fragment size/factor (%ld/%ld)\n", size, factor);
      return;
    }
  if(getSFragmentSize() != size||getLFragmentSize() != size*factor)
    {
      FV3_(irmodel3)::unloadImpulse();
      ir3mL->setFragmentSize(size, factor);
      ir3mR->setFragmentSize(size, factor);
    }
}

void FV3_(irmodel3)::processreplace(const fv3_float_t *inputL, const fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  long sFragmentSize = getSFragmentSize();
  long cursor = sFragmentSize - ir3mL->getScursor();  
  if(numsamples > cursor)
    {
      processreplaceS(inputL, inputR, outputL, outputR, cursor);
      long div = (numsamples - cursor)/sFragmentSize;
      long mod = (numsamples - cursor)%sFragmentSize;
      for(long i = 0;i < div;i ++)
        processreplaceS(inputL+cursor+i*sFragmentSize, inputR+cursor+i*sFragmentSize, outputL+cursor+i*sFragmentSize, outputR+cursor+i*sFragmentSize, sFragmentSize);
      processreplaceS(inputL+cursor+div*sFragmentSize, inputR+cursor+div*sFragmentSize, outputL+cursor+div*sFragmentSize, outputR+cursor+div*sFragmentSize, mod);
    }
  else
    {
      processreplaceS(inputL, inputR, outputL, outputR, numsamples);
    }
}

long FV3_(irmodel3)::getSFragmentSize(){ return ir3mL->getSFragmentSize(); }
long FV3_(irmodel3)::getLFragmentSize(){ return ir3mL->getLFragmentSize(); }
long FV3_(irmodel3)::getSFragmentCount(){ return ir3mL->getSFragmentCount(); }
long FV3_(irmodel3)::getLFragmentCount(){ return ir3mL->getLFragmentCount(); }

void FV3_(irmodel3)::printconfig()
{
  std::fprintf(stderr, "*** irmodel3 config ***\n");
  std::fprintf(stderr, "impulseSize = %ld\n", impulseSize);
  std::fprintf(stderr, "short fragment Size = %ld\n", getSFragmentSize());
  std::fprintf(stderr, "large fragment Size = %ld\n", getLFragmentSize());
  std::fprintf(stderr, "short fragment vector Length = %ld\n", getSFragmentCount());
  std::fprintf(stderr, "large fragment vector Length = %ld\n", getLFragmentCount());
}

#include "freeverb/fv3_ns_end.h"
