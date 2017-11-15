/**
 *  Impulse Response Processor model implementation
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

#include "freeverb/irmodel.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(irmodel)::FV3_(irmodel)()
{
  current = fftSize = fragmentSize = 0;
}

FV3_(irmodel)::FV3_(~irmodel)()
{
  FV3_(irmodel)::unloadImpulse();
}

void FV3_(irmodel)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
		   throw(std::bad_alloc)
{
  // if fragSize < impulseSize, the impulse should be 0 padded
  // to reduce latency.
  FV3_(irmodel)::unloadImpulse();
  impulseSize = size;
  long pulse = FV3_(utils)::checkPow2(size);
#ifdef DEBUG
  std::fprintf(stderr, "irmodel::loadImpulse(): %ld(impulseSize)->%ld(fragmentSize)\n", size, pulse);
#endif
  // fragmentSize = pulse = 2^n >= size = impulseSize
  FV3_(slot) impulse;
  impulse.alloc(2*pulse, 2);
  allocImpulse(pulse, size);

  for(long i = 0;i < size;i ++)
    {
      impulse.L[i] = inputL[i]/(fv3_float_t)(pulse*2);
      impulse.R[i] = inputR[i]/(fv3_float_t)(pulse*2);
    }
  
  FFTW_(plan) planL, planR;
  planL = FFTW_(plan_r2r_1d)(2*pulse, impulse.L, fftImpulse.L, FFTW_R2HC, FFTW_ESTIMATE);
  planR = FFTW_(plan_r2r_1d)(2*pulse, impulse.R, fftImpulse.R, FFTW_R2HC, FFTW_ESTIMATE);
  FFTW_(execute)(planL);
  FFTW_(execute)(planR);
  FFTW_(destroy_plan)(planL);
  FFTW_(destroy_plan)(planR);
  allocFFT(pulse, fftflags);
  delayL.setsize(size);
  delayR.setsize(size);  
  FV3_(irmodel)::mute();
}

void FV3_(irmodel)::unloadImpulse()
{
  impulseSize = 0;
  freeFFT();
  freeImpulse();
}

void FV3_(irmodel)::mute()
{
  current = 0;
  delayline.mute();
  fifo.mute();
  delayL.mute();
  delayR.mute();
  filter.mute();
}

long FV3_(irmodel)::getFragmentSize()
{
  return fragmentSize;
}

long FV3_(irmodel)::getLatency()
{
  return impulseSize;
}

void FV3_(irmodel)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR,
				   fv3_float_t *outputL, fv3_float_t *outputR,
				   long numsamples, unsigned options)
{
  if(numsamples <= 0||fragmentSize <= 0) return;
  if(numsamples > impulseSize)
    {
      // divide
      long div = numsamples/impulseSize;
      for(long i = 0;i < div;i ++)
	{
	  processreplace(inputL+i*impulseSize, inputR+i*impulseSize,
			 outputL+i*impulseSize, outputR+i*impulseSize,
			 impulseSize, options);
	}
      processreplace(inputL+div*impulseSize, inputR+div*impulseSize,
		     outputL+div*impulseSize, outputR+div*impulseSize,
		     numsamples%impulseSize, options);
      return;
    }
  
  std::memcpy(fifo.L+fifoSize+impulseSize, inputL, sizeof(fv3_float_t)*numsamples);
  std::memcpy(fifo.R+fifoSize+impulseSize, inputR, sizeof(fv3_float_t)*numsamples);
  if(fifoSize+numsamples >= impulseSize)
    {
      processSquare(fifo.L+impulseSize, fifo.R+impulseSize,
		    fifo.L+impulseSize, fifo.R+impulseSize);
    }
  for(long i = 0;i < numsamples;i ++)
    {
      // replacing
      outputL[i] = fifo.L[i+fifoSize]*wet1L + fifo.R[i+fifoSize]*wet2L +
	delayL.process(inputL[i])*dry;
      outputR[i] = fifo.R[i+fifoSize]*wet1R + fifo.L[i+fifoSize]*wet2R +
	delayR.process(inputR[i])*dry;
    }
  fifoSize += numsamples;
  if(fifoSize >= impulseSize)
    {
      std::memmove(fifo.L, fifo.L+impulseSize, sizeof(fv3_float_t)*2*impulseSize);
      std::memmove(fifo.R, fifo.R+impulseSize, sizeof(fv3_float_t)*2*impulseSize);
      fifoSize -= impulseSize;
    }
  return;
}

void FV3_(irmodel)::processSquare(fv3_float_t *inputL, fv3_float_t *inputR,
				  fv3_float_t *outputL, fv3_float_t *outputR)
{
  fftOrig.mute();
  std::memcpy(fftOrig.L, inputL, sizeof(fv3_float_t)*impulseSize);
  std::memcpy(fftOrig.R, inputR, sizeof(fv3_float_t)*impulseSize);
  FFTW_(execute)(planOrigL);
  FFTW_(execute)(planOrigR);
  
  // mul
  fftRev.L[0] = fftOrig.L[0] * fftImpulse.L[0];
  fftRev.R[0] = fftOrig.R[0] * fftImpulse.R[0];
  fftRev.L[fragmentSize] = fftOrig.L[fragmentSize] * fftImpulse.L[fragmentSize];
  fftRev.R[fragmentSize] = fftOrig.R[fragmentSize] * fftImpulse.R[fragmentSize];
  for(long i = 1;i < fragmentSize;i ++)
    {
      {
	fv3_float_t e = fftOrig.L[i];
	fv3_float_t d = fftOrig.L[fragmentSize*2 - i];
	fv3_float_t f = fftImpulse.L[i];
	fv3_float_t g = fftImpulse.L[fragmentSize*2 - i];
	fftRev.L[2*fragmentSize-i] = e*g + f*d;
	fftRev.L[i] = e*f - d*g;
      }
      {
	fv3_float_t e = fftOrig.R[i];
	fv3_float_t d = fftOrig.R[fragmentSize*2 - i];
	fv3_float_t f = fftImpulse.R[i];
	fv3_float_t g = fftImpulse.R[fragmentSize*2 - i];
	fftRev.R[2*fragmentSize-i] = e*g + f*d;
	fftRev.R[i] = e*f - d*g;
      }
    }
  
  FFTW_(execute)(planRevL);
  FFTW_(execute)(planRevR);
  
  // sigma
  // XXXXOOOO
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
      for(long i = 0;i < impulseSize*2-1;i ++)
	{
	  delayline.L[i] += fftRev.L[i];
	  delayline.R[i] += fftRev.R[i];
	}
      std::memcpy(outputL, delayline.L, sizeof(fv3_float_t)*impulseSize);
      std::memcpy(outputR, delayline.R, sizeof(fv3_float_t)*impulseSize);
      delayline.mute(impulseSize);
      current = impulseSize;
    }
  else // current == impulseSize
    {
      for(long i = 0;i < impulseSize;i ++)
	{
	  delayline.L[impulseSize+i] += fftRev.L[i];
	  delayline.R[impulseSize+i] += fftRev.R[i];
	}
      for(long i = 0;i < impulseSize-1;i ++)
	{
	  delayline.L[i] += fftRev.L[impulseSize+i];
	  delayline.R[i] += fftRev.R[impulseSize+i];
	}
      std::memcpy(outputL, delayline.L+impulseSize, sizeof(fv3_float_t)*impulseSize);
      std::memcpy(outputR, delayline.R+impulseSize, sizeof(fv3_float_t)*impulseSize);
      delayline.mute(impulseSize, impulseSize);
      current = 0;
    }
}

void FV3_(irmodel)::allocImpulse(long fsize, long isize)
		   throw(std::bad_alloc)
{
  freeImpulse();
  fragmentSize = fsize;
  fifoSize = isize;
  fifo.alloc(3*isize, 2);
  delayline.alloc(2*isize, 2);
  fftImpulse.alloc(2*fsize, 2);
}

void FV3_(irmodel)::freeImpulse()
{
  if(fragmentSize == 0) return;
  fifo.free();
  delayline.free();
  fftImpulse.free();
  fifoSize = fragmentSize = 0;
}

void FV3_(irmodel)::allocFFT(long size, unsigned fftflags)
		   throw(std::bad_alloc)
{
  freeFFT();
  fftRev.alloc(2*size, 2);
  fftOrig.alloc(2*size, 2);
  planOrigL = FFTW_(plan_r2r_1d)(size*2, fftOrig.L, fftOrig.L, FFTW_R2HC, fftflags);
  planOrigR = FFTW_(plan_r2r_1d)(size*2, fftOrig.R, fftOrig.R, FFTW_R2HC, fftflags);
  planRevL = FFTW_(plan_r2r_1d)(size*2, fftRev.L, fftRev.L, FFTW_HC2R, fftflags);
  planRevR = FFTW_(plan_r2r_1d)(size*2, fftRev.R, fftRev.R, FFTW_HC2R, fftflags);
  fftSize = size;
}

void FV3_(irmodel)::freeFFT()
{
  if(fftSize == 0) return;
  fftRev.free();
  fftOrig.free();
  FFTW_(destroy_plan)(planRevL);
  FFTW_(destroy_plan)(planRevR);
  FFTW_(destroy_plan)(planOrigL);
  FFTW_(destroy_plan)(planOrigR);
  fftSize = 0;
}

#include "freeverb/fv3_ns_end.h"
