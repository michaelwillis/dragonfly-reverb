/**
 *  FFT impulse fragment square multiplier
 *
 *  Copyright (C) 2006-2014 Teru Kamogashira
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

class _FV3_(fragfft)
{
 public:
  _FV3_(fragfft)();
  _FV3_(~fragfft)();
  void allocFFT(long size, unsigned fftflags) throw(std::bad_alloc);
  void freeFFT();
  long getFragmentSize();
  // replace size, size*2
  void R2HC(const _fv3_float_t * iL, const _fv3_float_t * iR, _fv3_float_t * oL, _fv3_float_t * oR);
  // add size*2, size*2
  void HC2R(const _fv3_float_t * iL, const _fv3_float_t * iR, _fv3_float_t * oL, _fv3_float_t * oR);
 private:
  _FV3_(fragfft)(const _FV3_(fragfft)& x);
  _FV3_(fragfft)& operator=(const _FV3_(fragfft)& x);
  void R2SA(const _fv3_float_t * in, _fv3_float_t * out, long n, long simd);
  void R2SA(const _fv3_float_t * in, _fv3_float_t * out, long n);
  void SA2R(const _fv3_float_t * in, _fv3_float_t * out, long n, long simd);
  void SA2R(const _fv3_float_t * in, _fv3_float_t * out, long n);
  void setSIMD(uint32_t simdFlag);
  _FFTW_(plan) planRevL, planRevR, planOrigL, planOrigR;
  long fragmentSize, simdSize;
  _FV3_(slot) fftOrig;
};

typedef void (_FV3_(*MULT_T))(const _fv3_float_t *, const _fv3_float_t *, _fv3_float_t *, long);

class _FV3_(frag)
{
 public:
  _FV3_(frag)();
  _FV3_(~frag)();
  void loadImpulse(const _fv3_float_t * L, const _fv3_float_t * R, long size, long limit, unsigned fftflags)
    throw(std::bad_alloc);
  void loadImpulse(const _fv3_float_t * L, const _fv3_float_t * R, long size, long limit, unsigned fftflags,
		   _fv3_float_t * preAllocatedL, _fv3_float_t * preAllocatedR)
    throw(std::bad_alloc);
  void unloadImpulse();
  long getFragmentSize();
  // add size*2, size*2
  void MULT(const _fv3_float_t * iL, const _fv3_float_t * iR, _fv3_float_t * oL, _fv3_float_t * oR);
  // replace size*2
  void getFFT(_fv3_float_t * oL, _fv3_float_t * oR);
  
private:
  _FV3_(frag)(const _FV3_(frag)& x);
  _FV3_(frag)& operator=(const _FV3_(frag)& x);  
  void setSIMD(uint32_t simdFlag);
  _FV3_(MULT_T) MULT_M;
  void allocImpulse(long size) throw(std::bad_alloc);
  void registerPreallocatedBlock(_fv3_float_t * _L, _fv3_float_t * _R, long size);
  void freeImpulse();
  long fragmentSize;
  _FV3_(slot) fftImpulse;
};
