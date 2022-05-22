/**
 *  Allpass filter
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

#include "freeverb/allpass.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// simple allpass filter

FV3_(allpass)::FV3_(allpass)()
{
  bufsize = bufidx = 0; decay = 1; buffer = NULL;
}

FV3_(allpass)::FV3_(~allpass)()
{
  free();
}

long FV3_(allpass)::getsize()
{
  return bufsize;
}

void FV3_(allpass)::setsize(long size)
                   throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "allpass::setsize(%ld)\n", size);
#endif
  if(size <= 0) return;
  fv3_float_t * new_buffer = NULL;
  try
    {
      new_buffer = new fv3_float_t[size];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "allpass::setsize(%ld) bad_alloc\n", size);
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

  free();
  bufidx = 0;
  bufsize = size;
  buffer = new_buffer;
}

void FV3_(allpass)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; bufidx = bufsize = 0;
}

void FV3_(allpass)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  bufidx = 0;
}

void FV3_(allpass)::setfeedback(fv3_float_t val) 
{
  feedback = val;
}

fv3_float_t FV3_(allpass)::getfeedback() 
{
  return feedback;
}

void FV3_(allpass)::setdecay(fv3_float_t val) 
{
  decay = val;
}

fv3_float_t FV3_(allpass)::getdecay()
{
  return decay;
}

// modulated allpass filter

FV3_(allpassm)::FV3_(allpassm)()
{
  bufsize = readidx = writeidx = modulationsize = 0;
  feedback = feedback_mod = z_1 = modulationsize_f = 0;
  buffer = NULL; decay = 1;
}

FV3_(allpassm)::FV3_(~allpassm)()
{
  free();
}

long FV3_(allpassm)::getsize()
{
  return bufsize;
}

long FV3_(allpassm)::getdelaysize()
{
  return (bufsize - modulationsize);
}

long FV3_(allpassm)::getmodulationsize()
{
  return modulationsize;
}

void FV3_(allpassm)::setsize(long size)
		   throw(std::bad_alloc)
{
  setsize(size, 0);
}

void FV3_(allpassm)::setsize(long size, long modsize)
		   throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "allpassm::setsize(%ld,%ld)\n", size, modsize);
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
      std::fprintf(stderr, "allpassm::setsize(%ld) bad_alloc\n", newsize);
      delete[] new_buffer;
      throw;
    }
  FV3_(utils)::mute(new_buffer, newsize);
  
  this->free();
  bufsize = newsize;
  readidx = modsize * 2;
  writeidx = 0;
  modulationsize = modsize;
  modulationsize_f = (fv3_float_t)modulationsize;
  buffer = new_buffer;
  z_1 = 0;
}

void FV3_(allpassm)::free()
{
  if(buffer == NULL||bufsize == 0) return;
  delete[] buffer;
  buffer = NULL; writeidx = bufsize = 0; z_1 = 0;
}

void FV3_(allpassm)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  writeidx = 0; z_1 = 0; readidx = modulationsize * 2; feedback_mod = feedback;
}

void FV3_(allpassm)::setfeedback(fv3_float_t val) 
{
  feedback_mod = feedback = val;
}

fv3_float_t FV3_(allpassm)::getfeedback() 
{
  return feedback;
}

void FV3_(allpassm)::setdecay(fv3_float_t val) 
{
  decay = val;
}

fv3_float_t FV3_(allpassm)::getdecay()
{
  return decay;
}

void FV3_(allpassm)::set_90degfq(fv3_float_t fc, fv3_float_t fs)
{
  fv3_float_t tant = std::tan(M_PI*fc/fs);
  feedback = (tant-1)/(tant+1);
}

// 2nd order allpass filter

FV3_(allpass2)::FV3_(allpass2)()
{
  bufsize1 = bufidx1 = bufsize2 = bufidx2 = 0; decay1 = decay2 = 1;
  feedback1 = feedback2 = 0;
  buffer1 = buffer2 = NULL;
}

FV3_(allpass2)::FV3_(~allpass2)()
{
  this->free();
}

void FV3_(allpass2)::setsize(long size1, long size2)
                   throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "allpass2::setsize(%ld,%ld)\n", size1, size2);
#endif
  if(size1 <= 0||size2 <= 0) return;
  free();
  try
    {
      buffer1 = new fv3_float_t[size1];
      buffer2 = new fv3_float_t[size2];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "allpass2::setsize(%ld,%ld) bad_alloc\n", size1, size2);
      delete[] buffer1;
      delete[] buffer2;
      throw;
   }
  bufsize1 = size1;
  bufsize2 = size2;
  mute();
}

void FV3_(allpass2)::free()
{
  if(buffer1 == NULL||bufsize1 == 0||buffer2 == NULL||bufsize2 == 0) return;
  delete[] buffer1; delete[] buffer2;
  buffer1 = buffer2 = NULL; bufidx1 = bufidx2 = bufsize1 = bufsize2 = 0;
}

void FV3_(allpass2)::mute()
{
  if(buffer1 == NULL||bufsize1 == 0||buffer2 == NULL||bufsize2 == 0) return;
  FV3_(utils)::mute(buffer1, bufsize1);
  FV3_(utils)::mute(buffer2, bufsize2);
}

void FV3_(allpass2)::setfeedback1(fv3_float_t val) 
{
  feedback1 = val;
}

void FV3_(allpass2)::setfeedback2(fv3_float_t val) 
{
  feedback2 = val;
}

void FV3_(allpass2)::setdecay1(fv3_float_t val) 
{
  decay1 = val;
}

void FV3_(allpass2)::setdecay2(fv3_float_t val) 
{
  decay2 = val;
}

// 3rd order allpass filter

FV3_(allpass3)::FV3_(allpass3)()
{
  bufsize1 = readidx1 = writeidx1 = bufsize2 = bufidx2 = bufsize3 = bufidx3 = modulationsize = 0;
  decay1 = decay2 = decay3 = 1;
  buffer1 = buffer2 = buffer3 = NULL;
  feedback1 = feedback2 = feedback3 = modulationsize_f = 0;
}

FV3_(allpass3)::FV3_(~allpass3)()
{
  this->free();
}

void FV3_(allpass3)::setsize(long size1, long size2, long size3)
                   throw(std::bad_alloc)
{
  setsize(size1, 0, size2, size3);
}

void FV3_(allpass3)::setsize(long size1, long size1mod, long size2, long size3)
                   throw(std::bad_alloc)
{
#ifdef DEBUG
  std::fprintf(stderr, "allpass3::setsize(%ld,%ld,%ld,%ld)\n", size1, size1mod, size2, size3);
#endif
  if(size1 <= 0||size2 <= 0||size3 <= 0) return;
  if(size1mod < 0) size1mod = 0;
  if(size1mod > size1) size1mod = size1;
  this->free();
  try
    {
      buffer1 = new fv3_float_t[size1+size1mod];
      buffer2 = new fv3_float_t[size2];
      buffer3 = new fv3_float_t[size3];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "allpass3::setsize(%ld,%ld,%ld) bad_alloc\n", size1, size2, size3);
      delete[] buffer1;
      delete[] buffer2;
      delete[] buffer3;
      throw;
    }
  bufsize1 = size1+size1mod;
  readidx1 = size1mod*2;
  writeidx1 = 0;
  modulationsize = size1mod;
  modulationsize_f = (fv3_float_t)modulationsize;
  bufsize2 = size2;
  bufsize3 = size3;
  mute();
}

void FV3_(allpass3)::free()
{
  if(buffer1 == NULL||bufsize1 == 0||buffer2 == NULL||bufsize2 == 0||buffer3 == NULL||bufsize3 == 0) return;
  delete[] buffer1; delete[] buffer2; delete[] buffer3;
  buffer1 = buffer2 = buffer3 = NULL;
  readidx1 = writeidx1 = bufidx2 = bufidx3 = bufsize1 = bufsize2 = bufsize3 = 0;
}

void FV3_(allpass3)::mute()
{
  if(buffer1 == NULL||bufsize1 == 0||buffer2 == NULL||bufsize2 == 0||buffer3 == NULL||bufsize3 == 0) return;
  FV3_(utils)::mute(buffer1, bufsize1);
  FV3_(utils)::mute(buffer2, bufsize2);
  FV3_(utils)::mute(buffer3, bufsize3);
  writeidx1 = 0; readidx1 = modulationsize * 2;
}

void FV3_(allpass3)::setfeedback1(fv3_float_t val) 
{
  feedback1 = val;
}

void FV3_(allpass3)::setfeedback2(fv3_float_t val) 
{
  feedback2 = val;
}

void FV3_(allpass3)::setfeedback3(fv3_float_t val) 
{
  feedback3 = val;
}

void FV3_(allpass3)::setdecay1(fv3_float_t val) 
{
  decay1 = val;
}

void FV3_(allpass3)::setdecay2(fv3_float_t val) 
{
  decay2 = val;
}

void FV3_(allpass3)::setdecay3(fv3_float_t val) 
{
  decay3 = val;
}

#include "freeverb/fv3_ns_end.h"
