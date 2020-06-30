/**
 *  Tank Loop Reverb based on the Progenitor Reverberator
 *
 *  Copyright (C) 1977-1978 David Griesinger
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

#include "freeverb/progenitor2.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// Fs = 34.125 kHz
const long FV3_(progenitor2)::iAllpassLCo[] = {617, 535, 434, 347, 218, 162, 144, 122, 109, 74,};
const long FV3_(progenitor2)::iAllpassRCo[] = {603, 547, 416, 364, 236, 162, 140, 131, 111, 79,};
const long FV3_(progenitor2)::iAllpassCLCo[] = {430, 341, 264, 174,};
const long FV3_(progenitor2)::iAllpassCRCo[] = {447, 324, 247, 191,};
const long FV3_(progenitor2)::idxOutCo2[] = {110, 790, 2, 189, 121, 10, 480, 359, 103, 30, 26, 10, 780, 109, 1200, 1310, 780, 10, 310, 800,};

FV3_(progenitor2)::FV3_(progenitor2)()
	       throw(std::bad_alloc)
{
  setidiffusion1(0.78);
  setodiffusion1(0.78);
  setmodulationnoise1(0.09);
  setmodulationnoise2(0.06);
  setcrossfeed(0.4);
  setbassap(150, 4);
}

void FV3_(progenitor2)::mute()
{
  FV3_(progenitor)::mute();
  bassAPL.mute(), bassAPR.mute();
  noise1.mute();
  for(long i = 0;i < FV3_PROGENITOR2_NUM_IALLPASS;i ++)
    {
      iAllpassL[i].mute(); iAllpassR[i].mute();
    }  
  for(long i = 0;i < FV3_PROGENITOR2_NUM_CALLPASS;i ++)
    {
      iAllpassCL[i].mute(); iAllpassCR[i].mute();
    }
}

void FV3_(progenitor2)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
		       throw(std::bad_alloc)
{
  switch(reverbType)
    {
    case FV3_REVTYPE_PROG:
      FV3_(progenitor)::processreplace(inputL, inputR, outputL, outputR, numsamples);
      return;
    case FV3_REVTYPE_SELF:
    case FV3_REVTYPE_PROG2:
    default:
      ;
    }
  
  if(numsamples <= 0) return;
  long count = numsamples;

  fv3_float_t outL, outR;

  while(count-- > 0)
    {
      outL = dccutL(*inputL), outR = dccutR(*inputR);
      
      fv3_float_t mnoise = noise1();
      fv3_float_t lfo = (lfo1() + modnoise1*mnoise)*wander;
      lfo = lfo1_lpf(lfo);
      // if(lfo < -1.) lfo = -1.; if(lfo > 1.) lfo = 1.;
      mnoise *= modnoise2;

      // input diffusion
      fv3_float_t i_sign = -1;
      for(long i = 0;i < FV3_PROGENITOR2_NUM_IALLPASS;i ++)
	{
	  outL = iAllpassL[i]._process(outL, lfo*i_sign, mnoise);
	  outR = iAllpassR[i]._process(outR, lfo, mnoise*i_sign);
	  i_sign *= -1;
	}
      
      fv3_float_t crossL = outL, crossR = outR;
      // input LR crossfeed diffusion
      for(long i = 0;i < FV3_PROGENITOR2_NUM_CALLPASS;i ++)
	{
	  crossL = iAllpassCL[i]._process(crossL);
	  crossR = iAllpassCR[i]._process(crossR);
	}
      
      //outL = lpfL_in_59_60(delayL_61(outL) + crossfeed * crossR);
      //outR = lpfR_in_64_65(delayR_66(outR) + crossfeed * crossL);
      outL = lpfL_in_59_60(outL + crossfeed * crossR);
      outR = lpfR_in_64_65(outR + crossfeed * crossL);

      /* add allpass stereo cross loop signal */
      crossR = delayR_58._getlast(), crossL = delayL_37._getlast();
      outL += loopdecay * (crossR + bassb * lpfL_9_10(bassAPL(crossR)));
      outR += loopdecay * (crossL + bassb * lpfR_7_8(bassAPR(crossL)));
            
      /* LPF damping and allpass diffusion */
      outL = allpassmL_17_18._process_dc(delayL_16._process(allpassmL_15_16._process_dc(lpfLdamp_11_12(outL), lfo, mnoise)), lfo*(-1.), mnoise*(-1.));
      outR = allpassmR_21_22._process_dc(delayR_ts._process(allpassmR_19_20._process_dc(lpfRdamp_13_14(outR), lfo*(-1.), mnoise*(-1.))), lfo, mnoise);
      
      delayL_37._process(allpass3L_34_37._process(delayL_31._process(allpass2L_25_27._process(delayL_23._process(outL))), lfo));
      // delayR_58._process(allpass3R_52_55._process(delayR_49._process(allpass2R_43_45._process(delayR_41._process(delayR_40._process(outR)))), lfo*(-1.)));
      delayR_58._process(allpass3R_52_55._process(delayR_49._process(allpass2R_43_45._process(delayR_40._process(outR))), lfo*(-1.)));

      fv3_float_t Bout = 0, Dout = 0;
      // node23           *  0.938
      // node27_31[40]    *  0.438
      // node45_49[192]   * -0.438
      // node27_31[276]   *  0.438
      // node40_42[110]   * -0.438
      // node45_49[2]     * -0.438
      // node37_39a[1572] *  0.125
      // node25_26[121]   *  0.125
      // node26_27[480]   *  0.125
      // node44_45[103]   * -0.125
      // node33_34[26]    *  0.125
      // node35_36[780]   *  0.125
      // node36_37[1200]  *  0.125
      // node53_54[310]   * -0.125
      // node37_39a[780]  *  0.090
      Dout = delayL_23._get_z(iOutC[8]) * 0.469
	+ (delayL_31._get_z(iOutC[7]) - delayR_49._get_z(iOutC[9]) + delayL_31._get_z(iOutC[0])
	   - delayR_40._get_z(iOutC2[0]) - delayR_49._get_z(iOutC[1])) * 0.219
	+ (delayL_37._get_z(iOutC[10]) + allpass2L_25_27._get_z1(iOutC2[4]) + allpass2L_25_27._get_z2(iOutC2[6]) - allpass2R_43_45._get_z2(iOutC2[8])
	   + allpass3L_34_37._get_z1(iOutC2[10]) + allpass3L_34_37._get_z2(iOutC2[12]) + allpass3L_34_37._get_z3(iOutC2[14])
	   - allpass3R_52_55._get_z2(iOutC2[18]))*0.064 + delayL_37._get_z(iOutC2[16])*0.045;
      
      // node40_42[625]   *  0.938
      // node45_49[468]   *  0.438
      // node27_31[312]   * -0.438
      // node45_49[24]    *  0.438
      // node37_39a[36]   * -0.438
      // node23[790]      * -0.438
      // node27_31[189]   * -0.438
      // node55_58[8]     *  0.125
      // node43_44[10]    *  0.125
      // node44_45[359]   *  0.125
      // node26_27[30]    * -0.125
      // node51_52[10]    *  0.125
      // node53_54[109]   *  0.125
      // node54_55[1310]  *  0.125
      // node35_36[800]   * -0.125
      // node55_58[10]    *  0.090
      Bout = delayR_40._get_z(iOutC[2])*0.469
	+ (delayR_49._get_z(iOutC[1]) - delayL_31._get_z(iOutC[3]) + delayR_49._get_z(iOutC[5]) - delayL_37._get_z(iOutC[6])
	   - delayL_23._get_z(iOutC2[1]) - delayL_31._get_z(iOutC2[3]))*0.219
	+ (delayR_58._get_z(iOutC[4]) + allpass2R_43_45._get_z1(iOutC2[5]) + allpass2R_43_45._get_z2(iOutC2[7]) - allpass2L_25_27._get_z2(iOutC2[9])
	   + allpass3R_52_55._get_z1(iOutC2[11]) + allpass3R_52_55._get_z2(iOutC2[13]) + allpass3R_52_55._get_z3(iOutC2[15])
	   - allpass3L_34_37._get_z2(iOutC2[19]))*0.064 + delayR_58._get_z(iOutC2[17])*0.045;
      
      lfo = lfo2_lpf(lfo2()*wander2);
      outL = outCombL._process_ff(Dout, lfo);
      outR = outCombR._process_ff(Bout, lfo*(-1.));
      
      fv3_float_t fpL = delayWL(out1_lpf.process(outL));
      fv3_float_t fpR = delayWR(out2_lpf.process(outR));
      *outputL = fpL*wet1 + fpR*wet2 + delayL(*inputL)*dry;
      *outputR = fpR*wet1 + fpL*wet2 + delayR(*inputR)*dry;
      UNDENORMAL(*outputL); UNDENORMAL(*outputR);
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(progenitor2)::setidiffusion1(fv3_float_t value)
{
  idiff1 = value;
  for(long i = 0;i < FV3_PROGENITOR2_NUM_IALLPASS;i ++)
    {
      iAllpassL[i].setfeedback(-1.*idiff1);
      iAllpassR[i].setfeedback(-1.*idiff1);
    }
}

fv3_float_t FV3_(progenitor2)::getidiffusion1()
{
  return idiff1;
}


void FV3_(progenitor2)::setodiffusion1(fv3_float_t value)
{
  odiff1 = value;
  for(long i = 0;i < FV3_PROGENITOR2_NUM_CALLPASS;i ++)
    {
      iAllpassCL[i].setfeedback(odiff1);
      iAllpassCR[i].setfeedback(odiff1);
    }
}

fv3_float_t FV3_(progenitor2)::getodiffusion1()
{
  return odiff1;
}

void FV3_(progenitor2)::setmodulationnoise1(fv3_float_t value)
{
  modnoise1 = value;
}

fv3_float_t FV3_(progenitor2)::getmodulationnoise1()
{
  return modnoise1;
}

void FV3_(progenitor2)::setmodulationnoise2(fv3_float_t value)
{
  modnoise2 = value;
}

fv3_float_t FV3_(progenitor2)::getmodulationnoise2()
{
  return modnoise2;
}

void FV3_(progenitor2)::setcrossfeed(fv3_float_t value)
{
  crossfeed = value;
}

fv3_float_t FV3_(progenitor2)::getcrossfeed()
{
  return crossfeed;
}

void FV3_(progenitor2)::setbassap(fv3_float_t fc, fv3_float_t bw)
{
  bassapfc = fc, bassapbw = bw;
  bassAPL.setAPF_RBJ(bassapfc, bassapbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
  bassAPR.setAPF_RBJ(bassapfc, bassapbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
}

void FV3_(progenitor2)::setFsFactors()
{
  FV3_(progenitor)::setFsFactors();
  
  fv3_float_t totalFactor = getTotalFactorFs()/(fv3_float_t)FV3_PROGENITOR_DEFAULT_FS;
  fv3_float_t excurFactor = getTotalSampleRate()/(fv3_float_t)FV3_PROGENITOR_DEFAULT_FS;

  for(long i = 0;i < FV3_PROGENITOR2_NUM_IALLPASS;i ++)
    {
      iAllpassL[i].setsize(p_(iAllpassLCo[i],totalFactor), p_(allpM_EXCURSION/3,excurFactor));
      iAllpassR[i].setsize(p_(iAllpassRCo[i],totalFactor), p_(allpM_EXCURSION/3,excurFactor));
    }

  for(long i = 0;i < FV3_PROGENITOR2_OUT_INDEX;i ++)
    {
      iOutC2[i] = f_(idxOutCo2[i],totalFactor);
    }

  for(long i = 0;i < FV3_PROGENITOR2_NUM_CALLPASS;i ++)
    {
      iAllpassCL[i].setsize(p_(iAllpassCLCo[i],totalFactor));
      iAllpassCR[i].setsize(p_(iAllpassCRCo[i],totalFactor));
    }

  setidiffusion1(getidiffusion1());
  setodiffusion1(getodiffusion1());
  setbassap(bassapfc, bassapbw);
}

#include "freeverb/fv3_ns_end.h"
