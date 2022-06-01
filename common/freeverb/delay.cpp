/**
 *  Simple Delay
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

#include "freeverb/delay.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// simple delay

FV3_(delay)::FV3_(delay)()
{
  feedback = 1.; bufsize = bufidx = 0; buffer = NULL;
}

FV3_(delay)::~FV3_(delay)()
{
  free();
}

long FV3_(delay)::getsize()
{
  return bufsize;
}

void FV3_(delay)::setsize(long size)
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
      std::fprintf(stderr, "delay::setsize(%ld) bad_alloc\n", size);
      delete[] new_buffer;
      throw;
    }
  FV3_(utils)::mute(new_buffer, size);
  
  if(bufsize > 0&&bufsize <= size)
    {
      long prefix_i = size-bufsize;
      for(long i = 0;i < bufsize;i++) new_buffer[prefix_i + i] = this->process(0);
    }
  if(bufsize > 0&&bufsize > size)
    {
      long cut = bufsize - size;
      for(long i = 0;i < cut;i ++)  this->process(0);
      for(long i = 0;i < size;i ++) new_buffer[i] = this->process(0);
    }

  this->free();
  bufidx = 0;
  bufsize = size;
  buffer = new_buffer;
}

void FV3_(delay)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; bufidx = bufsize = 0;
}

void FV3_(delay)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  bufidx = 0;
}

void FV3_(delay)::setfeedback(fv3_float_t val) 
{
  feedback = val;
}

fv3_float_t FV3_(delay)::getfeedback() 
{
  return feedback;
}

// modulated delay

FV3_(delaym)::FV3_(delaym)()
{
  bufsize = readidx = writeidx = modulationsize = 0;
  feedback = 1.;
  z_1 = modulationsize_f = 0;
  buffer = NULL;
}

FV3_(delaym)::~FV3_(delaym)()
{
  if(bufsize != 0) delete[] buffer;
}

long FV3_(delaym)::getsize()
{
  return bufsize;
}

long FV3_(delaym)::getdelaysize()
{
  return (bufsize - modulationsize);
}

long FV3_(delaym)::getmodulationsize()
{
  return modulationsize;
}

void FV3_(delaym)::setsize(long size)
		 throw(std::bad_alloc)
{
  setsize(size, 0);
}

void FV3_(delaym)::setsize(long size, long modsize)
		 throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "delaym::setsize(%ld,%ld)\n", size, modsize);
#endif
  if(size <= 0) return;
  if(modsize < 0) modsize = 0;
  if(modsize > size) modsize = size;
  long newsize = size + modsize;
  fv3_float_t * new_buffer = NULL;
  try
    {
      new_buffer = new fv3_float_t[newsize];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "delaym::setsize(%ld) bad_alloc\n", newsize);
      delete[] new_buffer;
      throw;
    }
  FV3_(utils)::mute(new_buffer, newsize);
  
  this->free();
  bufsize = newsize;
  readidx = modsize*2;
  writeidx = 0;
  modulationsize = modsize;
  modulationsize_f = (fv3_float_t)modulationsize;
  buffer = new_buffer;
  z_1 = 0;
}

void FV3_(delaym)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; writeidx = bufsize = 0; z_1 = 0;
}

void FV3_(delaym)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  writeidx = 0; z_1 = 0; readidx = modulationsize*2;
}

void FV3_(delaym)::setfeedback(fv3_float_t val) 
{
  feedback = val;
}

fv3_float_t FV3_(delaym)::getfeedback() 
{
  return feedback;
}

#include "freeverb/fv3_ns_end.h"
