/**
 *  Block Delay
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

#include "freeverb/blockDelay.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(blockDelay)::FV3_(blockDelay)()
{
  N = blockSize = cur = 0;
}

FV3_(blockDelay)::FV3_(~blockDelay)()
{
  freeF();
}

void FV3_(blockDelay)::freeF()
{
  if(N != 0) FV3_(utils)::aligned_free(f);
  N = 0;
}

void FV3_(blockDelay)::mute()
{
  FV3_(utils)::mute(f, N*blockSize);
}

void FV3_(blockDelay)::setBlock(long size, long n)
		      throw(std::bad_alloc)
{
  freeF();
  if(size < 0||n < 0) size = n = 0;
  f = (fv3_float_t*)FV3_(utils)::aligned_malloc(sizeof(fv3_float_t)*n*size, FV3_PTR_ALIGN_BYTE);
  if(f == NULL)
    {
      std::fprintf(stderr, "FV3_(blockDelay)::setBlock(%ld,%ld) !alloc\n", size, n);
      throw std::bad_alloc();
    }
  N = n;
  blockSize = size;
  cur = 0;
  FV3_(utils)::mute(f, n*size);
}

fv3_float_t * FV3_(blockDelay)::at(fv3_float_t * i, long prev)
{
  if(blockSize == 0) return NULL;
  if(prev == 0)
    {
      cur = (cur + 1) % N;
      std::memcpy(f + blockSize * cur, i, sizeof(fv3_float_t) * blockSize);
      return f + blockSize * cur;
    }
  else
    {
      return f + blockSize * ((N + cur - prev) % N);
    }
}

fv3_float_t * FV3_(blockDelay)::get(long prev)
{
  if(blockSize == 0) return NULL;
  return f + blockSize * ((N + cur - prev) % N);
}

void FV3_(blockDelay)::push(fv3_float_t * i)
{
  cur = (cur + 1) % N;
  std::memcpy(f + blockSize * cur, i, sizeof(fv3_float_t) * blockSize);
}

#include "freeverb/fv3_ns_end.h"
