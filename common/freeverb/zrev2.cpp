/**
 *  Simple FDN Reverb based on zita-rev1
 *
 *  Copyright (C) 2003-2010 Fons Adriaensen <fons@linuxaudio.org>
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

#include "freeverb/zrev2.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// Fs = 34.125 kHz
const long FV3_(zrev2)::iAllpassLCo[] = {617, 535, 434, 347, 218, 162, 144, 122, 109, 74,};
const long FV3_(zrev2)::iAllpassRCo[] = {603, 547, 416, 364, 236, 162, 140, 131, 111, 79,};
const long FV3_(zrev2)::allpM_EXCURSION = 32;

FV3_(zrev2)::FV3_(zrev2)()
            throw(std::bad_alloc)
{
  rt60 = 2.0;
  rt60_f_low = 1.3;
  rt60_f_high = 0.3;
  rt60_xo_low = 500;
  rt60_xo_high = 3600;
  idiff1 = 0.78;
  wander_ms = 22;
  spin_fq = 2.4;
  spin_factor = 0.3;

  setFsFactors();
}

void FV3_(zrev2)::mute()
{
  FV3_(zrev)::mute();
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++){ _lsf0[i].mute(); _hsf0[i].mute(); }
  for(long i = 0;i < FV3_ZREV2_NUM_IALLPASS;i ++){ iAllpassL[i].mute(); iAllpassR[i].mute(); }
  spin1_lfo.mute(); spin1_lpf.mute(); spincombl.mute(); spincombr.mute();
}

void FV3_(zrev2)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
		 throw(std::bad_alloc)
{
  switch(reverbType)
    {
    case FV3_REVTYPE_ZREV:
      FV3_(zrev)::processreplace(inputL, inputR, outputL, outputR, numsamples);
      return;
    case FV3_REVTYPE_SELF:
    case FV3_REVTYPE_ZREV2:
    default:
      ;
    }

  if(numsamples <= 0) return;
  long count = numsamples;

  fv3_float_t outL, outR;

  while(count-- > 0)
    {
      fv3_float_t lfo1q = lfo1_lpf(lfo1()*lfofactor);
      fv3_float_t lfo2q = lfo2_lpf(lfo2()*lfofactor);
      // if(lfo1q < -1.) lfo1q = -1.; if(lfo1q > 1.) lfo1q = 1.;
      // if(lfo2q < -1.) lfo2q = -1.; if(lfo2q > 1.) lfo2q = 1.;
      fv3_float_t lfo1p = -1 * lfo1q;
      fv3_float_t lfo2p = -1 * lfo2q;

      outL = dccutL(*inputL); outR = dccutR(*inputR);

      // input diffusion
      fv3_float_t i_sign = -1;
      for(long i = 0;i < FV3_ZREV2_NUM_IALLPASS;i ++)
        {
          outL = iAllpassL[i]._process(outL, lfo1q*i_sign);
          outR = iAllpassR[i]._process(outR, lfo2p*i_sign);
          i_sign *= -1;
        }

      fv3_float_t t, x0, x1, x2, x3, x4, x5, x6, x7;
      t = outL;
      x0 = _diff1[0]._process(_lsf0[0](_hsf0[0](_delay[0]._getlast() + t)), lfo1q);
      x1 = _diff1[1]._process(_lsf0[1](_hsf0[1](_delay[1]._getlast() + t)), lfo1p);
      x2 = _diff1[2]._process(_lsf0[2](_hsf0[2](_delay[2]._getlast() - t)), lfo1q);
      x3 = _diff1[3]._process(_lsf0[3](_hsf0[3](_delay[3]._getlast() - t)), lfo1p);
      t = outR;
      x4 = _diff1[4]._process(_lsf0[4](_hsf0[4](_delay[4]._getlast() + t)), lfo2p);
      x5 = _diff1[5]._process(_lsf0[5](_hsf0[5](_delay[5]._getlast() + t)), lfo2q);
      x6 = _diff1[6]._process(_lsf0[6](_hsf0[6](_delay[6]._getlast() - t)), lfo2p);
      x7 = _diff1[7]._process(_lsf0[7](_hsf0[7](_delay[7]._getlast() - t)), lfo2q);

      t = x0 - x1; x0 += x1;  x1 = t;
      t = x2 - x3; x2 += x3;  x3 = t;
      t = x4 - x5; x4 += x5;  x5 = t;
      t = x6 - x7; x6 += x7;  x7 = t;
      t = x0 - x2; x0 += x2;  x2 = t;
      t = x1 - x3; x1 += x3;  x3 = t;
      t = x4 - x6; x4 += x6;  x6 = t;
      t = x5 - x7; x5 += x7;  x7 = t;
      t = x0 - x4; x0 += x4;  x4 = t;
      t = x1 - x5; x1 += x5;  x5 = t;
      t = x2 - x6; x2 += x6;  x6 = t;
      t = x3 - x7; x3 += x7;  x7 = t;

      _delay[0]._process(x0, lfo2q);
      _delay[1]._process(x1, lfo1q);
      _delay[2]._process(x2, lfo2p);
      _delay[3]._process(x3, lfo1p);
      _delay[4]._process(x4, lfo1p);
      _delay[5]._process(x5, lfo2q);
      _delay[6]._process(x6, lfo1p);
      _delay[7]._process(x7, lfo2q);

      outL = .2*(x0 - x1 + x2 - x3);
      outR = .2*(x4 + x5 - x6 - x7);

      fv3_float_t spinlfo = spin1_lpf(spin1_lfo()*spin_factor);
      outL = spincombl._process_ff(outL, spinlfo);
      outR = spincombr._process_ff(outR, spinlfo*-1);

      fv3_float_t fpL = delayWL(out1_lpf(out1_hpf(outL)));
      fv3_float_t fpR = delayWR(out2_lpf(out2_hpf(outR)));
      *outputL = fpL*wet1 + fpR*wet2 + delayL(*inputL)*dry;
      *outputR = fpR*wet1 + fpL*wet2 + delayR(*inputR)*dry;
      UNDENORMAL(*outputL); UNDENORMAL(*outputR);
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(zrev2)::setrt60(fv3_float_t value)
{
  rt60 = value;
  fv3_float_t gain = std::sqrt(1./(fv3_float_t)FV3_ZREV_NUM_DELAYS);
  fv3_float_t back = rt60 * getTotalSampleRate();
  if(rt60 <= 0){ gain = 0; back = 1; }
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++)
    {
      _delay[i].setfeedback(gain*std::pow((fv3_float_t)10, (fv3_float_t)-3. * (fv3_float_t)(_delay[i].getsize() + _diff1[i].getsize()) / back));
      _lsf0[i].setLSF_RBJ(rt60_xo_low,
			  FV3_(utils)::R2dB(std::pow((fv3_float_t)10, (fv3_float_t)-3. * (fv3_float_t)(_delay[i].getsize() + _diff1[i].getsize())
							  / back / rt60_f_low * (1 - rt60_f_low))),
			  1, getTotalSampleRate());
      _hsf0[i].setHSF_RBJ(rt60_xo_high,
			  FV3_(utils)::R2dB(std::pow((fv3_float_t)10, (fv3_float_t)-3. * (fv3_float_t)(_delay[i].getsize() + _diff1[i].getsize())
							  / back / rt60_f_high * (1 - rt60_f_high))),
			  1, getTotalSampleRate());
    }
}

void FV3_(zrev2)::setrt60_factor_low(fv3_float_t gain)
{
  rt60_f_low = gain;
  setrt60(getrt60());
}

fv3_float_t FV3_(zrev2)::getrt60_factor_low() const
{
  return rt60_f_low;
}

void FV3_(zrev2)::setrt60_factor_high(fv3_float_t gain)
{
  rt60_f_high = gain;
  setrt60(getrt60());
}

fv3_float_t FV3_(zrev2)::getrt60_factor_high() const
{
  return rt60_f_high;
}

void FV3_(zrev2)::setloopdamp(fv3_float_t value)
{
  setxover_high(value);
}

void FV3_(zrev2)::setxover_low(fv3_float_t fc)
{
  rt60_xo_low = limFs2(fc);
  setrt60(getrt60());
}

fv3_float_t FV3_(zrev2)::getxover_low() const
{
  return rt60_xo_low;
}

void FV3_(zrev2)::setxover_high(fv3_float_t fc)
{
  FV3_(zrev)::setloopdamp(fc);
  rt60_xo_high = limFs2(fc);
  setrt60(getrt60());
}

fv3_float_t FV3_(zrev2)::getxover_high() const
{
  return rt60_xo_high;
}

void FV3_(zrev2)::setidiffusion1(fv3_float_t value)
{
  idiff1 = value;
  for(long i = 0;i < FV3_ZREV2_NUM_IALLPASS;i ++)
    {
      iAllpassL[i].setfeedback(-1.*idiff1);
      iAllpassR[i].setfeedback(-1.*idiff1);
    }
}

fv3_float_t FV3_(zrev2)::getidiffusion1() const
{
  return idiff1;
}

void FV3_(zrev2)::setwander(fv3_float_t ms)
{
  if(ms < 0) ms = 0;
  wander_ms = ms;
  spincombl.setsize(p_(wander_ms, getTotalSampleRate()*0.001));
  spincombr.setsize(p_(wander_ms, getTotalSampleRate()*0.001));
}

fv3_float_t FV3_(zrev2)::getwander() const
{
  return wander_ms;
}

void FV3_(zrev2)::setspin(fv3_float_t fq)
{
  spin1_lfo.setFreq((spin_fq = limFs2(fq)), getTotalSampleRate());
  spin1_lpf.setLPF_BW(spin_fq, getTotalSampleRate());
}

fv3_float_t FV3_(zrev2)::getspin() const { return spin_fq; }

void FV3_(zrev2)::setspinfactor(fv3_float_t value){ spin_factor = value; }

fv3_float_t FV3_(zrev2)::getspinfactor(){ return spin_factor; }

void FV3_(zrev2)::setFsFactors()
{
  FV3_(zrev)::setFsFactors();
  fv3_float_t totalFactor = getTotalFactorFs()/(fv3_float_t)FV3_ZREV2_ALLPASS_FS;
  fv3_float_t excurFactor = getTotalSampleRate()/(fv3_float_t)FV3_ZREV2_ALLPASS_FS;

  for(long i = 0;i < FV3_ZREV2_NUM_IALLPASS;i ++)
    {
      iAllpassL[i].setsize(p_(iAllpassLCo[i],totalFactor), p_(allpM_EXCURSION/3,excurFactor));
      iAllpassR[i].setsize(p_(iAllpassRCo[i],totalFactor), p_(allpM_EXCURSION/3,excurFactor));
    }

  setrt60(getrt60());
  setxover_low(getxover_low());
  setxover_high(getxover_high());
  setidiffusion1(getidiffusion1());
  setwander(getwander());
  setspin(getspin());
}

#include "freeverb/fv3_ns_end.h"
