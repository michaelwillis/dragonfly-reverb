/**
 *  Progenitor reverberator after Griesinger ca.1978.
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

#include "freeverb/progenitor.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// Fs = 34.125 kHz
const long FV3_(progenitor)::allpassLCo[] = {239, 392, 1944, 612, 1212, 121, 816, 1264,};
const long FV3_(progenitor)::allpassRCo[] = {205, 329, 2032, 368, 1452,   5, 688, 1340,};
const long FV3_(progenitor)::delayLCo[] = {1, 2, 1055, 344, 1572, 0,};
//                                        61 16    23   31    37
const long FV3_(progenitor)::delayRCo[] = {1, 625, 835, 500, 16, 1460,};
//                                        66   40   41   49  58 40+41
const long FV3_(progenitor)::allpM_EXCURSION = 32;
const long FV3_(progenitor)::idxOutCo[] = {276,   468, 625, 312,  8,   24, 36,   40,  1, 192, 1572,};
//                                         d31   {d49  d40  d31 d58}  d49 d37  {d31 d23  d49   d37}

FV3_(progenitor)::FV3_(progenitor)()
	       throw(std::bad_alloc)
{
  setrt60(2.);
  setdccutfreq(5);
  setdiffusion1(0.375);
  setdiffusion2(0.312);
  setdiffusion3(0.406);
  setdiffusion4(0.25);
  setinputdamp(20000);
  setdamp(9000);
  setoutputdamp(10000);
  setoutputdampbw(2);
  setspin(0.5);
  setspinlimit(20);
  setwander(0.5);
  setspin2wander(22);
  setspin2(2.4);
  setspinlimit2(12);
  setwander2(0.3);
  // normalized decay attenuation for 1[s] reverb
  setdecay0(0.237);
  setdecay1(0.938);
  setdecay2(0.844);
  setdecay3(0.906);
  setdecayf(1.000);
  setdamp2(500); // bass boost
  setbassbw(2);
  setbassboost(0.1);
}

void FV3_(progenitor)::mute()
{
  FV3_(revbase)::mute();
  dccutL.mute(), dccutR.mute();
  lpfL_in_59_60.mute(), lpfR_in_64_65.mute(), lpfLdamp_11_12.mute(), lpfRdamp_13_14.mute(),
    lpfL_9_10.mute(), lpfR_7_8.mute(), out1_lpf.mute(), out2_lpf.mute();
  /* delayL_61.mute(), */ delayL_16.mute(), delayL_23.mute(), delayL_31.mute(), delayL_37.mute();
  /* delayR_66.mute(), */ delayR_49.mute(), delayR_ts.mute(), delayR_40.mute(), delayR_41.mute(), delayR_58.mute();
  allpassmL_15_16.mute(), allpassmL_17_18.mute(), allpassmR_19_20.mute(), allpassmR_21_22.mute();
  allpass2L_25_27.mute(), allpass2R_43_45.mute();
  allpass3L_34_37.mute(), allpass3R_52_55.mute();
  lfo1.mute(), lfo1_lpf.mute(), lfo2.mute(), lfo2_lpf.mute();
  outCombL.mute(), outCombR.mute();
}

void FV3_(progenitor)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
		    throw(std::bad_alloc)
{
  if(numsamples <= 0) return;
  long count = numsamples;
  
  fv3_float_t outL, outR;

  while(count-- > 0)
    {
      UNDENORMAL(*inputL); UNDENORMAL(*inputR);
      outL = outR = 0.0;
      
      /* input DC-cut + LPF + short delay */
      // outL = 0.5 * delayL_61(lpfL_in_59_60(0.812*dccutL(*inputL)));
      // outR = 0.5 * delayR_66(lpfR_in_64_65(0.812*dccutR(*inputR)));
      // outL = delayL_61(lpfL_in_59_60(dccutL(*inputL)));
      // outR = delayR_66(lpfR_in_64_65(dccutR(*inputR)));
      outL = lpfL_in_59_60(dccutL(*inputL));
      outR = lpfR_in_64_65(dccutR(*inputR));

      /* add allpass stereo cross loop signal */
      fv3_float_t crossR = delayR_58._getlast(), crossL = delayL_37._getlast();
      // outL += decay0 * (0.344 * crossR + 0.156 * lpfL_9_10(0.125 * crossR));
      // outR += decay0 * (0.344 * crossL + 0.156 * lpfR_7_8(0.125 * crossL));
      outL += loopdecay * (crossR + bassb * lpfL_9_10(crossR));
      outR += loopdecay * (crossL + bassb * lpfR_7_8(crossL));
      
      /* LPF damping and allpass diffusion */
      fv3_float_t lfomod = lfo1_lpf(lfo1()*wander);
      // outL = allpassmL_17_18(delayL_16(allpassmL_15_16(lpfLdamp_11_12(0.688 * outL), lfo1_lpf(lfo1.process())*wander)));
      // outR = allpassmR_21_22(delayR_ts(allpassmR_19_20(lpfRdamp_13_14(0.688 * outR), lfo2_lpf(lfo2.process())*wander)));
      outL = allpassmL_17_18.process_dc(delayL_16(allpassmL_15_16.process_dc(lpfLdamp_11_12(outL), lfomod)), lfomod*(-1.));
      outR = allpassmR_21_22.process_dc(delayR_ts(allpassmR_19_20.process_dc(lpfRdamp_13_14(outR), lfomod*(-1.))), lfomod);
      delayL_37(allpass3L_34_37._process(delayL_31(allpass2L_25_27._process(delayL_23(outL))), lfomod));
      // delayR_58(allpass3R_52_55._process(delayR_49(allpass2R_43_45._process(delayR_41(delayR_40(outR)))), lfomod*(-1.)));
      delayR_58(allpass3R_52_55._process(delayR_49(allpass2R_43_45._process(delayR_40(outR))), lfomod*(-1.)));

      // Original Output
      // The Lexicon 224 manual says that A or C is the mono output, A and C are the stereo mode L and R outputs,
      // B, D, A and C are the front left, front right, rear left and rear right outputs.
      // Aout = node27_31[276]*0.938;
      // Bout = node45_49[468]*0.438 + node40_42[625]*0.938 - node27_31[312]*0.438 + node55_58[8]*0.125
      // Cout = node45_49[24]*0.938 + node37_39a[36]*0.469
      // Dout = node27_31[40]*0.438 + node23*0.938 - node45_49[192]*0.438 + node37_39a[1572]*0.125

      // fv3_float_t Aout, Cout;
      // Aout = delayL_31._get_z(iOutC[0])*0.938;
      // Cout = delayR_49._get_z(iOutC[5])*0.938 + delayL_37._get_z(iOutC[6])*0.469;

      fv3_float_t Bout, Dout;
      Dout = delayL_23._get_z(iOutC[8])*0.938 + (delayL_31._get_z(iOutC[7]) - delayR_49._get_z(iOutC[9]))*0.438 + delayL_37._get_z(iOutC[10])*0.125;
      Bout = delayR_40._get_z(iOutC[2])*0.938 + (delayR_49._get_z(iOutC[1]) - delayL_31._get_z(iOutC[3]))*0.438 + delayR_58._get_z(iOutC[4])*0.125;

      lfomod = lfo2_lpf(lfo2()*wander2);
      outL = outCombL._process_ff(Dout, lfomod);
      outR = outCombR._process_ff(Bout, lfomod*(-1.));
      
      fv3_float_t fpL = delayWL(out1_lpf.process(outL));
      fv3_float_t fpR = delayWR(out2_lpf.process(outR));
      *outputL = fpL*wet1 + fpR*wet2 + delayL(*inputL)*dry;
      *outputR = fpR*wet1 + fpL*wet2 + delayR(*inputR)*dry;
      UNDENORMAL(*outputL); UNDENORMAL(*outputR);
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(progenitor)::setdccutfreq(fv3_float_t value)
{
  dccutfq = limFs2(value);
  dccutL.setCutOnFreq(dccutfq, getTotalSampleRate());
  dccutR.setCutOnFreq(dccutfq, getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getdccutfreq()
{
  return dccutfq;
}

void FV3_(progenitor)::setdiffusion1(fv3_float_t value)
{
  diff1 = value;
  allpassmL_15_16.setfeedback(diff1);
  allpassmR_19_20.setfeedback(diff1);
}

fv3_float_t FV3_(progenitor)::getdiffusion1()
{
  return diff1;
}

void FV3_(progenitor)::setdiffusion2(fv3_float_t value)
{
  diff2 = value;
  allpassmL_17_18.setfeedback(diff2);
  allpassmR_21_22.setfeedback(diff2);
}

fv3_float_t FV3_(progenitor)::getdiffusion2()
{
  return diff2;
}

void FV3_(progenitor)::setdiffusion3(fv3_float_t value)
{
  diff3 = value;
  allpass2L_25_27.setfeedback2(diff3);
  allpass2R_43_45.setfeedback2(diff3);
  allpass3L_34_37.setfeedback3(diff3);
  allpass3R_52_55.setfeedback3(diff3);
}

fv3_float_t FV3_(progenitor)::getdiffusion3()
{
  return diff3;
}

void FV3_(progenitor)::setdiffusion4(fv3_float_t value)
{
  diff4 = value;
  allpass2L_25_27.setfeedback1(diff4);
  allpass2R_43_45.setfeedback1(diff4);
  allpass3L_34_37.setfeedback1(diff4); allpass3L_34_37.setfeedback2(diff4);
  allpass3R_52_55.setfeedback1(diff4); allpass3R_52_55.setfeedback2(diff4);
}

fv3_float_t FV3_(progenitor)::getdiffusion4()
{
  return diff4;
}

void FV3_(progenitor)::setspin(fv3_float_t value)
{
  lfo1.setFreq((spin = limFs2(value)), getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getspin()
{
  return spin;
}

void FV3_(progenitor)::setspinlimit(fv3_float_t value)
{
  lfo1_lpf.setLPF_BW((spinlimit = limFs2(value)), getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getspinlimit()
{
  return spinlimit;
}

void FV3_(progenitor)::setwander(fv3_float_t value)
{
  if(value < 0) value = 0;
  if(value > 1) value = 1;
  wander = value;
}

fv3_float_t FV3_(progenitor)::getwander()
{
  return wander;
}

void FV3_(progenitor)::setspin2(fv3_float_t value)
{
  lfo2.setFreq((spin2 = limFs2(value)), getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getspin2()
{
  return spin2;
}


void FV3_(progenitor)::setspin2wander(fv3_float_t value)
{
  if(value < 0) value = 0;
  spin2wander = value;
  outCombL.setsize(p_(spin2wander, getTotalSampleRate()*0.001));
  outCombR.setsize(p_(spin2wander, getTotalSampleRate()*0.001));
}

fv3_float_t FV3_(progenitor)::getspin2wander()
{
  return spin2wander;
}

void FV3_(progenitor)::setspinlimit2(fv3_float_t value)
{
  lfo2_lpf.setLPF_BW((spinlimit2 = limFs2(value)), getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getspinlimit2()
{
  return spinlimit2;
}

void FV3_(progenitor)::setwander2(fv3_float_t value)
{
  if(value < 0) value = 0;
  if(value > 1) value = 1;
  wander2 = value;
}

fv3_float_t FV3_(progenitor)::getwander2()
{
  return wander2;
}

void FV3_(progenitor)::setinputdamp(fv3_float_t value)
{
  inputdamp = limFs2(value);
  lpfL_in_59_60.setLPF_BW(inputdamp, getTotalSampleRate());
  lpfR_in_64_65.setLPF_BW(inputdamp, getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getinputdamp()
{
  return inputdamp;
}

void FV3_(progenitor)::setdamp(fv3_float_t value)
{
  damp = limFs2(value);
  lpfLdamp_11_12.setLPF_BW(damp, getTotalSampleRate());
  lpfRdamp_13_14.setLPF_BW(damp, getTotalSampleRate());
}

fv3_float_t FV3_(progenitor)::getdamp()
{
  return damp;
}

void FV3_(progenitor)::setdamp2(fv3_float_t value)
{
  damp2 = limFs2(value);
  lpfL_9_10.setLPF_RBJ(damp2, bassbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
  lpfR_7_8.setLPF_RBJ (damp2, bassbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
}

fv3_float_t FV3_(progenitor)::getdamp2()
{
  return damp2;
}

void FV3_(progenitor)::setbassbw(fv3_float_t value)
{
  if(value <= 0) value = 1;
  bassbw = value;
  lpfL_9_10.setLPF_RBJ(damp2, bassbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
  lpfR_7_8.setLPF_RBJ (damp2, bassbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
}

fv3_float_t FV3_(progenitor)::getbassbw()
{
  return bassbw;
}

void FV3_(progenitor)::setoutputdamp(fv3_float_t value)
{
  if(value > getTotalSampleRate()/FV3_PROGENITOR_OLPF_LIMIT) value = getTotalSampleRate()/FV3_PROGENITOR_OLPF_LIMIT;
  outputdamp = limFs2(value);
  out1_lpf.setLPF_RBJ(outputdamp, outputdampbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
  out2_lpf.setLPF_RBJ(outputdamp, outputdampbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
}

fv3_float_t FV3_(progenitor)::getoutputdamp()
{
  return outputdamp;
}


void FV3_(progenitor)::setoutputdampbw(fv3_float_t value)
{
  if(value < 0) value = 0;
  outputdampbw = value;
  out1_lpf.setLPF_RBJ(outputdamp, outputdampbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
  out2_lpf.setLPF_RBJ(outputdamp, outputdampbw, getTotalSampleRate(), FV3_BIQUAD_RBJ_BW);
}

fv3_float_t FV3_(progenitor)::getoutputdampbw()
{
  return outputdampbw;
}

void FV3_(progenitor)::resetdecay()
{
  // since the log10(decay) factors are fixed, there are no need to multiply the OSFactor.
  // rt60 decay = 10^(-3*delay_SampleSize/(rt60_SampleSize))
  //            = 10^(-3*R1*fs*OSFactor*RSFactor/(rt60*fs*OSFactor))
  //            = 10^(-3*R1/(rt60/RSFactor))
  //      -3*R1 = log10(decayn), rt60L = rt60/RSFactor
  fv3_float_t decay_, rt60L = rt60 / getRSFactor(), rt60s = rt60 * decayf / getRSFactor();
  UNDENORMAL(rt60L); UNDENORMAL(rt60s);
  
  decay_ = std::pow((fv3_float_t)10.0, std::log10(decay0) / rt60L);
  loopdecay = decay_;
  
  decay_ = std::pow((fv3_float_t)10.0, std::log10(decay1) / rt60s);
  allpass2L_25_27.setdecay1(decay_), allpass2R_43_45.setdecay1(decay_);
  allpass3L_34_37.setdecay1(decay_), allpass3L_34_37.setdecay2(decay_);
  allpass3R_52_55.setdecay1(decay_), allpass3R_52_55.setdecay2(decay_); 

  decay_ = std::pow((fv3_float_t)10.0, std::log10(decay2) / rt60s);
  allpassmL_15_16.setdecay(decay_), allpassmR_19_20.setdecay(decay_); 
  allpass2L_25_27.setdecay2(decay_), allpass2R_43_45.setdecay2(decay_);
  allpass3L_34_37.setdecay3(decay_), allpass3R_52_55.setdecay3(decay_);

  decay_ = std::pow((fv3_float_t)10.0, std::log10(decay3) / rt60s);
  allpassmL_17_18.setdecay(decay_), allpassmR_21_22.setdecay(decay_);
}

void FV3_(progenitor)::setrt60(fv3_float_t value){ rt60 = value, resetdecay(); }
void FV3_(progenitor)::setdecay0(fv3_float_t value){ decay0 = value, resetdecay(); }
void FV3_(progenitor)::setdecay1(fv3_float_t value){ decay1 = value, resetdecay(); }
void FV3_(progenitor)::setdecay2(fv3_float_t value){ decay2 = value, resetdecay(); }
void FV3_(progenitor)::setdecay3(fv3_float_t value){ decay3 = value, resetdecay(); }
void FV3_(progenitor)::setdecayf(fv3_float_t value){ decayf = value, resetdecay(); }

void FV3_(progenitor)::setFsFactors()
{
  FV3_(revbase)::setFsFactors();

  fv3_float_t totalFactor = getTotalFactorFs()/(fv3_float_t)FV3_PROGENITOR_DEFAULT_FS;
  fv3_float_t excurFactor = getTotalSampleRate()/(fv3_float_t)FV3_PROGENITOR_DEFAULT_FS;

  allpassmL_15_16.setsize(p_(allpassLCo[0],totalFactor), p_(allpM_EXCURSION,excurFactor));
  allpassmL_17_18.setsize(p_(allpassLCo[1],totalFactor), p_(allpM_EXCURSION,excurFactor));
  allpassmR_19_20.setsize(p_(allpassRCo[0],totalFactor), p_(allpM_EXCURSION,excurFactor));
  allpassmR_21_22.setsize(p_(allpassRCo[1],totalFactor), p_(allpM_EXCURSION,excurFactor));
  
  allpass2L_25_27.setsize(p_(allpassLCo[2],totalFactor), p_(allpassLCo[3],totalFactor));
  allpass2R_43_45.setsize(p_(allpassRCo[2],totalFactor), p_(allpassRCo[3],totalFactor));
  allpass3L_34_37.setsize(p_(allpassLCo[4],totalFactor), p_(allpassLCo[5],totalFactor), p_(allpassLCo[6],totalFactor), p_(allpassLCo[7],totalFactor));
  allpass3R_52_55.setsize(p_(allpassRCo[4],totalFactor), p_(allpassRCo[5],totalFactor), p_(allpassRCo[6],totalFactor), p_(allpassRCo[7],totalFactor));
  
  // delayL_61.setsize(p_(delayLCo[0],totalFactor));
  delayL_16.setsize(p_(delayLCo[1],totalFactor));
  delayL_23.setsize(p_(delayLCo[2],totalFactor));
  delayL_31.setsize(p_(delayLCo[3],totalFactor));
  delayL_37.setsize(p_(delayLCo[4],totalFactor));
  
  // delayR_66.setsize(p_(delayRCo[0],totalFactor));
  delayR_ts.setsize(p_((long)1,totalFactor));
  delayR_40.setsize(p_(delayRCo[5],totalFactor)); // delayRCo[1]+delayRCo[2]
  // delayR_41.setsize(p_(delayRCo[2],totalFactor));
  delayR_49.setsize(p_(delayRCo[3],totalFactor));
  delayR_58.setsize(p_(delayRCo[4],totalFactor));

  for(long i = 0;i < FV3_PROGENITOR_OUT_INDEX;i ++) iOutC[i] = f_(idxOutCo[i],totalFactor);

  setrt60(getrt60());
  setdccutfreq(getdccutfreq());
  setdiffusion1(getdiffusion1());
  setdiffusion2(getdiffusion2());
  setdiffusion3(getdiffusion3());
  setdiffusion4(getdiffusion4());
  setinputdamp(getinputdamp());
  setdamp(getdamp());
  setoutputdamp(getoutputdamp());
  setoutputdampbw(getoutputdampbw());
  setspin(getspin());
  setspinlimit(getspinlimit());
  setwander(getwander());
  setspin2wander(getspin2wander());
  setspin2(getspin2());
  setspinlimit2(getspinlimit2());
  setwander2(getwander2());
  setdamp2(getdamp2());
  setbassbw(getbassbw());
}

#include "freeverb/fv3_ns_end.h"
