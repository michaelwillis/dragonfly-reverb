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

#include "freeverb/zrev.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// [s] ~70[m]
const fv3_float_t FV3_(zrev)::delayLengthReal[] = { .153129, .210389, .127837, .256891, .174713, .192303, .125000, .219991, };
const fv3_float_t FV3_(zrev)::delayLengthDiff[] = { .020346, .024421, .031604, .027333, .022904, .029291, .013458, .019123, };
const fv3_float_t FV3_(zrev)::delay_EXCURSION = 0.001;

FV3_(zrev)::FV3_(zrev)()
	    throw(std::bad_alloc)
{
  rt60 = 2.0;
  apfeedback = 0.6;
  loopdamp = 3600;
  outputlpf = 10000;
  outputhpf = 4;
  dccutfq = 2.5;
  lfo1freq = 0.9;
  lfo2freq = 1.3;
  lfofactor = 0.31;
  setFsFactors();
}

void FV3_(zrev)::mute()
{
  FV3_(revbase)::mute();
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++){ _diff1[i].mute(); _delay[i].mute(); _filt1[i].mute(); }
  lfo1.mute(); lfo2.mute(); lfo1_lpf.mute(); lfo2_lpf.mute();
  dccutL.mute(), dccutR.mute(); out1_lpf.mute(); out2_lpf.mute(); out1_hpf.mute(); out2_hpf.mute();
}

void FV3_(zrev)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
		    throw(std::bad_alloc)
{
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

      fv3_float_t t, x0, x1, x2, x3, x4, x5, x6, x7;
      t = dccutL(*inputL);
      x0 = _diff1[0]._process(_delay[0]._getlast() + t, lfo1q);
      x1 = _diff1[1]._process(_delay[1]._getlast() + t, lfo1p);
      x2 = _diff1[2]._process(_delay[2]._getlast() - t, lfo1q);
      x3 = _diff1[3]._process(_delay[3]._getlast() - t, lfo1p);
      t = dccutR(*inputR);
      x4 = _diff1[4]._process(_delay[4]._getlast() + t, lfo2p);
      x5 = _diff1[5]._process(_delay[5]._getlast() + t, lfo2q);
      x6 = _diff1[6]._process(_delay[6]._getlast() - t, lfo2p);
      x7 = _diff1[7]._process(_delay[7]._getlast() - t, lfo2q);

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

      _delay[0]._process(_filt1[0](x0), lfo2q);
      _delay[1]._process(_filt1[1](x1), lfo1q);
      _delay[2]._process(_filt1[2](x2), lfo2p);
      _delay[3]._process(_filt1[3](x3), lfo1p);
      _delay[4]._process(_filt1[4](x4), lfo1p);
      _delay[5]._process(_filt1[5](x5), lfo2q);
      _delay[6]._process(_filt1[6](x6), lfo1p);
      _delay[7]._process(_filt1[7](x7), lfo2p);

      outL = 0.3*(x1 + x2);
      outR = 0.3*(x1 - x2);
      // Original Ambisonic 4ch output
      // q0 [i] = _g0 * x0;
      // q1 [i] = _g1 * x1;
      // q2 [i] = _g1 * x4;
      // q3 [i] = _g1 * x2;
      // Original Stereo output
      // q0 [i] = _g1 * (x1 + x2);
      // q1 [i] = _g1 * (x1 - x2);

      fv3_float_t fpL = delayWL(out1_lpf(out1_hpf(outL)));
      fv3_float_t fpR = delayWR(out2_lpf(out2_hpf(outR)));
      *outputL = fpL*wet1 + fpR*wet2 + delayL(*inputL)*dry;
      *outputR = fpR*wet1 + fpL*wet2 + delayR(*inputR)*dry;
      UNDENORMAL(*outputL); UNDENORMAL(*outputR);
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(zrev)::setrt60(fv3_float_t value)
{
  rt60 = value;
  fv3_float_t gain = std::sqrt(1./(fv3_float_t)FV3_ZREV_NUM_DELAYS);
  fv3_float_t back = rt60 * getTotalSampleRate();
  if(rt60 <= 0){ gain = 0; back = 1; }
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++)
    {
      _delay[i].setfeedback(gain*std::pow((fv3_float_t)10, (fv3_float_t)-3. * (fv3_float_t)(_delay[i].getsize() + _diff1[i].getsize()) / back));
    }
}

fv3_float_t FV3_(zrev)::getrt60() const
{
  return rt60;
}

void FV3_(zrev)::setapfeedback(fv3_float_t value)
{
  fv3_float_t rev = 1;
  apfeedback = value;
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++)
    {
      _diff1[i].setfeedback(rev*value);
      rev *= -1;
    }
}

fv3_float_t FV3_(zrev)::getapfeedback()
{
  return apfeedback;
}

void FV3_(zrev)::setloopdamp(fv3_float_t value)
{
  loopdamp = limFs2(value);
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++) _filt1[i].setLPF_BW(loopdamp, getTotalSampleRate());
}

fv3_float_t FV3_(zrev)::getloopdamp()
{
  return loopdamp;
}

void FV3_(zrev)::setoutputlpf(fv3_float_t value)
{
  outputlpf = limFs2(value);
  out1_lpf.setLPF_BW(outputlpf, getTotalSampleRate());
  out2_lpf.setLPF_BW(outputlpf, getTotalSampleRate());
}

fv3_float_t FV3_(zrev)::getoutputlpf() const
{
  return outputlpf;
}

void FV3_(zrev)::setoutputhpf(fv3_float_t value)
{
  outputhpf = limFs2(value);
  out1_hpf.setHPF_BW(outputhpf, getTotalSampleRate());
  out2_hpf.setHPF_BW(outputhpf, getTotalSampleRate());
}

fv3_float_t FV3_(zrev)::getoutputhpf() const
{
  return outputhpf;
}

void FV3_(zrev)::setdccutfreq(fv3_float_t value)
{
  dccutfq = limFs2(value);
  dccutL.setCutOnFreq(dccutfq, getTotalSampleRate());
  dccutR.setCutOnFreq(dccutfq, getTotalSampleRate());
}

fv3_float_t FV3_(zrev)::getdccutfreq()
{
  return dccutfq;
}

void FV3_(zrev)::setlfo1freq(fv3_float_t fq)
{
  lfo1.setFreq((lfo1freq = limFs2(fq)), getTotalSampleRate());
  lfo1_lpf.setLPF_BW(lfo1freq, getTotalSampleRate());
}

fv3_float_t FV3_(zrev)::getlfo1freq(){ return lfo1freq; }

void FV3_(zrev)::setlfo2freq(fv3_float_t fq)
{
  lfo2.setFreq((lfo2freq = limFs2(fq)), getTotalSampleRate());
  lfo2_lpf.setLPF_BW(lfo2freq, getTotalSampleRate());
}

fv3_float_t FV3_(zrev)::getlfo2freq(){ return lfo2freq; }

void FV3_(zrev)::setlfofactor(fv3_float_t value){ lfofactor = value; }

fv3_float_t FV3_(zrev)::getlfofactor(){ return lfofactor; }

void FV3_(zrev)::setFsFactors()
{
  FV3_(revbase)::setFsFactors();
  const fv3_float_t *Total = delayLengthReal, *Diff = delayLengthDiff;
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++) _delay[i].setsize(p_(Total[i]-Diff[i],getTotalFactorFs()), f_(delay_EXCURSION,getTotalSampleRate()));
  for(long i = 0;i < FV3_ZREV_NUM_DELAYS;i ++) _diff1[i].setsize(p_(Diff[i],getTotalFactorFs()), f_(delay_EXCURSION,getTotalSampleRate()));
  setrt60(getrt60());
  setapfeedback(getapfeedback());
  setloopdamp(getloopdamp());
  setoutputlpf(getoutputlpf());
  setoutputhpf(getoutputhpf());
  setdccutfreq(getdccutfreq());
  setlfo1freq(getlfo1freq());
  setlfo2freq(getlfo2freq());
}

#include "freeverb/fv3_ns_end.h"
