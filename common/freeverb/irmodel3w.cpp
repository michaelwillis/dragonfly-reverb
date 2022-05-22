/**
 *  Impulse Response Processor model implementation
 *  Low Latency Version / Windows Multithread Version
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

#include "freeverb/irmodel3w.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

static unsigned __stdcall lfThread(void *vdParam)
{
  FV3_(lfThreadInfoW) *info = (FV3_(lfThreadInfoW)*)vdParam;
  unsigned int threadId = GetCurrentThreadId();
  wchar_t eventName_StartThread[_MAX_PATH];
  wchar_t eventName_ThreadEnded[_MAX_PATH];
  wsprintfW(eventName_StartThread, FV3_EVENT_PREFIX L"%d" FV3_EVENT_START, threadId);
  wsprintfW(eventName_ThreadEnded, FV3_EVENT_PREFIX L"%d" FV3_EVENT_ENDED, threadId);
  HANDLE event_StartThread = OpenEventW(EVENT_ALL_ACCESS, FALSE, eventName_StartThread);
  HANDLE event_ThreadEnded = OpenEventW(EVENT_ALL_ACCESS, FALSE, eventName_ThreadEnded);
  
  while(1)
    {
      Sleep(0);
      WaitForSingleObject(event_StartThread, INFINITE);
      ResetEvent(event_StartThread);

      if(*info->flags & FV3_THREAD_FLAG_RUN)
        {
          EnterCriticalSection(info->threadSection);
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
          *info->flags ^= FV3_THREAD_FLAG_RUN;
          SetEvent(event_ThreadEnded);
          LeaveCriticalSection(info->threadSection);
        }
      if(*info->flags & FV3_THREAD_FLAG_EXIT)
        {
          *info->flags ^= FV3_THREAD_FLAG_EXIT;
          break;
        }
    }
  CloseHandle(event_StartThread);
  CloseHandle(event_ThreadEnded);
  _endthreadex(0);
  return 0;
}

// irmodel3wm

FV3_(irmodel3wm)::FV3_(irmodel3wm)()
{
  validThread = false;
  threadId = 0;
  threadPriority = THREAD_PRIORITY_NORMAL;
  hostThreadData.lFragmentSize = &lFragmentSize;
  hostThreadData.lFragments = &lFragments;
  hostThreadData.lBlockDelayL = &lBlockDelayL;
  hostThreadData.lSwapL = &lSwapSlot.L;
  hostThreadData.flags = &threadFlags;
  hostThreadData.threadSection = &threadSection;
  InitializeCriticalSection(&mainSection);
  InitializeCriticalSection(&threadSection);
  // REAPER calls only resume() on on/off load.
  resume();
}

FV3_(irmodel3wm)::FV3_(~irmodel3wm)()
{
  // REAPER does not call suspend().
  suspend();
  DeleteCriticalSection(&threadSection);
  DeleteCriticalSection(&mainSection);
}

bool FV3_(irmodel3wm)::setLFThreadPriority(int priority)
{
  bool ret = false;
  EnterCriticalSection(&mainSection);
  threadPriority = priority;
  if(validThread == true) ret = SetThreadPriority(lFragmentThreadHandle, priority);
  LeaveCriticalSection(&mainSection);
  return ret;
}

void FV3_(irmodel3wm)::resume()
{
  EnterCriticalSection(&mainSection);
  if(validThread == false)
    {
      // init exit flag
      threadFlags = FV3_THREAD_FLAG_0;
      lFragmentThreadHandle = (HANDLE)_beginthreadex(NULL, 0, lfThread, &hostThreadData, CREATE_SUSPENDED, &threadId);
      // install event and resume thread
      wsprintfW(eventName_StartThread, FV3_EVENT_PREFIX L"%d" FV3_EVENT_START, threadId);
      wsprintfW(eventName_ThreadEnded, FV3_EVENT_PREFIX L"%d" FV3_EVENT_ENDED, threadId);
      event_StartThread = CreateEventW(NULL, TRUE, FALSE, eventName_StartThread);
      event_ThreadEnded = CreateEventW(NULL, TRUE, TRUE,  eventName_ThreadEnded);
      event_trigger = OpenEventW(EVENT_ALL_ACCESS, FALSE, eventName_StartThread);
      event_waitfor = OpenEventW(EVENT_ALL_ACCESS, FALSE, eventName_ThreadEnded);
      SetThreadPriority(lFragmentThreadHandle, threadPriority);
      ResumeThread(lFragmentThreadHandle);
      validThread = true;
    }
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3wm)::suspend()
{
  EnterCriticalSection(&mainSection);
  if(validThread == true)
    {
      threadFlags |= FV3_THREAD_FLAG_EXIT;
      SetEvent(event_trigger);
      WaitForSingleObject(lFragmentThreadHandle, INFINITE);
      CloseHandle(lFragmentThreadHandle);
      CloseHandle(event_trigger);
      CloseHandle(event_waitfor);
      CloseHandle(event_StartThread);
      CloseHandle(event_ThreadEnded);
      validThread = false;
    }
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3wm)::loadImpulse(const fv3_float_t * inputL, long size)
  throw(std::bad_alloc)
{
  suspend();
  EnterCriticalSection(&mainSection);
  EnterCriticalSection(&threadSection);
  try
    {
      FV3_(irmodel3m)::loadImpulse(inputL, size);
    }
  catch(std::bad_alloc&)
    {
      LeaveCriticalSection(&threadSection);
      LeaveCriticalSection(&mainSection);
      throw;
    }
  LeaveCriticalSection(&threadSection);
  LeaveCriticalSection(&mainSection);
  resume();
}

void FV3_(irmodel3wm)::unloadImpulse()
{
  suspend();
  EnterCriticalSection(&mainSection);
  EnterCriticalSection(&threadSection);
  FV3_(irmodel3m)::unloadImpulse();
  LeaveCriticalSection(&threadSection);
  LeaveCriticalSection(&mainSection);
  resume();
}

void FV3_(irmodel3wm)::setFragmentSize(long size, long factor)
{
  EnterCriticalSection(&mainSection);
  EnterCriticalSection(&threadSection);
  FV3_(irmodel3m)::setFragmentSize(size, factor);
  LeaveCriticalSection(&threadSection);
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3wm)::mute()
{
  EnterCriticalSection(&mainSection);
  EnterCriticalSection(&threadSection);
  FV3_(irmodel3m)::mute();
  LeaveCriticalSection(&threadSection);
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3wm)::processZL(fv3_float_t *inputL, long numsamples)
{
  // SOL2 calls suspend() many times though the thread mutex is not clear.
  EnterCriticalSection(&mainSection);
  if(validThread != true)
    {
      LeaveCriticalSection(&mainSection);
      return;
    }

  if(Lcursor == 0&&lFragments.size() > 0)
    {
      lFrameSlot.mute(lFragmentSize);
      lReverseSlot.mute(lFragmentSize-1, lFragmentSize+1);
     
      WaitForSingleObject(event_waitfor, INFINITE);
      ResetEvent(event_waitfor);

      EnterCriticalSection(&threadSection);
      lBlockDelayL.push(lIFFTSlot.L);
      lFragments[0]->MULT(lBlockDelayL.get(0), lSwapSlot.L);
      lFragmentsFFT.HC2R(lSwapSlot.L, lReverseSlot.L);
      lSwapSlot.mute(lFragmentSize*2);
      LeaveCriticalSection(&threadSection);
      
      // trigger thread
      threadFlags |= FV3_THREAD_FLAG_RUN;
      SetEvent(event_trigger);
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
  LeaveCriticalSection(&mainSection);
}

// irmodel3w

FV3_(irmodel3w)::FV3_(irmodel3w)()
{
  delete irmL, irmL = NULL;
  delete irmR, irmR = NULL;
  try
    {
      ir3wmL = new FV3_(irmodel3wm);
      ir3wmR = new FV3_(irmodel3wm);
      irmL = ir3mL = ir3wmL;
      irmR = ir3mR = ir3wmR;
    }
  catch(std::bad_alloc&)
    {
      delete irmL;
      delete irmR;
      throw;
    }
  InitializeCriticalSection(&mainSection);
  // REAPER only calls resume() on on/off load.
  resume();
}

FV3_(irmodel3w)::FV3_(~irmodel3w)()
{
  // REAPER does not call suspend().
  suspend();
  DeleteCriticalSection(&mainSection);
}

void FV3_(irmodel3w)::loadImpulse(const fv3_float_t * inputL, const fv3_float_t * inputR, long size)
  throw(std::bad_alloc)
{
  suspend();
  EnterCriticalSection(&mainSection);
  try
    {
      FV3_(irmodel3)::loadImpulse(inputL, inputR, size);
    }
  catch(std::bad_alloc&)
    {
      LeaveCriticalSection(&mainSection);
      throw;
    }
  LeaveCriticalSection(&mainSection);
  resume();
}

void FV3_(irmodel3w)::unloadImpulse()
{
  suspend();
  EnterCriticalSection(&mainSection);
  FV3_(irmodel3)::unloadImpulse();
  LeaveCriticalSection(&mainSection);
  resume();
}

void FV3_(irmodel3w)::resume()
{
  EnterCriticalSection(&mainSection);
  FV3_(irbase)::resume();
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3w)::suspend()
{
  EnterCriticalSection(&mainSection);
  FV3_(irbase)::suspend();
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3w)::mute()
{
  EnterCriticalSection(&mainSection);
  FV3_(irmodel3)::mute();
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3w)::setFragmentSize(long size, long factor)
{
  EnterCriticalSection(&mainSection);
  FV3_(irmodel3)::setFragmentSize(size, factor);
  LeaveCriticalSection(&mainSection);
}

void FV3_(irmodel3w)::setInitialDelay(long numsamples)
  throw(std::bad_alloc)
{
  EnterCriticalSection(&mainSection);
  FV3_(irbase)::setInitialDelay(numsamples);
  LeaveCriticalSection(&mainSection);
}

bool FV3_(irmodel3w)::setLFThreadPriority(int priority)
{
  bool ret = false;
  EnterCriticalSection(&mainSection);
  if(ir3wmL->setLFThreadPriority(priority)) ret = true;
  if(ir3wmR->setLFThreadPriority(priority)) ret = true;
  LeaveCriticalSection(&mainSection);
  return ret;
}

#include "freeverb/fv3_ns_end.h"

