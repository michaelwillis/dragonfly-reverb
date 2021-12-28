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

class _FV3_(progenitor) : public _FV3_(revbase)
{
public:
  _FV3_(progenitor)() throw(std::bad_alloc);
  virtual void mute();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);
  
  /**
   * set the RT60 of reverb.
   * @param[in] RT60 in seconds. The value under 0.2 will produce unstable output.
   */
  void setrt60(_fv3_float_t value);
  _fv3_float_t getrt60(){return rt60;}

  /**
   * set the cut on frequency of the input signals' DC cut filter.
   * @param[in] value actual freqneucy.
   */
  void setdccutfreq(_fv3_float_t value);
  _fv3_float_t getdccutfreq();
  
  void setdiffusion1(_fv3_float_t value);
  void setdiffusion2(_fv3_float_t value);
  void setdiffusion3(_fv3_float_t value);
  void setdiffusion4(_fv3_float_t value);
  _fv3_float_t getdiffusion1();
  _fv3_float_t getdiffusion2();
  _fv3_float_t getdiffusion3();
  _fv3_float_t getdiffusion4();
  void setinputdamp(_fv3_float_t value);
  _fv3_float_t getinputdamp();
  void setdamp(_fv3_float_t value);
  _fv3_float_t getdamp();
  void setoutputdamp(_fv3_float_t value);
  _fv3_float_t getoutputdamp();
  void setoutputdampbw(_fv3_float_t value);
  _fv3_float_t getoutputdampbw();

  /**
   * set the modulation frequency of the LFO of the modulated allpass filter.
   * @param[in] value LFO modulation freqneucy.
   */
  void setspin(_fv3_float_t value);
  _fv3_float_t getspin();

  /**
   * set the modulation frequency limiter's frequency.
   * @param[in] value max LFO modulation freqneucy.
   */
  void setspinlimit(_fv3_float_t value);
  _fv3_float_t getspinlimit();

  /**
   * set the modulation strength of the modulated allpass filter.
   * the max modulation duration is 64/ *1000[ms]
   * @param[in] value strength of the LFO.
   */
  void setwander(_fv3_float_t value);
  _fv3_float_t getwander();

  /**
   * set the comb filter length for the modulation.
   * @param[in] value The comb filter length in ms.
   */
  void setspin2wander(_fv3_float_t value);
  _fv3_float_t getspin2wander();

  /**
   * set the modulation frequency of the strength of the filter.
   * @param[in] value LFO modulation freqneucy.
   */
  void setspin2(_fv3_float_t value);
  _fv3_float_t getspin2();

  /**
   * set the modulation frequency limiter's frequency.
   * @param[in] value max LFO modulation freqneucy.
   */
  void setspinlimit2(_fv3_float_t value);
  _fv3_float_t getspinlimit2();

  /**
   * set the modulation strength.
   * @param[in] value strength of the LFO.
   */
  void setwander2(_fv3_float_t value);
  _fv3_float_t getwander2();

  void setdecay0(_fv3_float_t value);
  void setdecay1(_fv3_float_t value);
  void setdecay2(_fv3_float_t value);
  void setdecay3(_fv3_float_t value);
  void setdecayf(_fv3_float_t value);
  _fv3_float_t getdecay0(){ return decay0; }
  _fv3_float_t getdecay1(){ return decay1; }
  _fv3_float_t getdecay2(){ return decay2; }
  _fv3_float_t getdecay3(){ return decay3; }
  _fv3_float_t getdecayf(){ return decayf; }

  void setdamp2(_fv3_float_t value);
  _fv3_float_t getdamp2();
  void setbassbw(_fv3_float_t value);
  _fv3_float_t getbassbw();
  void setbassboost(_fv3_float_t value){ bassb = value; }
  _fv3_float_t getbassboost(){ return bassb; }

 protected:
  _FV3_(progenitor)(const _FV3_(progenitor)& x);
  _FV3_(progenitor)& operator=(const _FV3_(progenitor)& x);
  void resetdecay();
  virtual void setFsFactors();
  _fv3_float_t rt60, decay0 = 0.237, dccutfq, inputdamp, damp, damp2, outputdamp, outputdampbw = 2.0, decay1 = 0.938, decay2 = 0.844, decay3 = 0.906, decayf = 1.0;
  _fv3_float_t diff1, diff2, diff3, diff4, spin, spinlimit, wander, spin2, spin2wander, spinlimit2, wander2, bassbw = 2.0, bassb, loopdecay;
  
  _FV3_(dccut) dccutL, dccutR;
  _FV3_(iir_1st) lpfL_in_59_60, lpfR_in_64_65, lpfLdamp_11_12, lpfRdamp_13_14;
  _FV3_(biquad) lpfL_9_10, lpfR_7_8, out1_lpf, out2_lpf;
  _FV3_(delay) /* delayL_61, */ delayL_16, delayL_23, delayL_31, delayL_37;
  _FV3_(delay) /* delayR_66, */ delayR_49, delayR_ts, delayR_40, delayR_41, delayR_58;

  _FV3_(allpassm) allpassmL_15_16, allpassmL_17_18, allpassmR_19_20, allpassmR_21_22;
  _FV3_(allpass2) allpass2L_25_27, allpass2R_43_45;
  _FV3_(allpass3) allpass3L_34_37, allpass3R_52_55;

  _FV3_(lfo) lfo1, lfo2;
  _FV3_(iir_1st) lfo1_lpf, lfo2_lpf;
  _FV3_(comb) outCombL, outCombR;

  const static long allpassLCo[FV3_PROGENITOR_NUM_ALLPASS], allpassRCo[FV3_PROGENITOR_NUM_ALLPASS],
    delayLCo[FV3_PROGENITOR_NUM_DELAY], delayRCo[FV3_PROGENITOR_NUM_DELAY], idxOutCo[FV3_PROGENITOR_OUT_INDEX];
  const static long allpM_EXCURSION;
  long iOutC[FV3_PROGENITOR_OUT_INDEX];
};
