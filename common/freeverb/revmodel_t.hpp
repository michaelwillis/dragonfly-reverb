/**
 *  Enhanced Freeverb
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

class _FV3_(revmodel) : public _FV3_(revbase)
{
public:
  _FV3_(revmodel)() throw(std::bad_alloc);
  virtual void mute();
  void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);
  void setroomsize(_fv3_float_t value);
  _fv3_float_t	getroomsize();
  void setdamp(_fv3_float_t value);
  _fv3_float_t	getdamp();
  virtual void setwet(_fv3_float_t value);
  virtual _fv3_float_t	getwet();
  virtual void setdry(_fv3_float_t value);
  virtual _fv3_float_t	getdry();
  virtual void printconfig();

 private:
  _FV3_(revmodel)(const _FV3_(revmodel)& x);
  _FV3_(revmodel)& operator=(const _FV3_(revmodel)& x);
  virtual void setFsFactors();
  void setAllpassFeedback(_fv3_float_t fb);
  _fv3_float_t roomsize, damp;
  _FV3_(allpass) allpassL[FV3_FREV_NUM_ALLPASS], allpassR[FV3_FREV_NUM_ALLPASS];
  _FV3_(comb) combL[FV3_FREV_NUM_COMB], combR[FV3_FREV_NUM_COMB];
  const static long combCo[FV3_FREV_NUM_COMB], allpCo[FV3_FREV_NUM_ALLPASS];
};
