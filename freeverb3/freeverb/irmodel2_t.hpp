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

class _FV3_(irmodel2m) : public _FV3_(irbasem)
{
 public:
  _FV3_(irmodel2m)();
  virtual _FV3_(~irmodel2m)();
  virtual void loadImpulse(const _fv3_float_t * inputL, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void processreplace(_fv3_float_t *inputL, long numsamples);
  virtual void mute();
  
  long getFragmentSize();
  void setFragmentSize(long size);

 protected:
  long fragmentSize;
  std::vector<_FV3_(frag)*> fragments;
  _FV3_(fragfft) fragFFT;
  _FV3_(blockDelay) blkdelayDL, blkdelayDR;
  long fifoSize;
  _FV3_(slot) fifoSlot, reverseSlot, ifftSlot, swapSlot, restSlot;

 private:
  _FV3_(irmodel2m)(const _FV3_(irmodel2m)& x);
  _FV3_(irmodel2m)& operator=(const _FV3_(irmodel2m)& x);
};

class _FV3_(irmodel2) : public _FV3_(irmodel1)
{
 public:
  _FV3_(irmodel2)();
  virtual _FV3_(~irmodel2)();
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  using _FV3_(irbase)::processreplace;
  virtual void processreplace(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);

  virtual void setFragmentSize(long size);

 protected:
    _FV3_(irmodel2m) *ir2mL, *ir2mR;

 private:
  _FV3_(irmodel2)(const _FV3_(irmodel2)& x);
  _FV3_(irmodel2)& operator=(const _FV3_(irmodel2)& x);
};
