/**
 *  Early Reflection Class
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

class _FV3_(earlyref) : public _FV3_(revbase)
{
public:
  _FV3_(earlyref)() throw(std::bad_alloc);
  virtual _FV3_(~earlyref)();

  virtual void mute();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc);

  void loadPresetReflection(long program);
  long getCurrentPreset();
  void loadUserReflection(const _fv3_float_t * delayL, const _fv3_float_t * gainL, const _fv3_float_t * delayR, const _fv3_float_t * gainR, long sizeL, long sizeR)
    throw(std::bad_alloc);
  void unloadReflection();

  void         setLRDelay(_fv3_float_t value_ms);
  _fv3_float_t getLRDelay();
  void         setLRCrossApFreq(_fv3_float_t fc, _fv3_float_t bw);
  _fv3_float_t getLRCrossApFreq();
  void         setDiffusionApFreq(_fv3_float_t fc, _fv3_float_t bw);
  _fv3_float_t getDiffusionApFreq();

  void setoutputlpf(_fv3_float_t value);
  _fv3_float_t getoutputlpf() const;
  void setoutputhpf(_fv3_float_t value);
  _fv3_float_t getoutputhpf() const;

 protected:
  _FV3_(earlyref)(const _FV3_(earlyref)& x);
  _FV3_(earlyref)& operator=(const _FV3_(earlyref)& x);
  void loadReflection(const _fv3_float_t * delayL, const _fv3_float_t * gainL, const _fv3_float_t * delayR, const _fv3_float_t * gainR, long sizeL, long sizeR)
    throw(std::bad_alloc);
  virtual void setFsFactors();

  _fv3_float_t maxDelay(const _fv3_float_t * delaySet, long size);

  _FV3_(delayline) delayLineL, delayLineR;
  _FV3_(delay) delayLtoR, delayRtoL;
  _FV3_(biquad) allpassXL, allpassL2, allpassXR, allpassR2;
  _FV3_(iir_1st) out1_lpf, out2_lpf, out1_hpf, out2_hpf;
  long currentPreset, tapLengthL, tapLengthR, lrDelay;
  _fv3_float_t lrCrossApFq, lrCrossApBw, diffApFq, diffApBw, outputlpf, outputhpf;
  _fv3_float_t *gainTableL, *gainTableR, *delayTableL, *delayTableR;

  const static long preset0_size;
  const static _fv3_float_t preset0_delayL[], preset0_delayR[], preset0_gainL[], preset0_gainR[];
  const static long preset1_size;
  const static _fv3_float_t preset1_delayL[], preset1_delayR[], preset1_gainL[], preset1_gainR[];
  const static long preset2_size;
  const static _fv3_float_t preset2_delayL[], preset2_delayR[], preset2_gainL[], preset2_gainR[];

  const static long preset11_sizeL, preset11_sizeR;
  const static _fv3_float_t preset11_delayL[], preset11_delayR[], preset11_gainL[], preset11_gainR[];
  const static long preset12_sizeL, preset12_sizeR;
  const static _fv3_float_t preset12_delayL[], preset12_delayR[], preset12_gainL[], preset12_gainR[];
  const static long preset13_sizeL, preset13_sizeR;
  const static _fv3_float_t preset13_delayL[], preset13_delayR[], preset13_gainL[], preset13_gainR[];
  const static long preset14_sizeL, preset14_sizeR;
  const static _fv3_float_t preset14_delayL[], preset14_delayR[], preset14_gainL[], preset14_gainR[];
  const static long preset15_sizeL, preset15_sizeR;
  const static _fv3_float_t preset15_delayL[], preset15_delayR[], preset15_gainL[], preset15_gainR[];
  const static long preset16_sizeL, preset16_sizeR;
  const static _fv3_float_t preset16_delayL[], preset16_delayR[], preset16_gainL[], preset16_gainR[];
  const static long preset17_sizeL, preset17_sizeR;
  const static _fv3_float_t preset17_delayL[], preset17_delayR[], preset17_gainL[], preset17_gainR[];
  const static long preset18_sizeL, preset18_sizeR;
  const static _fv3_float_t preset18_delayL[], preset18_delayR[], preset18_gainL[], preset18_gainR[];
  const static long preset19_sizeL, preset19_sizeR;
  const static _fv3_float_t preset19_delayL[], preset19_delayR[], preset19_gainL[], preset19_gainR[];
  const static long preset20_sizeL, preset20_sizeR;
  const static _fv3_float_t preset20_delayL[], preset20_delayR[], preset20_gainL[], preset20_gainR[];
  const static long preset21_sizeL, preset21_sizeR;
  const static _fv3_float_t preset21_delayL[], preset21_delayR[], preset21_gainL[], preset21_gainR[];
  const static long preset22_sizeL, preset22_sizeR;
  const static _fv3_float_t preset22_delayL[], preset22_delayR[], preset22_gainL[], preset22_gainR[];
};
