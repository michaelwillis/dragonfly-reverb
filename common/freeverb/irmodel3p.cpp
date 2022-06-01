/**
 *  Impulse Response Processor model implementation
 *  Low Latency Version / Pthread Multithread Version
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

#include "freeverb/irmodel3p.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

static void * irmodel3p_lfthread(void *vdParam)
{
  FV3_(lfThreadInfoW) *info = (FV3_(lfThreadInfoW)*)vdParam;
  while(1)
    {
      sleep(0);
      info->event_StartThread->wait();
      info->event_StartThread->reset();
      if(*info->flags & FV3_IR3P_THREAD_FLAG_RUN)
        {
          info->threadSection->lock();
          if(info->lFragments->size() > 0)
            {
              for(long i = 0;i < (long)info->lFragments->size()-1;i ++)
                {
                  if((long)info->lFragments->size() > i+1)
                    {
                      info->lFragments->at(i+1)->MULT(info->lBlockDelayL->get(i), *info->lSwapL);
                    }
                }
            }
          *info->flags ^= FV3_IR3P_THREAD_FLAG_RUN;
          info->event_ThreadEnded->trigger();
          info->threadSection->unlock();
        }
      if(*info->flags & FV3_IR3P_THREAD_FLAG_EXIT)
        {
          *info->flags ^= FV3_IR3P_THREAD_FLAG_EXIT;
          break;
        }
    }
  pthread_exit(NULL);
  return 0;
}

// irmodel3pm

FV3_(irmodel3pm)::FV3_(irmodel3pm)()
{
  validThread = false;
  hostThreadData.lFragmentSize = &lFragmentSize;
  hostThreadData.lFragments = &lFragments;
  hostThreadData.lBlockDelayL = &lBlockDelayL;
  hostThreadData.lSwapL = &lSwapSlot.L;
  hostThreadData.flags = &threadFlags;
  hostThreadData.threadSection = &threadSection;
  hostThreadData.event_StartThread = &event_StartThread;
  hostThreadData.event_ThreadEnded = &event_ThreadEnded;
  resume();
}

FV3_(irmodel3pm)::FV3_(~irmodel3pm)()
{
  suspend();
}

void FV3_(irmodel3pm)::resume()
{
  mainSection.lock();
  if(validThread == false)
    {
      threadFlags = FV3_IR3P_THREAD_FLAG_0; // init exit flag
      pthread_create(&lFragmentThreadHandle, NULL, irmodel3p_lfthread, &hostThreadData);
      event_StartThread.reset();
      event_ThreadEnded.reset();
      event_ThreadEnded.set();
      validThread = true;
    }
  mainSection.unlock();
}

void FV3_(irmodel3pm)::suspend()
{
  mainSection.lock();
  if(validThread == true)
    {
      threadFlags |= FV3_IR3P_THREAD_FLAG_EXIT;
      event_StartThread.trigger();
      pthread_join(lFragmentThreadHandle, NULL);
      validThread = false;
    }
  mainSection.unlock();
}

void FV3_(irmodel3pm)::loadImpulse(const fv3_float_t * inputL, long size)
  throw(std::bad_alloc)
{
  suspend();
  mainSection.lock();
  threadSection.lock();
  try
    {
      FV3_(irmodel3m)::loadImpulse(inputL, size);
    }
  catch(std::bad_alloc&)
    {
      threadSection.unlock();
      mainSection.unlock();
      throw;
    }
  threadSection.unlock();
  mainSection.unlock();
  resume();
}

void FV3_(irmodel3pm)::unloadImpulse()
{
  suspend();
  mainSection.lock();
  threadSection.lock();
  FV3_(irmodel3m)::unloadImpulse();
  threadSection.unlock();
  mainSection.unlock();
  resume();
}

void FV3_(irmodel3pm)::setFragmentSize(long size, long factor)
{
  mainSection.lock();
  threadSection.lock();
  FV3_(irmodel3m)::setFragmentSize(size, factor);
  threadSection.unlock();
  mainSection.unlock();
}

void FV3_(irmodel3pm)::mute()
{
  mainSection.lock();
  threadSection.lock();
  FV3_(irmodel3m)::mute();
  threadSection.unlock();
  mainSection.unlock();
}

void FV3_(irmodel3pm)::processZL(fv3_float_t *inputL, long numsamples)
{
  // SOL2 calls suspend() multiple times while the thread mutex is still not clear.
  mainSection.lock();
  if(validThread != true)
    {
      mainSection.unlock();
      return;
    }

  if(Lcursor == 0&&lFragments.size() > 0)
    {
      lFrameSlot.mute(lFragmentSize);
      lReverseSlot.mute(lFragmentSize-1, lFragmentSize+1);
      event_ThreadEnded.wait();
      event_ThreadEnded.reset();
      threadSection.lock();
      lBlockDelayL.push(lIFFTSlot.L);
      lFragments[0]->MULT(lBlockDelayL.get(0), lSwapSlot.L);
      lFragmentsFFT.HC2R(lSwapSlot.L, lReverseSlot.L);
      lSwapSlot.mute(lFragmentSize*2);
      threadSection.unlock();
      threadFlags |= FV3_IR3P_THREAD_FLAG_RUN;
      event_StartThread.trigger();
    }
  
  if(Scursor == 0)
    {
      sFramePointerL = lFrameSlot.L+Lcursor;
      sSwapSlot.mute(sFragmentSize*2);
      sBlockDelayL.push(sIFFTSlot.L);
      for(long i = 1;i < (long)sFragments.size();i ++){ sFragments[i]->MULT(sBlockDelayL.get(i-1), sSwapSlot.L); }
    }
  
  sOnlySlot.mute(sFragmentSize);
  
  memcpy(lFrameSlot.L+Lcursor, inputL, sizeof(fv3_float_t)*numsamples);
  memcpy(sOnlySlot.L+Scursor, inputL, sizeof(fv3_float_t)*numsamples);
  
  if(sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sOnlySlot.L, sIFFTSlot.L);
      sFragments[0]->MULT(sIFFTSlot.L, sSwapSlot.L);
      sReverseSlot.mute(sFragmentSize*2);
      sFragmentsFFT.HC2R(sSwapSlot.L, sReverseSlot.L);
    }
  
  if(lFragments.size() > 0)
    {
      for(long i = 0;i < numsamples;i ++){ inputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i] + (lReverseSlot.L+Lcursor)[i]; }
    }
  else
    {
      for(long i = 0;i < numsamples;i ++){ inputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i]; }
    }
  
  Scursor += numsamples;
  Lcursor += numsamples;

  // thread...

  if(Scursor == sFragmentSize&&sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sFramePointerL, sIFFTSlot.L);
      memcpy(restSlot.L, sReverseSlot.L+sFragmentSize, sizeof(fv3_float_t)*(sFragmentSize-1));
      Scursor = 0;
    }
  
  if(Lcursor == lFragmentSize)
    {
      if(lFragments.size() > 0)
        {
          lFragmentsFFT.R2HC(lFrameSlot.L, lIFFTSlot.L);
          memcpy(lReverseSlot.L, lReverseSlot.L+lFragmentSize, sizeof(fv3_float_t)*(lFragmentSize-1));
        }
      // thread enter
      Lcursor = Lstep = 0;
    }
  mainSection.unlock();
}

// irmodel3p

FV3_(irmodel3p)::FV3_(irmodel3p)()
{
  delete irmL, irmL = NULL;
  delete irmR, irmR = NULL;
  try
    {
      ir3mL = new FV3_(irmodel3pm);
      ir3mR = new FV3_(irmodel3pm);
      irmL = ir3mL;
      irmR = ir3mR;
    }
  catch(std::bad_alloc&)
    {
      delete irmL;
      delete irmR;
      throw;
    }
  // REAPER only calls resume() on on/off load.
  resume();
}

FV3_(irmodel3p)::FV3_(~irmodel3p)()
{
  // REAPER does not call suspend().
  suspend();
}

void FV3_(irmodel3p)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
  throw(std::bad_alloc)
{
  suspend();
  mainSection.lock();
  try
    {
      FV3_(irmodel3)::loadImpulse(inputL, inputR, size);
    }
  catch(std::bad_alloc&)
    {
      mainSection.unlock();
      throw;
    }
  mainSection.unlock();
  resume();
}

void FV3_(irmodel3p)::unloadImpulse()
{
  suspend();
  mainSection.lock();
  FV3_(irmodel3)::unloadImpulse();
  mainSection.unlock();
  resume();
}

void FV3_(irmodel3p)::resume()
{
  mainSection.lock();
  FV3_(irbase)::resume();
  mainSection.unlock();
}

void FV3_(irmodel3p)::suspend()
{
  mainSection.lock();
  FV3_(irbase)::suspend();
  mainSection.unlock();
}

void FV3_(irmodel3p)::mute()
{
  mainSection.lock();
  FV3_(irmodel3)::mute();
  mainSection.unlock();
}

void FV3_(irmodel3p)::setFragmentSize(long size, long factor)
{
  mainSection.lock();
  FV3_(irmodel3)::setFragmentSize(size, factor);
  mainSection.unlock();
}

void FV3_(irmodel3p)::setInitialDelay(long numsamples)
  throw(std::bad_alloc)
{
  mainSection.lock();
  FV3_(irbase)::setInitialDelay(numsamples);
  mainSection.unlock();
}

#include "freeverb/fv3_ns_end.h"

