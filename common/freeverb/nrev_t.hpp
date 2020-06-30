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

class _FV3_(nrev) : public _FV3_(revbase)
{
 public:
  _FV3_(nrev)() throw(std::bad_alloc);
  virtual void mute();

  void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);

  /**
   * set the time length (T60/RT60) of the reverb.
   * @param[in] value reverb time in seconds.
   */
  void setrt60(_fv3_float_t value);
  _fv3_float_t	getrt60();

  /**
   * set the parameter of the allpass filters' parameter.
   * @param[in] value allpass parameter.
   */
  virtual void setfeedback(_fv3_float_t value);
  _fv3_float_t getfeedback();
  
  /**
   * set the parameter of the comb filters' internal LPF.
   * @param[in] value LPF parameter.
   */
  virtual void setdamp(_fv3_float_t value);
  _fv3_float_t	getdamp();

  /**
   * set the parameter of LPF after the comb/allpass filters.
   * @param[in] value LPF parameter.
   */
  void setdamp2(_fv3_float_t value);
  _fv3_float_t	getdamp2();

  /**
   * set the parameter of HPF after the input signals' DC cut filter.
   * @param[in] value HPF parameter.
   */
  void setdamp3(_fv3_float_t value);
  _fv3_float_t	getdamp3();
  
  /**
   * set the cut on frequency of the input signals' DC cut filter.
   * @param[in] value actual freqneucy.
   */
  void setdccutfreq(_fv3_float_t value);
  _fv3_float_t getdccutfreq();

  virtual void printconfig();
  
 protected:
  virtual void setcombfeedback(_fv3_float_t back, long zero);
  virtual void setFsFactors();
  _fv3_float_t roomsize, feedback, damp, damp2, damp2_1, damp3, damp3_1;
  _fv3_float_t dccutfq;
  _FV3_(allpass) allpassL[FV3_NREV_NUM_ALLPASS], allpassR[FV3_NREV_NUM_ALLPASS];
  _FV3_(comb) combL[FV3_NREV_NUM_COMB], combR[FV3_NREV_NUM_COMB];

  const static long combCo[FV3_NREV_NUM_COMB], allpassCo[FV3_NREV_NUM_ALLPASS];
  _FV3_(dccut) inDCC, lLDCC, lRDCC;
  // work values
  _fv3_float_t hpf, lpfL, lpfR;

 private:
  _FV3_(nrev)(const _FV3_(nrev)& x);
  _FV3_(nrev)& operator=(const _FV3_(nrev)& x);  
};
