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

#include "freeverb/irmodels.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(irmodels)::FV3_(irmodels)()
{
  current = impulseSize;
}

FV3_(irmodels)::FV3_(~irmodels)()
{
  freeImpulse();
}

void FV3_(irmodels)::unloadImpulse()
{
  freeImpulse();
}

void FV3_(irmodels)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
		    throw(std::bad_alloc)
{
  impulseSize = size;
  allocImpulse(size);
  std::memcpy(impulse.L, inputL, sizeof(fv3_float_t)*size);
  std::memcpy(impulse.R, inputR, sizeof(fv3_float_t)*size);
  mute();
}

void FV3_(irmodels)::processreplace(const fv3_float_t *inputL, const fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  if(numsamples <= 0||impulseSize <= 0) return;
  for(long i = 0;i < numsamples;i ++)
    {
      fv3_float_t * offsetL = delay.L + current;
      fv3_float_t * offsetR = delay.R + current;
      fv3_float_t L = 0, R = 0;
      offsetL[0] = inputL[i]; offsetR[0] = inputR[i];
      
      for(long j = 0;j < impulseSize;j ++) L += impulse.L[j]*offsetL[j];
      for(long j = 0;j < impulseSize;j ++) R += impulse.R[j]*offsetR[j];
      
      if(current == 0)
	{
	  current = impulseSize;
	  std::memmove(delay.L+impulseSize+1, delay.L, sizeof(fv3_float_t)*(impulseSize-1));
	  std::memmove(delay.R+impulseSize+1, delay.R, sizeof(fv3_float_t)*(impulseSize-1));
	}
      else
	current --;
      
      if((processoptions & FV3_IR_SKIP_FILTER) == 0)
	{
	  L = filter.processL(L);
	  R = filter.processR(R);
	}
      
      outputL[i] = outputR[i] = 0;
      if((processoptions & FV3_IR_MUTE_DRY) == 0)
	{
	  outputL[i] += inputL[i]*dry;
	  outputR[i] += inputR[i]*dry;
	}
      if((processoptions & FV3_IR_MUTE_WET) == 0)
	{
	  outputL[i] += L*wet1L+R*wet2L;
	  outputR[i] += R*wet1R+L*wet2R;
	}
    }
}

void FV3_(irmodels)::mute()
{
  if(impulseSize == 0) return;
  delay.mute();
  current = impulseSize;
}

void FV3_(irmodels)::allocImpulse(long size)
		    throw(std::bad_alloc)
{
  freeImpulse();
  impulseSize = size;
  try
    {
      impulse.alloc(size, 2);
      delay.alloc(size*2, 2);
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "irmodels::allocImpulse(%ld) bad_alloc\n", size);
      freeImpulse();
      throw std::bad_alloc();
    }
}

void FV3_(irmodels)::freeImpulse()
{
  if(impulseSize == 0) return;
  impulse.free();
  delay.free();
  impulseSize = 0;
}

#include "freeverb/fv3_ns_end.h"
