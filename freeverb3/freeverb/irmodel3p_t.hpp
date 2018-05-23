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

typedef struct {
  long * lFragmentSize;
  std::vector<_FV3_(frag)*> *lFragments;
  _FV3_(blockDelay) *lBlockDelayL;
  _fv3_float_t **lSwapL;
  volatile int *flags;
  PthreadEvent *event_StartThread, *event_ThreadEnded;
  PthreadLocker *threadSection;
} _FV3_(lfThreadInfoW);

class _FV3_(irmodel3pm) : public _FV3_(irmodel3m)
{
 public:
  _FV3_(irmodel3pm)();
  virtual _FV3_(~irmodel3pm)();
  virtual void loadImpulse(const _fv3_float_t * inputL, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void resume();
  virtual void suspend();
  virtual void mute();
  virtual void setFragmentSize(long size, long factor);
  
 protected:
  virtual void processZL(_fv3_float_t *inputL, long numsamples);

  bool validThread;
  _FV3_(lfThreadInfoW) hostThreadData;
  volatile int threadFlags;
  PthreadLocker threadSection, mainSection;
  PthreadEvent event_StartThread, event_ThreadEnded;
  pthread_t lFragmentThreadHandle;

 private:
  _FV3_(irmodel3pm)(const _FV3_(irmodel3pm)& x);
  _FV3_(irmodel3pm)& operator=(const _FV3_(irmodel3pm)& x);
};

class _FV3_(irmodel3p) : public _FV3_(irmodel3)
{
 public:
  _FV3_(irmodel3p)();
  virtual _FV3_(~irmodel3p)();
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void resume();
  virtual void suspend();
  virtual void mute();
  virtual void setFragmentSize(long size, long factor);
  virtual void setInitialDelay(long numsamples)
    throw(std::bad_alloc);
  
 protected:
  PthreadLocker mainSection;

 private:
  _FV3_(irmodel3p)(const _FV3_(irmodel3)& x);
  _FV3_(irmodel3p)& operator=(const _FV3_(irmodel3)& x);
};
