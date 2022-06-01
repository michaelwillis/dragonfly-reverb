/**
 *  DelayLine Processor Base Class
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

#include "freeverb/delayline.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(delayline)::FV3_(delayline)()
		throw(std::bad_alloc)
{
  currentfs = FV3_REVBASE_DEFAULT_FS;
  bufsize = baseidx = 0, buffer = NULL;
}

FV3_(delayline)::~FV3_(delayline)()
{
  free();
}

void FV3_(delayline)::setSampleRate(fv3_float_t fs)
		     throw(std::bad_alloc)
{
  currentfs = fs;
}

fv3_float_t FV3_(delayline)::getSampleRate()
{
  return currentfs;
}

long FV3_(delayline)::getsize()
{
  return bufsize;
}

void FV3_(delayline)::setsize(long size)
		 throw(std::bad_alloc)
{
  if(size <= 0) return;
  fv3_float_t * new_buffer = NULL;
  try
    {
      new_buffer = new fv3_float_t[size];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "delayline::setsize(%ld) bad_alloc\n", size);
      delete[] new_buffer;
      throw;
    }
  FV3_(utils)::mute(new_buffer, size);

  if(bufsize > 0&&bufsize <= size)
    {
      for(long i = 0;i < bufsize;i++) new_buffer[size-bufsize+i] = at(i);
    }
  if(bufsize > 0&&bufsize > size)
    {
      //long cut = bufsize - size;
      for(long i = 0;i < size;i++) new_buffer[i] = at(i);
    }

  this->free();
  bufsize = size;
  buffer = new_buffer;
}

void FV3_(delayline)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; baseidx = bufsize = 0;
}

void FV3_(delayline)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
}

fv3_float_t FV3_(delayline)::process(fv3_float_t input)
{
  // simple delay line example
  baseidx --; if(baseidx < 0) baseidx += bufsize;
  fv3_float_t lastOut = (*this)[0];
  (*this)[0] = input;
  return lastOut;
}

long FV3_(delayline)::p_(fv3_float_t ms)
{
  long base = static_cast<long>(currentfs*ms*0.001);
  if(primeMode){while(!FV3_(utils)::isPrime(base)) base++;}
  return base;
}

#include "freeverb/fv3_ns_end.h"
