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

class _FV3_(zrev2) : public _FV3_(zrev)
{
public:
  _FV3_(zrev2)() throw(std::bad_alloc);

  virtual void mute();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);

  virtual void setrt60(_fv3_float_t value);
  virtual void setloopdamp(_fv3_float_t value);

  void setrt60_factor_low(_fv3_float_t gain);
  _fv3_float_t getrt60_factor_low() const;
  void setrt60_factor_high(_fv3_float_t gain);
  _fv3_float_t getrt60_factor_high() const;


  void setxover_low(_fv3_float_t fc);
  _fv3_float_t getxover_low() const;
  void setxover_high(_fv3_float_t fc);
  _fv3_float_t getxover_high() const;

  /**
   * set the allpass diffusion parameter.
   * @param[in] value The parameter in -1~1.
   */
  void setidiffusion1(_fv3_float_t value);
  _fv3_float_t getidiffusion1() const;

  /**
   * set the comb filter length for the modulation.
   * @param[in] value The comb filter length in ms.
   */
  void setwander(_fv3_float_t ms);
  _fv3_float_t getwander() const;

  /**
   * set the modulation frequency of the strength of the comb filter.
   * @param[in] value LFO modulation freqneucy.
   */
  void setspin(_fv3_float_t fq);
  _fv3_float_t getspin() const;
  void setspinfactor(_fv3_float_t value);
  _fv3_float_t getspinfactor();

 protected:
  _FV3_(zrev2)(const _FV3_(zrev2)& x);
  _FV3_(zrev2)& operator=(const _FV3_(zrev2)& x);
  virtual void setFsFactors();
  _fv3_float_t rt60_f_low, rt60_f_high, rt60_xo_low, rt60_xo_high, idiff1, wander_ms, spin_fq, spin_factor;
  _FV3_(biquad) _lsf0[FV3_ZREV_NUM_DELAYS], _hsf0[FV3_ZREV_NUM_DELAYS];
  _FV3_(allpassm) iAllpassL[FV3_ZREV2_NUM_IALLPASS], iAllpassR[FV3_ZREV2_NUM_IALLPASS];
  _FV3_(lfo) spin1_lfo; _FV3_(iir_1st) spin1_lpf;
  const static long iAllpassLCo[FV3_ZREV2_NUM_IALLPASS], iAllpassRCo[FV3_ZREV2_NUM_IALLPASS], allpM_EXCURSION;
  _FV3_(comb) spincombl, spincombr;
};
