/**
 *  Impulse Response Processor model implementation
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

class _FV3_(irmodel1m) : public _FV3_(irbasem)
{
 public:
  _FV3_(irmodel1m)();
  virtual _FV3_(~irmodel1m)();
  virtual void loadImpulse(const _fv3_float_t * inputL, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void processreplace(_fv3_float_t *inputL, long numsamples);
  virtual void mute();
  
  long getFragmentSize();
  
 private:
  _FV3_(irmodel1m)(const _FV3_(irmodel1m)& x);
  _FV3_(irmodel1m)& operator=(const _FV3_(irmodel1m)& x);
  void processSquareReplace(_fv3_float_t *inputL);
  long fragmentSize, current, fifopt;
  _FFTW_(plan) planRevrL, planOrigL;
  _FV3_(slot) fftRevr, fftImpl, delayline, fifo;
};

class _FV3_(irmodel1) : public _FV3_(irbase)
{
 public:
  _FV3_(irmodel1)();
  virtual _FV3_(~irmodel1)();  
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  using _FV3_(irbase)::processreplace;
  virtual void processreplace(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);
  virtual void processreplaceS(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);
  virtual void mute();
  
  virtual long getFragmentSize();

 protected:
  long fragmentSize;
  _FV3_(slot) inputW, inputD;

 private:
  _FV3_(irmodel1)(const _FV3_(irmodel1)& x);
  _FV3_(irmodel1)& operator=(const _FV3_(irmodel1)& x);
};
