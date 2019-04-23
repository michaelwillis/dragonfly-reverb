/**
 *  Impulse Response Processor model abstract class
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


class _FV3_(irbasem)
{
 public:
  _FV3_(irbasem)();
  virtual _FV3_(~irbasem)();
  virtual void loadImpulse(const _fv3_float_t * inputL, long size)
    throw(std::bad_alloc) = 0;
  virtual void unloadImpulse();
  virtual unsigned setFFTFlags(unsigned flags);
  virtual unsigned getFFTFlags();
  virtual void     setSIMD(uint32_t flag1, uint32_t flag2);
  virtual uint32_t getSIMD(uint32_t select);
  virtual long getImpulseSize();
  virtual long getLatency();
  virtual void resume();
  virtual void suspend();
  virtual void mute() = 0;
  virtual void processreplace(_fv3_float_t *inputL, long numsamples) = 0;
  
 protected:
  long impulseSize, latency;
  unsigned fftflags;
  uint32_t simdFlag1, simdFlag2;

 private:
  _FV3_(irbasem)(const _FV3_(irbasem)& x);
  _FV3_(irbasem)& operator=(const _FV3_(irbasem)& x);
};

class _FV3_(irbase)
{
 public:
  _FV3_(irbase)();
  virtual _FV3_(~irbase)();
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc) = 0;
  virtual void unloadImpulse();
  virtual void setprocessoptions(unsigned options);
  virtual unsigned getprocessoptions();
  virtual unsigned setFFTFlags(unsigned flags);
  virtual unsigned getFFTFlags();
  virtual void     setSIMD(uint32_t flag1, uint32_t flag2);
  virtual uint32_t getSIMD(uint32_t select);
  virtual long getImpulseSize();
  virtual long getLatency();
  virtual void setInitialDelay(long numsamples)
    throw(std::bad_alloc);
  virtual long getInitialDelay();
  virtual void resume();
  virtual void suspend();
  virtual void mute();
  virtual void processreplace(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples) = 0;
  virtual void processreplace(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples, unsigned options);
  virtual void processdrywetout(const _fv3_float_t *dL, const _fv3_float_t *dR, _fv3_float_t *wL, _fv3_float_t *wR, _fv3_float_t *oL, _fv3_float_t *oR, long numsamples);
  virtual void setwet(_fv3_float_t db);
  virtual _fv3_float_t getwet();
  virtual void setwetr(_fv3_float_t value);
  virtual _fv3_float_t getwetr();
  virtual void setdry(_fv3_float_t db);
  virtual _fv3_float_t getdry();
  virtual void setdryr(_fv3_float_t value);
  virtual _fv3_float_t getdryr();
  virtual void setwidth(_fv3_float_t value);
  virtual _fv3_float_t getwidth();
  virtual void setLPF(_fv3_float_t value);
  virtual _fv3_float_t getLPF();
  virtual void setHPF(_fv3_float_t value);
  virtual _fv3_float_t getHPF();
  virtual void setLRBalance(_fv3_float_t value);
  virtual _fv3_float_t getLRBalance();
  
 protected:
  void update();
  _fv3_float_t wet, wetdB, dry, drydB, width, lrbalance, wet1, wet2, wet1L, wet2L, wet1R, wet2R;
  _FV3_(delay) delayDL, delayDR, delayWL, delayWR;
  _FV3_(efilter) filter;
  long impulseSize, initialDelay, latency;
  unsigned fftflags, processoptions;
  uint32_t simdFlag1, simdFlag2;
  _FV3_(irbasem) *irmL, *irmR;
  
 private:
  _FV3_(irbase)(const _FV3_(irbase)& x);
  _FV3_(irbase)& operator=(const _FV3_(irbase)& x);
};
