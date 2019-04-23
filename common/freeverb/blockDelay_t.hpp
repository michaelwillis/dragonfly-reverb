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

class _FV3_(blockDelay)
{
public:
  _FV3_(blockDelay)();
  virtual _FV3_(~blockDelay)();
  void setBlock(long size, long n) throw(std::bad_alloc);
  _fv3_float_t * at(_fv3_float_t * i, long prev);
  _fv3_float_t * get(long prev);
  void push(_fv3_float_t * i);
  void mute();
  
 private:
  _FV3_(blockDelay)(const _FV3_(blockDelay)& x);
  _FV3_(blockDelay)& operator=(const _FV3_(blockDelay)& x);
  void freeF();
  _fv3_float_t * f;
  long N, blockSize, cur;
};
