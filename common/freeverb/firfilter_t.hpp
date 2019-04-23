/**
 *  FIR filter
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

class _FV3_(firfilter)
{
 public:
  static void lpf(_fv3_float_t h[], const long N, const long WINDOW, const _fv3_float_t fc, const _fv3_float_t param);
  static void hpf(_fv3_float_t h[], const long N, const long WINDOW, const _fv3_float_t fc, const _fv3_float_t param);
  static void bef(_fv3_float_t h[], const long N, const long WINDOW, const _fv3_float_t fc1, const _fv3_float_t fc2, const _fv3_float_t param);
  static void bpf(_fv3_float_t h[], const long N, const long WINDOW, const _fv3_float_t fc1, const _fv3_float_t fc2, const _fv3_float_t param);
  static long predictFilterLength(long w, _fv3_float_t fc);
};
