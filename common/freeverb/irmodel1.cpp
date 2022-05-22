/**
 *  Impulse Response Processor model implementation
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

#include "freeverb/irmodel1.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// irmodel1m

FV3_(irmodel1m)::FV3_(irmodel1m)()
{
  impulseSize = fragmentSize = current = 0;
}

FV3_(irmodel1m)::FV3_(~irmodel1m)()
{
  ;
}

void FV3_(irmodel1m)::loadImpulse(const fv3_float_t * inputL, long size)
  throw(std::bad_alloc)
{
  if(size <= 0) return;
  unloadImpulse();
 
  // fragmentSize = pulse = 2^n >= size = impulseSize
  // if fragSize < impulseSize, the impulse should be 0 padded to reduce latency.
  long pulse = FV3_(utils)::checkPow2(size);
#ifdef DEBUG
  std::fprintf(stderr, "irmodel1m::loadImpulse(): Size=%ld(impulse)->%ld(fragment)\n", size, pulse);
#endif
  impulseSize  = fifopt = size;
  fragmentSize = pulse;
  try
	{
	  fftImpl.alloc(2*fragmentSize, 1);
	  fifo.alloc(3*impulseSize, 1);
	  delayline.alloc(2*impulseSize, 1);
	  
	  FV3_(slot) impulse; // normalize impulse with fragment length
	  impulse.alloc(2*fragmentSize, 1);
	  for(long i = 0;i < size;i ++){ impulse.L[i] = inputL[i]/(fv3_float_t)(fragmentSize*2); }
	  FFTW_(plan) planL; // DFT 2^n impulse
	  planL = FFTW_(plan_r2r_1d)(2*fragmentSize, impulse.L, fftImpl.L, FFTW_R2HC, FFTW_ESTIMATE);
	  FFTW_(execute)(planL);
	  FFTW_(destroy_plan)(planL);
	  
	  fftRevr.alloc(2*fragmentSize, 1); // input signal processing plans
	  planRevrL = FFTW_(plan_r2r_1d)(fragmentSize*2, fftRevr.L, fftRevr.L, FFTW_HC2R, fftflags);
	  planOrigL = FFTW_(plan_r2r_1d)(fragmentSize*2, fftRevr.L, fftRevr.L, FFTW_R2HC, fftflags);

      latency = impulseSize;
      mute();
	}
  catch(std::bad_alloc&)
	{
      std::fprintf(stderr, "irmodel1m::loadImpulse(%ld) bad_alloc\n", size);
      unloadImpulse();
      throw;
    }
}

void FV3_(irmodel1m)::unloadImpulse()
{
  if(impulseSize == 0) return;
  impulseSize = current = fifopt = 0;
  fifo.free();
  delayline.free();
  fftImpl.free();
  fftRevr.free();
  FFTW_(destroy_plan)(planRevrL);
  FFTW_(destroy_plan)(planOrigL);
}

void FV3_(irmodel1m)::mute()
{
  current = 0;
  delayline.mute();
  fifo.mute();
}

long FV3_(irmodel1m)::getFragmentSize()
{
  if(impulseSize == 0) return 0;
  return fragmentSize;
}

void FV3_(irmodel1m)::processreplace(fv3_float_t *inputL, long numsamples)
{
  if(numsamples <= 0||fragmentSize <= 0) return;
  if(numsamples > impulseSize) // divide into impulseSize pieces
    {
      long div = numsamples/impulseSize;
      for(long i = 0;i < div;i ++){ processreplace(inputL+i*impulseSize, impulseSize); }
      processreplace(inputL+div*impulseSize, numsamples%impulseSize);
      return;
    }
  
  std::memcpy(fifo.L+fifopt+impulseSize, inputL, sizeof(fv3_float_t)*numsamples);
  if(fifopt+numsamples >= impulseSize){ processSquareReplace(fifo.L+impulseSize); }
  std::memcpy(inputL, fifo.L+fifopt, sizeof(fv3_float_t)*numsamples);
  fifopt += numsamples;
  if(fifopt >= impulseSize)
    {
      std::memmove(fifo.L, fifo.L+impulseSize, sizeof(fv3_float_t)*2*impulseSize);
      fifopt -= impulseSize;
    }
}

void FV3_(irmodel1m)::processSquareReplace(fv3_float_t *inputL)
{
  fftRevr.mute();
  std::memcpy(fftRevr.L, inputL, sizeof(fv3_float_t)*impulseSize);
  FFTW_(execute)(planOrigL); // inputL -DFT(replace)-> fftRevr
  fftRevr.L[0] *= fftImpl.L[0];
  fftRevr.L[fragmentSize] *= fftImpl.L[fragmentSize];
  for(long i = 1;i < fragmentSize;i ++)
    {
      {
		fv3_float_t e = fftRevr.L[i];
		fv3_float_t d = fftRevr.L[fragmentSize*2 - i];
		fv3_float_t f = fftImpl.L[i];
		fv3_float_t g = fftImpl.L[fragmentSize*2 - i];
		fftRevr.L[i] = e*f - d*g;
		fftRevr.L[2*fragmentSize-i] = e*g + f*d;
      }
    }
  FFTW_(execute)(planRevrL);
  
  // XXXXOOOO // sigma
  // OXXXXOOO
  // OOXXXXOO
  // OOOXXXXO
  // 11112222
  // -------
  // --- ----
  // ^
  //     ^
  if(current == 0)
    {
      for(long i = 0;i < impulseSize*2-1;i ++){ delayline.L[i] += fftRevr.L[i]; }
      std::memcpy(inputL, delayline.L, sizeof(fv3_float_t)*impulseSize);
      delayline.mute(impulseSize);
      current = impulseSize;
    }
  else // current == impulseSize
    {
      for(long i = 0;i < impulseSize;i ++)  { delayline.L[impulseSize+i] += fftRevr.L[i]; }
      for(long i = 0;i < impulseSize-1;i ++){ delayline.L[i] += fftRevr.L[impulseSize+i]; }
      std::memcpy(inputL, delayline.L+impulseSize, sizeof(fv3_float_t)*impulseSize);
      delayline.mute(impulseSize, impulseSize); // mute with offset
      current = 0;
    }
}

// irmodel1

FV3_(irmodel1)::FV3_(irmodel1)()
{
  fragmentSize = 0;
  try
    {
      irmL = new FV3_(irmodel1m);
      irmR = new FV3_(irmodel1m);
    }
  catch(std::bad_alloc&)
    {
      delete irmL;
      delete irmR;
      throw;
    }
}

FV3_(irmodel1)::FV3_(~irmodel1)()
{
  unloadImpulse();
}

void FV3_(irmodel1)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
  throw(std::bad_alloc)
{
  if(size <= 0) return;
  unloadImpulse();
  try
    {
      irmL->loadImpulse(inputL, size), irmR->loadImpulse(inputR, size);
      impulseSize = latency = size;
      FV3_(irmodel1m) *ir = dynamic_cast<FV3_(irmodel1m)*>(irmL);
      if(ir == NULL){ throw std::bad_alloc(); }
      fragmentSize = ir->getFragmentSize();
      inputW.alloc(impulseSize, 2);
      inputD.alloc(impulseSize, 2);
      setInitialDelay(getInitialDelay());
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodel1::loadImpulse(%ld) bad_alloc\n", size);
      unloadImpulse();
      throw;
    }
  mute();
}

void FV3_(irmodel1)::unloadImpulse()
{
  impulseSize = 0;
  irmL->unloadImpulse(), irmR->unloadImpulse();
  inputW.free();
  inputD.free();
}

void FV3_(irmodel1)::mute()
{
  FV3_(irbase)::mute();
  irmL->mute(), irmR->mute();
  inputW.mute();
  inputD.mute();
}

long FV3_(irmodel1)::getFragmentSize()
{
  if(impulseSize == 0) return 0;
  return fragmentSize;
}

void FV3_(irmodel1)::processreplace(const fv3_float_t *inputL, const fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  long div = numsamples/impulseSize;
  for(long i = 0;i < div;i ++) processreplaceS(inputL+i*impulseSize, inputR+i*impulseSize, outputL+i*impulseSize, outputR+i*impulseSize, impulseSize);
  processreplaceS(inputL+div*impulseSize, inputR+div*impulseSize, outputL+div*impulseSize, outputR+div*impulseSize, numsamples%impulseSize);
}

void FV3_(irmodel1)::processreplaceS(const fv3_float_t *inputL, const fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  
  if((processoptions & FV3_IR_MONO2STEREO) != 0)
    {
      for(long i = 0;i < numsamples;i ++) inputW.L[i] = inputW.R[i] = (inputL[i] + inputR[i])/2.0;
    }
  else
    {
      std::memcpy(inputW.L, inputL, sizeof(fv3_float_t)*numsamples);
      std::memcpy(inputW.R, inputR, sizeof(fv3_float_t)*numsamples);
    }
  std::memcpy(inputD.L, inputL, sizeof(fv3_float_t)*numsamples);
  std::memcpy(inputD.R, inputR, sizeof(fv3_float_t)*numsamples);
  
#pragma omp parallel
#pragma omp sections
  {
#pragma omp section
    {
      irmL->processreplace(inputW.L, numsamples);
    }
#pragma omp section
    {
      irmR->processreplace(inputW.R, numsamples);
    }
  }
#pragma omp barrier

  processdrywetout(inputD.L, inputD.R, inputW.L, inputW.R, outputL, outputR, numsamples);
}

#include "freeverb/fv3_ns_end.h"
