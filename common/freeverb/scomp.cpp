/**
 *  Simple Compressor
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

#include "freeverb/scomp.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(scomp)::FV3_(scomp)()
{
  setRMS(0); setAttack(0); setRelease(0);
  Threshold = -10; SoftKnee = 0; // for first init
  setThreshold(-10); setSoftKnee(0); setRatio(1);
  env = 0;
}

fv3_float_t FV3_(scomp)::getEnv()
{
  return env;
}

void FV3_(scomp)::mute()
{
  Rms.mute();
  env = 0;
}

long FV3_(scomp)::getRMS()
{
  return Rms.getsize();
}

void FV3_(scomp)::setRMS(long value)
{
  Rms.setsize(value);
}

fv3_float_t FV3_(scomp)::getAttack()
{
  return Attack;
}

void FV3_(scomp)::setAttack(fv3_float_t value)
{
  Attack = value;
  if(Attack > 0)
    attackDelta = std::exp(-1.0/Attack);
  else
    attackDelta = 0;
}

fv3_float_t FV3_(scomp)::getRelease()
{
  return Release;
}

void FV3_(scomp)::setRelease(fv3_float_t value)
{
  Release = value;
  if(Release > 0)
    releaseDelta = std::exp(-1.0/Release);
  else
    releaseDelta = 0.0;
}

fv3_float_t FV3_(scomp)::getThreshold()
{
  return Threshold;
}

fv3_float_t FV3_(scomp)::getSoftKnee()
{
  return SoftKnee;
}

void FV3_(scomp)::setThreshold(fv3_float_t value)
{
  Threshold = value;
  threshold_log = std::log(Threshold);
  update();
}

void FV3_(scomp)::setSoftKnee(fv3_float_t dB)
{
  SoftKnee = dB;
  log_soft = std::log(FV3_(utils)::dB2R(SoftKnee));
  update();
}

void FV3_(scomp)::update()
{
  lowClip = Threshold*FV3_(utils)::dB2R(-SoftKnee);
  highClip = Threshold*FV3_(utils)::dB2R(SoftKnee);
}

fv3_float_t FV3_(scomp)::getRatio()
{
  return Ratio;
}

void FV3_(scomp)::setRatio(fv3_float_t value)
{
  Ratio = value;
  r = -(1-1/Ratio);
}

#include "freeverb/fv3_ns_end.h"
