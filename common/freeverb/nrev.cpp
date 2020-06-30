/**
 *  CCRMA NRev
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

#include "freeverb/nrev.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// Fs = 25.641 kHz
const long FV3_(nrev)::combCo[] =    {1433, 1601, 1867, 2053, 2251, 2399,};
const long FV3_(nrev)::allpassCo[] = {347, 113, 37, 59, 53, 43, 37, 29, 19,};
//                                   (  0    1   2) F3  R4  L5  R6 RL7 RR8

FV3_(nrev)::FV3_(nrev)()
	   throw(std::bad_alloc)
{
  hpf = lpfL = lpfR = 0;
  setrt60(1);
  setfeedback(0.7);
  setdamp(0.5);
  setdamp2(0.5);
  setdamp3(0.5);
  setdccutfreq(8);
}

void FV3_(nrev)::printconfig()
{
  std::fprintf(stderr, "*** NRev config ***\n");
  std::fprintf(stderr, "roomsize %f damp %f damp2 %f damp3 %f\n", (double)roomsize, (double)damp, (double)damp2, (double)damp3);
}

void FV3_(nrev)::mute()
{
  FV3_(revbase)::mute();
  for (long i = 0;i < FV3_NREV_NUM_COMB;i ++)
    {
      combL[i].mute(); combR[i].mute();
    }
  for (long i = 0;i < FV3_NREV_NUM_ALLPASS;i ++)
    {
      allpassL[i].mute(); allpassR[i].mute();
    }
  hpf = lpfL = lpfR = 0;
  inDCC.mute(); lLDCC.mute(); lRDCC.mute();
}

void FV3_(nrev)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
		throw(std::bad_alloc)
{
  if(numsamples <= 0) return;
  long count = numsamples;

  fv3_float_t outL, outR;
  while(count-- > 0)
    {
      outL = outR = 0;
      hpf = damp3_1*inDCC(*inputL + *inputR) - damp3*hpf;
      UNDENORMAL(hpf);

      hpf *= FV3_NREV_SCALE_WET;
      
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outL += combL[i]._process(hpf);
      for(long i = 0;i < 3;i ++) outL = allpassL[i]._process_ov(outL);
      lpfL = damp2*lpfL + damp2_1*outL; UNDENORMAL(lpfL);
      outL = allpassL[3]._process_ov(lpfL); outL = allpassL[5]._process_ov(outL);
      outL = delayWL(lLDCC(outL));
      
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outR += combR[i]._process(hpf);
      for(long i = 0;i < 3;i ++) outR = allpassR[i]._process_ov(outR);
      lpfR = damp2*lpfR + damp2_1*outR; UNDENORMAL(lpfR);
      outR = allpassR[3]._process_ov(lpfR); outR = allpassL[6]._process_ov(outR);
      outR = delayWR(lRDCC(outR));
      
      *outputL = outL*wet1 + outR*wet2 + delayL(*inputL)*dry;
      *outputR = outR*wet1 + outL*wet2 + delayR(*inputR)*dry;
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(nrev)::setcombfeedback(fv3_float_t back, long zero)
{
  for(long i = 0;i < FV3_NREV_NUM_COMB;i ++)
    {
      combL[i].setfeedback(zero*std::pow((fv3_float_t)10.0, -3 * (fv3_float_t)combL[i].getsize() / back));
      combR[i].setfeedback(zero*std::pow((fv3_float_t)10.0, -3 * (fv3_float_t)combR[i].getsize() / back));
    }
}

void FV3_(nrev)::setrt60(fv3_float_t value)
{
  fv3_float_t back = (roomsize = value) * getTotalSampleRate();
  UNDENORMAL(back);
  if(back > 0)
    setcombfeedback(back, 1);
  else
    setcombfeedback(1, 0);
}

fv3_float_t FV3_(nrev)::getrt60()
{
  return roomsize;
}

void FV3_(nrev)::setfeedback(fv3_float_t value)
{
  feedback = value;
  for(long i = 0;i < FV3_NREV_NUM_ALLPASS;i ++)
    {
      allpassL[i].setfeedback(value);
      allpassR[i].setfeedback(value);
    }
}

fv3_float_t FV3_(nrev)::getfeedback()
{
  return feedback;
}

void FV3_(nrev)::setdamp(fv3_float_t value)
{
  damp = value;
  for(long i = 0;i < FV3_NREV_NUM_COMB;i ++)
    {
      combL[i].setdamp(damp);
      combR[i].setdamp(damp);
    }
}

fv3_float_t FV3_(nrev)::getdamp()
{
  return damp;
}

void FV3_(nrev)::setdamp2(fv3_float_t value)
{
  damp2 = value;
  damp2_1 = 1 - value;
}

fv3_float_t FV3_(nrev)::getdamp2()
{
  return damp2;
}

void FV3_(nrev)::setdamp3(fv3_float_t value)
{
  damp3 = value;
  damp3_1 = 1 - value;
}

fv3_float_t FV3_(nrev)::getdamp3()
{
  return damp3;
}

void FV3_(nrev)::setdccutfreq(fv3_float_t value)
{
  dccutfq = limFs2(value);
  inDCC.setCutOnFreq(dccutfq, getTotalSampleRate());
  lLDCC.setCutOnFreq(dccutfq, getTotalSampleRate());
  lRDCC.setCutOnFreq(dccutfq, getTotalSampleRate());
}

fv3_float_t FV3_(nrev)::getdccutfreq()
{
  return dccutfq;
}

void FV3_(nrev)::setFsFactors()
{
  FV3_(revbase)::setFsFactors();

  fv3_float_t totalFactor = getTotalFactorFs()/(fv3_float_t)FV3_NREV_DEFAULT_FS;
  long stereoSpread = f_((long)FV3_NREV_STEREO_SPREAD, totalFactor);
  for(long i = 0;i < FV3_NREV_NUM_COMB;i ++)
    {
      combL[i].setsize(p_(combCo[i],totalFactor));
      combR[i].setsize(p_(f_(combCo[i],totalFactor)+stereoSpread,1));
    }
  for(long i = 0;i < FV3_NREV_NUM_ALLPASS;i ++)
    {
      allpassL[i].setsize(p_(allpassCo[i],totalFactor));
      allpassR[i].setsize(p_(f_(allpassCo[i],totalFactor)+stereoSpread,1));
    }
  setrt60(getrt60());
  setdccutfreq(getdccutfreq());
}

#include "freeverb/fv3_ns_end.h"
