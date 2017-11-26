/**
 *  Simple FDN Reverb based on zita-rev1
 *
 *  Copyright (C) 2003-2010 Fons Adriaensen <fons@linuxaudio.org>
 *  Copyright (C) 2006-2014 Teru Kamogashira
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

class _FV3_(zrev) : public _FV3_(revbase)
{
public:
  _FV3_(zrev)() throw(std::bad_alloc);

  virtual void mute();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);

  virtual void setrt60(_fv3_float_t value);
  _fv3_float_t getrt60() const;
  void setapfeedback(_fv3_float_t value);
  _fv3_float_t getapfeedback();
  virtual void setloopdamp(_fv3_float_t value);
  _fv3_float_t getloopdamp();
  void setoutputlpf(_fv3_float_t value);
  _fv3_float_t getoutputlpf() const;
  void setoutputhpf(_fv3_float_t value);
  _fv3_float_t getoutputhpf() const;
  void setdccutfreq(_fv3_float_t value);
  _fv3_float_t getdccutfreq();

  void setlfo1freq(_fv3_float_t fq);
  _fv3_float_t getlfo1freq();
  void setlfo2freq(_fv3_float_t fq);
  _fv3_float_t getlfo2freq();
  void setlfofactor(_fv3_float_t value);
  _fv3_float_t getlfofactor();

 protected:
  _FV3_(zrev)(const _FV3_(zrev)& x);
  _FV3_(zrev)& operator=(const _FV3_(zrev)& x);
  virtual void setFsFactors();

  _fv3_float_t rt60, apfeedback, loopdamp, outputlpf, outputhpf, dccutfq;
  _FV3_(allpassm) _diff1[FV3_ZREV_NUM_DELAYS];
  _FV3_(delaym) _delay[FV3_ZREV_NUM_DELAYS];
  _FV3_(dccut) dccutL, dccutR;
  _FV3_(iir_1st) _filt1[FV3_ZREV_NUM_DELAYS], out1_lpf, out2_lpf, out1_hpf, out2_hpf;
  _fv3_float_t  lfo1freq, lfo2freq, lfofactor;
  _FV3_(lfo) lfo1, lfo2;
  _FV3_(iir_1st) lfo1_lpf, lfo2_lpf;
  const static _fv3_float_t delayLengthReal[FV3_ZREV_NUM_DELAYS], delayLengthDiff[FV3_ZREV_NUM_DELAYS];
  const static _fv3_float_t delay_EXCURSION;
};
