/**
 *  Sampling Rate Converter
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

class _FV3_(src)
{
 public:
  _FV3_(src)();
  virtual _FV3_(~src)();
  void setSRCFactor(long factor, long converter_type);
  void setSRCFactor(long factor);
  long getSRCFactor(){ return overSamplingFactor; }
  long getConverterType(){ return src_converter; }
  long getLatency();
  void setLPFBW(_fv3_float_t bw){ lpf_iir2_bw = bw; }
  long usrc(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);
  long dsrc(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);
  void mute();
 private:
  _FV3_(src)(const _FV3_(src)& x);
  _FV3_(src)& operator=(const _FV3_(src)& x);
  void process(_SRC_(SRC_STATE) * state, _SRC_(SRC_DATA) * data);
  void freeSRC();
  long filloutSRC();
  void src_uzoh(_fv3_float_t *input, _fv3_float_t *output, long factor, long numsamples);
  void src_dzoh(_fv3_float_t *input, _fv3_float_t *output, long factor, long numsamples);
  void src_u_iir1(_fv3_float_t *input, _fv3_float_t *output, long factor, long numsamples, _FV3_(iir_1st) * iir);
  void src_d_iir1(_fv3_float_t *input, _fv3_float_t *output, long factor, long numsamples, _FV3_(iir_1st) * iir);
  void src_u_iir2(_fv3_float_t *input, _fv3_float_t *output, long factor, long numsamples, _FV3_(biquad) * iir);
  void src_d_iir2(_fv3_float_t *input, _fv3_float_t *output, long factor, long numsamples, _FV3_(biquad) * iir);
  long overSamplingFactor, src_converter, latency;
  _SRC_(SRC_STATE) *src_stateL, *src_stateR, *src_stateLV, *src_stateRV;
  _SRC_(SRC_DATA) src_dataL, src_dataR, src_dataLV, src_dataRV;
  long src_errorL, src_errorR;
  _FV3_(iir_1st) up1L, up1R, down1L, down1R;
  _FV3_(biquad) up2L, up2R, down2L, down2R;
  _fv3_float_t lpf_iir2_bw;
};
