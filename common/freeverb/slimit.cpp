/**
 *  Simple Limiter
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

#include "freeverb/slimit.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(slimit)::FV3_(slimit)()
{
  buffer = NULL;
  lookahead = bufsize = bufidx = 0; env = 0;
  setRMS(0); setAttack(0); setRelease(0);
  Lookahead = 5; LookaheadRatio = 1; Ceiling = Threshold = 0; // for first init
  setLookahead(5); setLookaheadRatio(1); setThreshold(0);
  setCeiling(0);
}

FV3_(slimit)::~FV3_(slimit)()
{
  if(bufsize > 0) delete[] buffer;
}

fv3_float_t FV3_(slimit)::getEnv()
{
  return env;
}

void FV3_(slimit)::mute()
{
  env = 0;
  bufidx = 0;
  if(lookahead > 0) FV3_(utils)::mute(buffer, bufsize);
  Rms.mute();
}

long FV3_(slimit)::getRMS()
{
  return Rms.getsize();
}

void FV3_(slimit)::setRMS(long value)
{
  if(value < 0) return;
  Rms.setsize(value);
}

long FV3_(slimit)::getLookahead()
{
  return lookahead;
}

void FV3_(slimit)::setLookahead(long value)
		  throw(std::bad_alloc)
{
  if(value < 0) return;
  lookahead = value;
  Lookahead = lookahead;

  bufsize = lookahead+1;
  bufidx = 0;
  
  if(bufidx > 0) delete[] buffer;
  try
    {
      buffer = new fv3_float_t[lookahead+1];
    }
  catch(std::bad_alloc&)
    {
      std::fprintf(stderr, "FV3_(slimit)::setLa bad_alloc\n");
      delete[] buffer;
      bufsize = 0;
      throw;
    }

  update();
  mute();
}

fv3_float_t FV3_(slimit)::getLookaheadRatio()
{
  return LookaheadRatio;
}

void FV3_(slimit)::setLookaheadRatio(fv3_float_t value)
{
  if(value <= 0) return;
  LookaheadRatio = value;
  update();
}

fv3_float_t FV3_(slimit)::getAttack()
{
  return Attack;
}

void FV3_(slimit)::setAttack(fv3_float_t value)
{
  if(value < 0) return;
  Attack = value;
  if(Attack > 0)
    attackDelta = std::exp(-1.0/Attack);
  else
    attackDelta = 0;
}

fv3_float_t FV3_(slimit)::getRelease()
{
  return Release;
}

void FV3_(slimit)::setRelease(fv3_float_t value)
{
  if(value < 0) return;
  Release = value;
  if(Release > 0)
    releaseDelta = std::exp(-1.0/Release);
  else
    releaseDelta = 0.0;
}

fv3_float_t FV3_(slimit)::getThreshold()
{
  return Threshold;
}

void FV3_(slimit)::setThreshold(fv3_float_t value)
{
  Threshold = value;
  update();
}

fv3_float_t FV3_(slimit)::getCeiling()
{
  return Ceiling;
}

void FV3_(slimit)::setCeiling(fv3_float_t value)
{
  Ceiling = value;
  update();
}

void FV3_(slimit)::update()
{
  fv3_float_t db_ceil = FV3_(utils)::R2dB(Ceiling);
  fv3_float_t db_thr = FV3_(utils)::R2dB(Threshold);
  R1 = std::log(10.0)/20.0;
  C_T2 = (db_ceil-db_thr)*(db_ceil-db_thr);
  C_2T = db_ceil - 2*db_thr;
  R2 = R1*db_ceil;
  if(Lookahead > 0)
    lookaheadDelta = LookaheadRatio/Lookahead;
  else
    lookaheadDelta = 0;
}

#include "freeverb/fv3_ns_end.h"
