/**
 *  Sweep Generator
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

#include "freeverb/sweep.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(sweep)::FV3_(sweep)()
{
  setSampleRate(48000);
  setStartFs(20); setEndFs(20000);
  setInitialMuteLength(500);
  setLeadInLength(500);
  setSweepLength(8000);
  setLeadOutLength(500);
  setEndMuteLength(1000);
  setSweepMode(FV3_SWP_DEFAULT);
  init();
  inverseMode = false;
}

void FV3_(sweep)::setSampleRate(fv3_float_t fs)
{
  currentfs = fs;
}

fv3_float_t FV3_(sweep)::getSampleRate()
{
  return currentfs;
}

void FV3_(sweep)::setStartFs(fv3_float_t fs)
{
  startfs = fs;
}

void FV3_(sweep)::setEndFs(fv3_float_t fs)
{
  endfs = fs;
}

void FV3_(sweep)::setSweepMode(long mode)
{
  sweepMode = mode;
}

long FV3_(sweep)::getSweepMode()
{
  return sweepMode;
}

void FV3_(sweep)::init()
{
  countf = countr = -1;
  initialMuteLength = currentfs*initialmutelength/1000.;
  leadInLength = currentfs*leadinlength/1000.;
  sweepLength = currentfs*sweeplength/1000.;
  leadOutLength = currentfs*leadoutlength/1000.;
  endMuteLength = currentfs*endmutelength/1000.;
  fv3_float_t W1 = 2. * M_PI * startfs;
  fv3_float_t W2 = 2. * M_PI * endfs;
  fv3_float_t TRatio = std::log(W2/W1);
  S1 = (W1 * sweeplength/1000.) / TRatio;
  S2 = TRatio / sweepLength;
  DecayTime = sweepLength*std::log(2.0)/TRatio;

  L1 = 2. * M_PI * startfs/currentfs;
  L2 = M_PI * (endfs-startfs)/(sweeplength/1000.)/currentfs/currentfs;

  WC1In = M_PI / (leadInLength - 1);
  WC2In = 2 * M_PI / (leadInLength - 1);
  WC1Out = M_PI / (leadOutLength - 1);
  WC2Out = 2 * M_PI / (leadOutLength - 1);
}

fv3_float_t FV3_(sweep)::forward_sweep(fv3_float_t input)
{
  switch(sweepMode)
    {
    case FV3_SWP_EXP:
    case FV3_SWP_LIN:
      return forward_sweep_explin(input);
    default:
      return 0;
    }
}

fv3_float_t FV3_(sweep)::forward_sweep_explin(fv3_float_t input)
{
  countf ++;

  // Initial silence
  if(countf < initialMuteLength)
    return 0;

  // Final silence
  if(countf >= (initialMuteLength+sweepLength)&&countf < (initialMuteLength+sweepLength+endMuteLength))
    return 0;

  // base sweep
  fv3_float_t output = 0;
  fv3_float_t I = (fv3_float_t)(countf - initialMuteLength);
  switch(sweepMode)
    {
    case FV3_SWP_EXP:
      output = std::sin(S1 * (std::exp(I * S2) - 1.0));
      break;
    case FV3_SWP_LIN:
      output = std::sin(L1 * I + L2 * I * I);
      break;
    default:
      ;
    }
  
  // Initial lead in
  if(countf >= initialMuteLength&&countf < (initialMuteLength+leadInLength))
    {
      // Blackman window
      fv3_float_t WC = (0.42 - 0.5 * std::cos(WC1In * I) + 0.08 * std::cos(WC2In * I));
      return input*output*WC;
    }

  // Full sweep
  if(countf >= (initialMuteLength+leadInLength)&&countf < (initialMuteLength+sweepLength-leadOutLength))
    {
      return input*output;
    }
  
  // Final lead out
  if(countf >= (initialMuteLength+sweepLength-leadOutLength)&&countf < (initialMuteLength+sweepLength))
    {
      fv3_float_t J = (fv3_float_t)(initialMuteLength + sweepLength - countf);
      fv3_float_t WC = (0.42 - 0.5 * std::cos(WC1Out * J) + 0.08 * std::cos(WC2Out * J));
      return input*output*WC;
    }
  
  // Revert to Start Position
  if(countf > (initialMuteLength+sweepLength+endMuteLength))
    countf = -1;

  return 0;
}


fv3_float_t FV3_(sweep)::inverse_sweep(fv3_float_t input)
{
  switch(sweepMode)
    {
    case FV3_SWP_EXP:
    case FV3_SWP_LIN:
      return inverse_sweep_explin(input);
    default:
      return 0;
    }
}

fv3_float_t FV3_(sweep)::inverse_sweep_explin(fv3_float_t input)
{
  countr ++;

  // Initial silence
  if(countr < initialMuteLength)
    return 0;
  
  // Final silence
  if(countr >= (initialMuteLength+sweepLength)&&countr < (initialMuteLength+sweepLength+endMuteLength))
    return 0;

  fv3_float_t output = 0;
  fv3_float_t I = (fv3_float_t)(countr - initialMuteLength);
  fv3_float_t J = (fv3_float_t)(sweepLength - I);
  fv3_float_t Decay = std::pow(0.5, I/DecayTime);

  switch(sweepMode)
    {
    case FV3_SWP_EXP:
      output = std::sin(S1 * (std::exp(J * S2) - 1.0)) * Decay;
      break;
    case FV3_SWP_LIN:
      output = std::sin(L1 * J + L2 * J * J);
      break;
    default:
      ;
    }

  // Initial lead in
  if(countr >= initialMuteLength&&countr < (initialMuteLength+leadOutLength))
    {
      fv3_float_t WC = (0.42 - 0.5 * std::cos(WC1Out * I) + 0.08 * std::cos(WC2Out * I));
      return input*output*WC;
    }
  
  // Full sweep
  if(countr >= (initialMuteLength+leadOutLength)&&countr < (initialMuteLength+sweepLength-leadInLength))
    {
      return input*output;
    }
  
  // Final lead out
  if(countr >= (initialMuteLength+sweepLength-leadInLength)&&countr < (initialMuteLength+sweepLength))
    {
      fv3_float_t WC = (0.42 - 0.5 * std::cos(WC1In * J) + 0.08 * std::cos(WC2In * J));
      return input*output*WC;
    }
  
  // Revert to Start Position
  if(countr > (initialMuteLength+sweepLength+endMuteLength))
    countr = -1;

  return 0;
}

fv3_float_t FV3_(sweep)::process(fv3_float_t input)
{
  if(inverseMode == false)
    return forward_sweep(input);
  else
    return inverse_sweep(input);
}

long FV3_(sweep)::getTotalLength()
{
  return initialMuteLength+sweepLength+endMuteLength;
}

#include "freeverb/fv3_ns_end.h"
