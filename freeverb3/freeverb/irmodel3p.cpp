/**
 *  Impulse Response Processor model implementation
 *  Low Latency Version / Pthread Multithread Version
 *
 *  Copyright (C) 2014 Teru Kamogashira
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
		      info->lFragments->at(i+1)->MULT(info->lBlockDelayL->get(i), info->lBlockDelayR->get(i), *info->lSwapL, *info->lSwapR);
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

FV3_(irmodel3p)::FV3_(irmodel3p)()
{
#ifdef DEBUG
  fprintf(stderr, "irmodel3p::irmodel3p()\n");
#endif
  validThread = false;
  hostThreadData.lFragmentSize = &lFragmentSize;
  hostThreadData.lFragments = &lFragments;
  hostThreadData.lBlockDelayL = &lBlockDelayL;
  hostThreadData.lBlockDelayR = &lBlockDelayR;
  hostThreadData.lSwapL = &lSwapSlot.L;
  hostThreadData.lSwapR = &lSwapSlot.R;
  hostThreadData.flags = &threadFlags;
  hostThreadData.threadSection = &threadSection;
  hostThreadData.event_StartThread = &event_StartThread;
  hostThreadData.event_ThreadEnded = &event_ThreadEnded;
  // REAPER calls only resume() on on/off load.
  resume();
}

FV3_(irmodel3p)::FV3_(~irmodel3p)()
{
#ifdef DEBUG
  fprintf(stderr, "irmodel3p::~irmodel3p()\n");
#endif
  // REAPER does not call suspend().
  suspend();
}

void FV3_(irmodel3p)::resume()
{
  mainSection.lock();
  if(validThread == false)
    {
      // init exit flag
      threadFlags = FV3_IR3P_THREAD_FLAG_0;
      pthread_create(&lFragmentThreadHandle, NULL, irmodel3p_lfthread, &hostThreadData);
      event_StartThread.reset();
      event_ThreadEnded.reset();
      event_ThreadEnded.set();
      validThread = true;
    }
  mainSection.unlock();
}

void FV3_(irmodel3p)::suspend()
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

void FV3_(irmodel3p)::loadImpulse(fv3_float_t * inputL, fv3_float_t * inputR, long size)
		     throw(std::bad_alloc)
{
  suspend();
  mainSection.lock();
  threadSection.lock();
  try
    {
      FV3_(irmodel3)::loadImpulse(inputL, inputR, size);
    }
  catch(std::bad_alloc)
    {
      threadSection.unlock();
      mainSection.unlock();
      throw;
    }
  threadSection.unlock();
  mainSection.unlock();
  resume();
}

void FV3_(irmodel3p)::unloadImpulse()
{
  suspend();
  mainSection.lock();
  threadSection.lock();
  FV3_(irmodel3)::unloadImpulse();
  threadSection.unlock();
  mainSection.unlock();
  resume();
}

void FV3_(irmodel3p)::setFragmentSize(long size, long factor)
{
  mainSection.lock();
  threadSection.lock();
  FV3_(irmodel3)::setFragmentSize(size, factor);
  threadSection.unlock();
  mainSection.unlock();
}

void FV3_(irmodel3p)::mute()
{
  mainSection.lock();
  threadSection.lock();
  FV3_(irmodel3)::mute();
  threadSection.unlock();
  mainSection.unlock();
}

void FV3_(irmodel3p)::processZL(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples, unsigned options)
{
  // SOL2 calls suspend() many times though the thread mutex is not clear.
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
      lBlockDelayR.push(lIFFTSlot.R);
      lFragments[0]->MULT(lBlockDelayL.get(0), lBlockDelayR.get(0), lSwapSlot.L, lSwapSlot.R);
      lFragmentsFFT.HC2R(lSwapSlot.L, lSwapSlot.R, lReverseSlot.L, lReverseSlot.R);
      lSwapSlot.mute(lFragmentSize*2);
      threadSection.unlock();
      threadFlags |= FV3_IR3P_THREAD_FLAG_RUN;
      event_StartThread.trigger();
    }
  
  if(Scursor == 0)
    {
      sFramePointerL = lFrameSlot.L+Lcursor;
      sFramePointerR = lFrameSlot.R+Lcursor;
      sSwapSlot.mute(sFragmentSize*2);
      sBlockDelayL.push(sIFFTSlot.L);
      sBlockDelayR.push(sIFFTSlot.R);
      for(long i = 1;i < (long)sFragments.size();i ++)
	sFragments[i]->MULT(sBlockDelayL.get(i-1), sBlockDelayR.get(i-1), sSwapSlot.L, sSwapSlot.R);
    }
  
  sOnlySlot.mute(sFragmentSize);
  
  if(options & FV3_IR_MONO2STEREO)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  (lFrameSlot.L+Lcursor)[i] = (lFrameSlot.R+Lcursor)[i] = (sOnlySlot.L+Scursor)[i] = (sOnlySlot.R+Scursor)[i] = (inputL[i] + inputR[i])/2.0;
	}
    }
  else
    {
      memcpy(lFrameSlot.L+Lcursor, inputL, sizeof(fv3_float_t)*numsamples);
      memcpy(lFrameSlot.R+Lcursor, inputR, sizeof(fv3_float_t)*numsamples);
      memcpy(sOnlySlot.L+Scursor, inputL, sizeof(fv3_float_t)*numsamples);
      memcpy(sOnlySlot.R+Scursor, inputR, sizeof(fv3_float_t)*numsamples);
    }
  
  if(sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sOnlySlot.L, sOnlySlot.R, sIFFTSlot.L, sIFFTSlot.R);
      sFragments[0]->MULT(sIFFTSlot.L, sIFFTSlot.R, sSwapSlot.L, sSwapSlot.R);
      sReverseSlot.mute(sFragmentSize*2);
      sFragmentsFFT.HC2R(sSwapSlot.L, sSwapSlot.R, sReverseSlot.L, sReverseSlot.R);
    }
  
  if(lFragments.size() > 0)
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i] + (lReverseSlot.L+Lcursor)[i];
	  outputR[i] = (sReverseSlot.R+Scursor)[i] + (restSlot.R+Scursor)[i] + (lReverseSlot.R+Lcursor)[i];
	}
    }
  else
    {
      for(long i = 0;i < numsamples;i ++)
	{
	  outputL[i] = (sReverseSlot.L+Scursor)[i] + (restSlot.L+Scursor)[i];
	  outputR[i] = (sReverseSlot.R+Scursor)[i] + (restSlot.R+Scursor)[i];
	}
    }
  
  Scursor += numsamples;
  Lcursor += numsamples;

  // thread...

  if(Scursor == sFragmentSize&&sFragments.size() > 0)
    {
      sFragmentsFFT.R2HC(sFramePointerL, sFramePointerR, sIFFTSlot.L, sIFFTSlot.R);
      memcpy(restSlot.L, sReverseSlot.L+sFragmentSize, sizeof(fv3_float_t)*(sFragmentSize-1));
      memcpy(restSlot.R, sReverseSlot.R+sFragmentSize, sizeof(fv3_float_t)*(sFragmentSize-1));
      Scursor = 0;
    }
  
  if(Lcursor == lFragmentSize)
    {
      if(lFragments.size() > 0)
	{
	  lFragmentsFFT.R2HC(lFrameSlot.L, lFrameSlot.R, lIFFTSlot.L, lIFFTSlot.R);
	  memcpy(lReverseSlot.L, lReverseSlot.L+lFragmentSize, sizeof(fv3_float_t)*(lFragmentSize-1));
	  memcpy(lReverseSlot.R, lReverseSlot.R+lFragmentSize, sizeof(fv3_float_t)*(lFragmentSize-1));
	}
      // thread enter
      Lcursor = Lstep = 0;
    }
  mainSection.unlock();
}

#include "freeverb/fv3_ns_end.h"

