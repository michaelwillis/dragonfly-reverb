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

class _FV3_(irmodel3m) : public _FV3_(irbasem)
{
 public:
  _FV3_(irmodel3m)();
  virtual _FV3_(~irmodel3m)();
  virtual void loadImpulse(const _fv3_float_t * inputL, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void processreplace(_fv3_float_t *inputL, long numsamples);
  virtual void mute();

  void setFragmentSize(long size, long factor);
  long getSFragmentSize();
  long getLFragmentSize(); 
  long getSFragmentCount();
  long getLFragmentCount();
  long getScursor();
  
 protected:
  virtual void processZL(_fv3_float_t *inputL, long numsamples);
  
  void allocFrags(std::vector<_FV3_(frag)*> *to, const _fv3_float_t *inputL, long fragSize, long num, long mod, unsigned fftflags, _fv3_float_t * preAllocL)
    throw(std::bad_alloc);
  void freeFrags(std::vector<_FV3_(frag)*> *v);
  void allocSlots(long ssize, long lsize)
    throw(std::bad_alloc);
  void freeSlots();

  long Lcursor, Scursor, Lstep, sFragmentSize, lFragmentSize;
  _FV3_(slot) sReverseSlot, lReverseSlot, sIFFTSlot, lIFFTSlot, sSwapSlot, lSwapSlot, restSlot, fifoSlot, lFrameSlot, sOnlySlot, sImpulseFFTBlock, lImpulseFFTBlock;
  _fv3_float_t *sFramePointerL, *sFramePointerR;
  std::vector<_FV3_(frag)*> sFragments, lFragments;
  _FV3_(fragfft) sFragmentsFFT, lFragmentsFFT;
  _FV3_(blockDelay) sBlockDelayL, lBlockDelayL;

 private:
  _FV3_(irmodel3m)(const _FV3_(irmodel3m)& x);
  _FV3_(irmodel3m)& operator=(const _FV3_(irmodel3m)& x);
};

class _FV3_(irmodel3) : public _FV3_(irmodel1)
{
 public:
  _FV3_(irmodel3)();
  virtual _FV3_(~irmodel3)();
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  using _FV3_(irbase)::processreplace;
  virtual void processreplace(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);
  
  virtual void setFragmentSize(long size, long factor);
  
  long getSFragmentSize();
  long getLFragmentSize(); 
  long getSFragmentCount();
  long getLFragmentCount();
  void printconfig();
  
 protected:
  _FV3_(irmodel3m) *ir3mL, *ir3mR;

 private:
  _FV3_(irmodel3)(const _FV3_(irmodel3)& x);
  _FV3_(irmodel3)& operator=(const _FV3_(irmodel3)& x);
};
