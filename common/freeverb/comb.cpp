/**
 *  Comb (delay) filter with LPF
 *
 *  Copyright (C) 2000 Jezar at Dreampoint
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

#include "freeverb/comb.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// simple comb filter

FV3_(comb)::FV3_(comb)()
{
  bufsize = bufidx = 0; buffer = NULL; setdamp(0);
  feedback = filterstore = 0;
}

FV3_(comb)::FV3_(~comb)()
{
  this->free();
}

long FV3_(comb)::getsize()
{
  return bufsize;
}

void FV3_(comb)::setsize(long size)
                throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "comb::setsize(%ld)\n", size);
#endif
  if(size <= 0) return;
  fv3_float_t * new_buffer = NULL;
  try
    {
      new_buffer = new fv3_float_t[size];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "comb::setsize(%ld) bad_alloc\n", size);
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
  filterstore = 0;
}

void FV3_(comb)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; bufidx = bufsize = 0; filterstore = 0;
}

void FV3_(comb)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  filterstore = 0; bufidx = 0;
}

void FV3_(comb)::setdamp(fv3_float_t val) 
{
  damp1 = val; damp2 = 1-val;
}

fv3_float_t FV3_(comb)::getdamp() 
{
  return damp1;
}

fv3_float_t FV3_(comb)::getfeedback() 
{
  return feedback;
}

// modulated comb filter

FV3_(combm)::FV3_(combm)()
{
  bufsize = readidx = writeidx = delaysize = modulationsize = 0;
  buffer = NULL; setdamp(0);
  feedback = 1; z_1 = filterstore = modulationsize_f = 0;
}

FV3_(combm)::FV3_(~combm)()
{
  this->free();
}

long FV3_(combm)::getsize()
{
  return bufsize;
}

long FV3_(combm)::getdelaysize()
{
  return delaysize;
}

long FV3_(combm)::getmodulationsize()
{
  return modulationsize;
}

void FV3_(combm)::setsize(long size)
		throw(std::bad_alloc)
{
  setsize(size, 0);
}

void FV3_(combm)::setsize(long size, long modsize)
		throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "comb::setsize(%ld,%ld)\n", size, modsize);
#endif
 if(size <= 0) return;
 if(modsize < 0) modsize = 0;
  if(modsize > size) modsize = size;
  long newsize = size+modsize;
  fv3_float_t * new_buffer = NULL;
  try
    {
      new_buffer = new fv3_float_t[newsize];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "combm::setsize(%ld) bad_alloc\n", newsize);
      delete[] new_buffer;
      throw;
    }
  FV3_(utils)::mute(new_buffer, newsize);

  this->free();
  bufsize = newsize;
  readidx = modsize*2;
  delaysize = size;
  modulationsize = modsize;
  modulationsize = (fv3_float_t)modulationsize;
  buffer = new_buffer;
  writeidx = 0;
  z_1 = 0;
}

void FV3_(combm)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; writeidx = bufsize = 0; z_1 = filterstore = 0;
}

void FV3_(combm)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  writeidx = 0; filterstore = z_1 = 0; readidx = modulationsize*2;
}

void FV3_(combm)::setdamp(fv3_float_t val) 
{
  damp1 = val; damp2 = 1-val;
}

fv3_float_t FV3_(combm)::getdamp() 
{
  return damp1;
}

void FV3_(combm)::setfeedback(fv3_float_t val) 
{
  feedback = val;
}

fv3_float_t FV3_(combm)::getfeedback() 
{
  return feedback;
}

#include "freeverb/fv3_ns_end.h"
