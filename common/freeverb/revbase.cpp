/**
 *  Reverb Base Class
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

#include "freeverb/revbase.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(revbase)::FV3_(revbase)()
	      throw(std::bad_alloc)
{
  setwetr(1); setdryr(1); setwidth(1);
  primeMode = true; muteOnChange = false; rsfactor = 1.; currentfs = FV3_REVBASE_DEFAULT_FS;
  setPreDelay(0); setReverbType(FV3_REVTYPE_SELF);
}

FV3_(revbase)::FV3_(~revbase)()
{

}

void FV3_(revbase)::setReverbType(unsigned type)
{
  reverbType = type;
}

unsigned FV3_(revbase)::getReverbType()
{
  return reverbType;
}

void FV3_(revbase)::setInitialDelay(long numsamples)
{
  initialDelay = numsamples;
  preDelay = (fv3_float_t)initialDelay*1000.0f/currentfs;
  if(initialDelay >= 0)
    {
#ifdef DEBUG
      std::fprintf(stderr, "revbase::setInitialDelay(%ld) delayW(%ld))\n", numsamples, initialDelay);
#endif
      delayL.setsize(0);
      delayR.setsize(0);
      delayWL.setsize(initialDelay);
      delayWR.setsize(initialDelay);
    }
  else
    {
      long dryD = - initialDelay;
#ifdef DEBUG
      std::fprintf(stderr, "revbase::setInitialDelay(%ld) delayD(%ld))\n", numsamples, dryD);
#endif
      delayL.setsize(dryD);
      delayR.setsize(dryD);
      delayWL.setsize(0);
      delayWR.setsize(0);
    }
}

long FV3_(revbase)::getInitialDelay()
{
  return initialDelay;
}

void FV3_(revbase)::setPreDelay(fv3_float_t value_ms)
{
  setInitialDelay((long)(currentfs*(preDelay = value_ms)/1000.));
}

fv3_float_t FV3_(revbase)::getPreDelay() const
{
  return preDelay;
}

long FV3_(revbase)::getLatency()
{
  return 0;
}

void FV3_(revbase)::printconfig()
{
  std::fprintf(stderr, "*** revbase config ***\n");
  std::fprintf(stderr, "Fs = %f[Hz]\n",currentfs);
  std::fprintf(stderr, "Wet %f Dry %f Width %f\n", (double)wet, (double)dry, (double)width);
}

void FV3_(revbase)::mute()
{
  delayL.mute(); delayR.mute();
  delayWL.mute(); delayWR.mute();
}

void FV3_(revbase)::setwet(fv3_float_t value)
{
  wetDB = value;
  wet = FV3_(utils)::dB2R(value);
  update_wet();

}

fv3_float_t FV3_(revbase)::getwet()
{
  return wetDB;
}

void FV3_(revbase)::setwetr(fv3_float_t value)
{
  if(value == 0)
    {
      wet = value;
      wetDB = FP_NAN;
    }
  else
    {
      wetDB = FV3_(utils)::R2dB(wet = value);
    }
  update_wet();
}

fv3_float_t FV3_(revbase)::getwetr() const
{
  return wet;
}

void FV3_(revbase)::setdry(fv3_float_t value)
{
  dryDB = value;
  dry = FV3_(utils)::dB2R(value);
}

fv3_float_t FV3_(revbase)::getdry()
{
  return dryDB;
}

void FV3_(revbase)::setdryr(fv3_float_t value)
{
  if(value == 0)
    {
      dry = value;
      dryDB = FP_NAN;
    }
  else
    {
      dryDB = FV3_(utils)::R2dB(dry = value);
    }
}

fv3_float_t FV3_(revbase)::getdryr()
{
  return dry;
}

void FV3_(revbase)::setwidth(fv3_float_t value)
{
  width = value;
  update_wet();
}

fv3_float_t FV3_(revbase)::getwidth() const
{
  return width;
}

void FV3_(revbase)::update_wet()
{
  wet1 = wet*(width/2 + 0.5f);
  wet2 = wet*((1-width)/2);
}

fv3_float_t FV3_(revbase)::getSampleRate()
{
  return currentfs;
}

void FV3_(revbase)::setSampleRate(fv3_float_t fs)
		    throw(std::bad_alloc)
{
  if(fs <= 0) return;
  currentfs = fs;
  setFsFactors();
  if(muteOnChange) mute();
}

void FV3_(revbase)::setRSFactor(fv3_float_t value)
{
  if(value <= 0) return;
  rsfactor = value;
  setFsFactors();
  if(muteOnChange) mute();
}

fv3_float_t FV3_(revbase)::getRSFactor() const
{
  return rsfactor;
}

void FV3_(revbase)::setFsFactors()
{
#ifdef DEBUG
  std::fprintf(stderr, "revbase::setFsFactors(%f,%f)\n", getSampleRate(), getRSFactor());
#endif
  setPreDelay(getPreDelay());
}

void FV3_(revbase)::setPrimeMode(bool value)
{
  primeMode = value;
}

bool FV3_(revbase)::getPrimeMode()
{
  return primeMode;
}

void FV3_(revbase)::setMuteOnChange(bool value)
{
  muteOnChange = value;
}

bool FV3_(revbase)::getMuteOnChange()
{
  return muteOnChange;
}

fv3_float_t FV3_(revbase)::limFs2(fv3_float_t fq)
{
  if(fq < 0) fq = 0;
  if(fq > getTotalSampleRate()/2) fq = getTotalSampleRate()/2;
  return fq;
}

long FV3_(revbase)::f_(long def, fv3_float_t factor){ return f_((fv3_float_t)def, factor); }
long FV3_(revbase)::f_(fv3_float_t def, fv3_float_t factor)
{
  long fact = (long)(def*factor); if(fact <= 0) fact = 1;
  return fact;
}

long FV3_(revbase)::p_(long def, fv3_float_t factor){ return p_((fv3_float_t)def, factor); }
long FV3_(revbase)::p_(fv3_float_t def, fv3_float_t factor)
{
  long base = f_(def,factor);
  if(primeMode){ while(!FV3_(utils)::isPrime(base)) base++; }
  return base;
}

#include "freeverb/fv3_ns_end.h"
