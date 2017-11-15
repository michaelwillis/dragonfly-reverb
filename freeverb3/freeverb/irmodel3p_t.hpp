/**
 *  Impulse Response Processor model implementation
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

typedef struct {
  long * lFragmentSize;
  std::vector<_FV3_(frag)*> *lFragments;
  _FV3_(blockDelay) *lBlockDelayL, *lBlockDelayR;
  _fv3_float_t **lSwapL, **lSwapR;
  volatile int *flags;
  PthreadEvent *event_StartThread, *event_ThreadEnded;
  PthreadLocker *threadSection;
} _FV3_(lfThreadInfoW);

class _FV3_(irmodel3p) : public _FV3_(irmodel3)
{
 public:
  _FV3_(irmodel3p)();
  virtual _FV3_(~irmodel3p)();
  virtual void loadImpulse(_fv3_float_t * inputL, _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void resume();
  virtual void suspend();
  virtual void mute();
  virtual void setFragmentSize(long size, long factor);

 protected:
  _FV3_(irmodel3p)(const _FV3_(irmodel3p)& x);
  _FV3_(irmodel3p)& operator=(const _FV3_(irmodel3p)& x);
  virtual void processZL(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples, unsigned options);
  bool validThread;
  _FV3_(lfThreadInfoW) hostThreadData;
  unsigned int threadId;
  volatile int threadFlags;
  int threadPriority;
  PthreadLocker threadSection, mainSection;
  PthreadEvent event_StartThread, event_ThreadEnded;
  pthread_t lFragmentThreadHandle;
};
