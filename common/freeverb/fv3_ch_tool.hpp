/**
 *  The channel splitter/merger tool
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

#ifndef _fv3_ch_tool_hpp
#define _fv3_ch_tool_hpp

#include <cstdarg>

namespace fv3
{
  template <class T> void splitChannelsV(const int ch, const long size, const T *in, ...)
  {
    va_list ap;
    va_start(ap, in);
    for(int i = 0;i < ch;i ++)
      {
	T * out = va_arg(ap, T*);
	for(long t = 0;t < size;t ++)
	  out[t] = in[t*ch+i];
      }
    va_end(ap);
  }

  template <class T> void splitChannelsS(const int ch, const long size, const T *in, T **out)
  {
    for(int i = 0;i < ch;i ++)
      {
	for(long t = 0;t < size;t ++)
	  out[i][t] = in[t*ch+i];
      }
  }
  
  template <class T> void mergeChannelsV(const int ch, const long size, T *out, ...)
  {
    va_list ap;
    va_start(ap, out);
    for(int i = 0;i < ch;i ++)
      {
	T * in = va_arg(ap, T*);
	for(long t = 0;t < size;t ++)
	  out[t*ch+i] = in[t];
      }
    va_end(ap);
  }
  
  template <class T> void mergeChannelsS(const int ch, const long size, T *out, const T **in)
  {
    for(int i = 0;i < ch;i ++)
      {
	for(long t = 0;t < size;t ++)
	  out[t*ch+i] = in[i][t];
      }
  }
};

#endif
