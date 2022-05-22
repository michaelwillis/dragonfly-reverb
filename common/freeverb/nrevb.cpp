/**
 *  CCRMA NRev v2 (nrevb)
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

#include "freeverb/nrevb.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// Fs = 25.641 kHz
// const long FV3_(nrev)::allpassCo[] = {347, 113, 37, 59, 53, 43, 37, 29, 19,};
//                                      (  0    1   2) F3  R4  L5  R6 RL7 RR8
// const long FV3_(nrev)::combCo[] =    {1433, 1601, 1867, 2053, 2251, 2399,};
const long FV3_(nrevb)::combCo2[] =    {1257, 1333, 1499, 1501, 1535, 1637, 1738, 1904, 1999, 2020, 2120, 2509,};
const long FV3_(nrevb)::allpassCo2[] = {57, 87, 103,};

FV3_(nrevb)::FV3_(nrevb)()
	    throw(std::bad_alloc)
{
  lastL = lastR = 0;
  setdamp(0.1);
  setfeedback(0.5);
  setapfeedback(0.2);
}

void FV3_(nrevb)::mute()
{
  FV3_(nrev)::mute();
  lastL = lastR = 0;
  for (long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++)
    {
      comb2L[i].mute(); comb2R[i].mute();
    }
  for (long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++)
    {
      allpass2L[i].mute(); allpass2R[i].mute();
    }
}

void FV3_(nrevb)::setcombfeedback(fv3_float_t back, long zero)
{
  FV3_(nrev)::setcombfeedback(back, zero);
  for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++)
    {
      comb2L[i].setfeedback(zero*std::pow((fv3_float_t)10.0, -3 * (fv3_float_t)comb2L[i].getsize() / back));
      comb2R[i].setfeedback(zero*std::pow((fv3_float_t)10.0, -3 * (fv3_float_t)comb2R[i].getsize() / back));
    }
}

void FV3_(nrevb)::setfeedback(fv3_float_t value)
{
  FV3_(nrev)::setfeedback(value);
  for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++)
    {
      allpass2L[i].setfeedback(value);
      allpass2R[i].setfeedback(value);
    }
}

void FV3_(nrevb)::setdamp(fv3_float_t value)
{
  FV3_(nrev)::setdamp(value);
  for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++)
    {
      comb2L[i].setdamp(value);
      comb2R[i].setdamp(value);
    }
}

void FV3_(nrevb)::processloop2(long count, fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR)
{
  fv3_float_t outL, outR, tmpL, tmpR;
  while(count-- > 0)
    {
      hpf = damp3_1*inDCC.process(*inputL + *inputR) - damp3*hpf; UNDENORMAL(hpf);
      outL = outR = tmpL = tmpR = hpf;
      
      outL += apfeedback*lastL;
      lastL += -1*apfeedback*outL;
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outL += combL[i]._process(tmpL);
      for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++) outL += comb2L[i]._process(tmpL);
      for(long i = 0;i < 3;i ++) outL = allpassL[i]._process(outL);
      for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++) outL = allpass2L[i]._process(outL);
      lpfL = damp2*lpfL + damp2_1*outL; UNDENORMAL(lpfL);
      outL = allpassL[3]._process(lpfL); outL = allpassL[5]._process(outL);
      outL = lLDCC(outL);

      outR += apfeedback*lastR;
      lastR += -1*apfeedback*outR;
      for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outR += combR[i]._process(tmpR);
      for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++) outR += comb2R[i]._process(tmpR);
      for(long i = 0;i < 3;i ++) outR = allpassR[i]._process(outR);
      for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++) outR = allpass2R[i]._process(outR);
      lpfR = damp2*lpfR + damp2_1*outR; UNDENORMAL(lpfR);
      outR = allpassR[3]._process(lpfR); outR = allpassL[6]._process(outR);
      outR = lRDCC(outR);
      
      lastL = FV3_NREVB_SCALE_WET*delayWL(lastL);
      lastR = FV3_NREVB_SCALE_WET*delayWR(lastR);
      *outputL = lastL*wet1 + lastR*wet2 + delayL(*inputL)*dry;
      *outputR = lastR*wet1 + lastL*wet2 + delayR(*inputR)*dry;
      lastL = outL; lastR = outR;
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(nrevb)::processloop4(long, fv3_float_t*, fv3_float_t*, fv3_float_t*, fv3_float_t*, fv3_float_t*, fv3_float_t*)
{
}

void FV3_(nrevb)::setFsFactors()
{
  FV3_(nrev)::setFsFactors();

  fv3_float_t totalFactor = getTotalFactorFs()/(fv3_float_t)FV3_NREV_DEFAULT_FS;
  long stereoSpread = f_((long)FV3_NREV_STEREO_SPREAD, totalFactor);
  for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++)
    {
      comb2L[i].setsize(p_(combCo2[i],totalFactor));
      comb2R[i].setsize(p_(f_(combCo2[i],totalFactor)+stereoSpread,1));
    }
  for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++)
    {
      allpass2L[i].setsize(p_(allpassCo2[i],totalFactor));
      allpass2R[i].setsize(p_(f_(allpassCo2[i],totalFactor)+stereoSpread,1));
    }
}

#include "freeverb/fv3_ns_end.h"
