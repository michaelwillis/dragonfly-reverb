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

class _FV3_(rms)
{
 public:
  _FV3_(rms)();
  _FV3_(~rms)();
  void free();

  void setsize(long size) throw(std::bad_alloc);
  long getsize();
  void mute();
  inline _fv3_float_t process(_fv3_float_t input)
  {
    if(bufsize == 0) return std::fabs(input);
    if(bufidx == bufsize-1)
      bufidx = 0;
    else
      bufidx ++;
    sum -= buffer[bufidx];
    buffer[bufidx] = input*input;
    sum += buffer[bufidx];
    if(sum < 0) sum = 0;
    UNDENORMAL(input);
    _fv3_float_t ret = std::sqrt(sum/bufs);
    return ret;
  }
  inline _fv3_float_t operator()(_fv3_float_t input){return this->process(input);}

 private:
  _FV3_(rms)(const _FV3_(rms)& x);
  _FV3_(rms)& operator=(const _FV3_(rms)& x); 
  _fv3_float_t *buffer, sum, bufs;
  long bufsize, bufidx;
};
