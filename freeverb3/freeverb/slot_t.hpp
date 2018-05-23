/**
 *  Simple Slot
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

class _FV3_(slot)
{
 public:
  _FV3_(slot)();
  virtual _FV3_(~slot)();
  void alloc(long nsize, long nch)
    throw(std::bad_alloc);
  _fv3_float_t * c(long nch);
  void free();
  void mute();
  void mute(long limit);
  void mute(long offset, long limit);
  long getsize(){ return size; }
  long getch(){ return ch; }
  _fv3_float_t ** getArray();
  _fv3_float_t *L, *R;

 private:
  _FV3_(slot)(const _FV3_(slot)& x);
  _FV3_(slot)& operator=(const _FV3_(slot)& x);
  long size, ch;
  _fv3_float_t ** data;
};
