/**
 *  RMS
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

#include "freeverb/rms.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(rms)::FV3_(rms)()
{
  sum = bufs = 0;
  bufsize = bufidx = 0;
  buffer = NULL;
}

FV3_(rms)::~FV3_(rms)()
{
  this->free();
}

long FV3_(rms)::getsize()
{
  return bufsize;
}

void FV3_(rms)::setsize(long size)
	       throw(std::bad_alloc)
{
  if(size <= 0) return;
  this->free();
  try
    {
      buffer = new fv3_float_t[size];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "rms::setsize(%ld) bad_alloc\n", size);
      delete[] buffer;
      throw;
    }
  bufs = bufsize = size;
  mute();
}

void FV3_(rms)::free()
{
  if(buffer != NULL&&bufsize != 0) delete[] buffer;
  buffer = NULL; bufidx = bufsize = 0;
}

void FV3_(rms)::mute()
{
  if(buffer == NULL||bufsize == 0) return;
  FV3_(utils)::mute(buffer, bufsize);
  sum = 0; bufidx = 0;
}

#include "freeverb/fv3_ns_end.h"
