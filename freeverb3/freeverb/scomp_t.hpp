/**
 *  Simple Compressor
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

class _FV3_(scomp)
{
 public:
  _FV3_(scomp)();
  void setRMS(long value);
  long getRMS();
  void setAttack(_fv3_float_t value);
  _fv3_float_t getAttack();
  void setRelease(_fv3_float_t value);
  _fv3_float_t getRelease();
  void setThreshold(_fv3_float_t value);
  _fv3_float_t getThreshold();
  void setSoftKnee(_fv3_float_t dB);
  _fv3_float_t getSoftKnee();
  void setRatio(_fv3_float_t value);
  _fv3_float_t getRatio();
  inline _fv3_float_t process(_fv3_float_t input)
  {
    _fv3_float_t rmsf = Rms.process(input);
    _fv3_float_t theta = rmsf > env ? attackDelta : releaseDelta;
    env = (1.0-theta)*rmsf + theta*env;
    UNDENORMAL(env);
    if(env < 0) env = 0;
    if(env >= highClip)
      {
	return std::exp((std::log(env)-threshold_log)*r);
      }
    if(env >= lowClip)
      {
	_fv3_float_t dif = std::log(env)-threshold_log+log_soft;
	return std::exp(dif*dif*r/4.0/log_soft);
      }
    return 1;
  }
  
  _fv3_float_t getEnv();
  void mute();

 private:
  _FV3_(scomp)(const _FV3_(scomp)& x);
  _FV3_(scomp)& operator=(const _FV3_(scomp)& x);
  void update();
  _fv3_float_t Attack, Release, Threshold, threshold_log;
  _fv3_float_t attackDelta, releaseDelta, env, Ratio, r;
  _fv3_float_t SoftKnee, log_soft, lowClip, highClip;
  _FV3_(rms) Rms;
};
