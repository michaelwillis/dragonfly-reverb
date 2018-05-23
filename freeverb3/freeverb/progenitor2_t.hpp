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

class _FV3_(progenitor2) : public _FV3_(progenitor)
{
public:
  _FV3_(progenitor2)() throw(std::bad_alloc);
  virtual void mute();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);
  void setidiffusion1(_fv3_float_t value);
  _fv3_float_t getidiffusion1();
  void setodiffusion1(_fv3_float_t value);
  _fv3_float_t getodiffusion1();
  void setmodulationnoise1(_fv3_float_t value);
  _fv3_float_t getmodulationnoise1();
  void setmodulationnoise2(_fv3_float_t value);
  _fv3_float_t getmodulationnoise2();
  void setcrossfeed(_fv3_float_t value);
  _fv3_float_t getcrossfeed();
  void setbassap(_fv3_float_t fc, _fv3_float_t bw);

 protected:
  _FV3_(progenitor2)(const _FV3_(progenitor2)& x);
  _FV3_(progenitor2)& operator=(const _FV3_(progenitor2)& x);
  virtual void setFsFactors();
  _fv3_float_t idiff1, modnoise1, modnoise2, odiff1, crossfeed, bassapfc, bassapbw;
  _FV3_(biquad) bassAPL, bassAPR;
  _FV3_(noisegen_pink_frac) noise1;
  _FV3_(allpassm) iAllpassL[FV3_PROGENITOR2_NUM_IALLPASS], iAllpassR[FV3_PROGENITOR2_NUM_IALLPASS];
  _FV3_(allpass) iAllpassCL[FV3_PROGENITOR2_NUM_CALLPASS], iAllpassCR[FV3_PROGENITOR2_NUM_CALLPASS];
  const static long iAllpassLCo[FV3_PROGENITOR2_NUM_IALLPASS], iAllpassRCo[FV3_PROGENITOR2_NUM_IALLPASS],
    idxOutCo2[FV3_PROGENITOR2_OUT_INDEX], iAllpassCLCo[FV3_PROGENITOR2_NUM_CALLPASS], iAllpassCRCo[FV3_PROGENITOR2_NUM_CALLPASS];
  long iOutC2[FV3_PROGENITOR2_OUT_INDEX];
};
