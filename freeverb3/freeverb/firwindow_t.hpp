/**
 *  FIR window
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

class _FV3_(firwindow)
{
 public:
  static void Square(_fv3_float_t w[], const long N);
  static void Hamming(_fv3_float_t w[], const long N);
  static void Hanning(_fv3_float_t w[], const long N);
  static void Blackman(_fv3_float_t w[], const long N);
  static _fv3_float_t i_zero(const _fv3_float_t x);
  static void Kaiser(_fv3_float_t w[], const long N, const _fv3_float_t beta);
  static _fv3_float_t KaiserBeta(const _fv3_float_t dB);
  static void CosROW(_fv3_float_t w[], const long N, const _fv3_float_t fc, const _fv3_float_t alpha);
  static void getWindow(const long WINDOW, _fv3_float_t w[], const long N, const _fv3_float_t fc, const _fv3_float_t param);
  static void Sinc(_fv3_float_t sinc[], const long N, const _fv3_float_t fc);
};
