/**
 *  BiQuad filter
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

class _FV3_(biquad)
{
 public:
  _FV3_(biquad)();
  void printconfig();
  void mute();
  _fv3_float_t get_A1(){return a1;}
  _fv3_float_t get_A2(){return a2;}
  _fv3_float_t get_B0(){return b0;}
  _fv3_float_t get_B1(){return b1;}
  _fv3_float_t get_B2(){return b2;}
  void set_A1(_fv3_float_t v){a1=v;}
  void set_A2(_fv3_float_t v){a2=v;}
  void set_B0(_fv3_float_t v){b0=v;}
  void set_B1(_fv3_float_t v){b1=v;}
  void set_B2(_fv3_float_t v){b2=v;}
  
  void setCoefficients(_fv3_float_t _b0, _fv3_float_t _b1, _fv3_float_t _b2, _fv3_float_t _a1, _fv3_float_t _a2);
  
  void setAPF_RBJ(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);
  void setLPF_RBJ(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);
  void setHPF_RBJ(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);
  void setBSF_RBJ(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);
  void setBPF_RBJ(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);
  void setBPFP_RBJ(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);
  void setPeakEQ_RBJ(_fv3_float_t fc, _fv3_float_t gain, _fv3_float_t bw, _fv3_float_t fs);
  void setLSF_RBJ(_fv3_float_t fc, _fv3_float_t gain, _fv3_float_t slope, _fv3_float_t fs);
  void setHSF_RBJ(_fv3_float_t fc, _fv3_float_t gain, _fv3_float_t slope, _fv3_float_t fs);

  inline _fv3_float_t process(_fv3_float_t input)
  {
    return this->processd1(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){return this->process(input);}
  
  // Direct form I
  inline _fv3_float_t processd1(_fv3_float_t input)
  {
    _fv3_float_t i0 = input;
    input *= b0;
    input += b1 * i1 + b2 * i2;
    input -= a1 * o1 + a2 * o2 ;
    UNDENORMAL(input);
    i2 = i1; i1 = i0;
    o2 = o1; o1 = input;
    return input;
  }

  // Direct form II
  inline _fv3_float_t processd2(_fv3_float_t input)
  {
    input -= a1 * t1 + a2 * t2 ;
    t0 = input; input *= b0;
    input += b1 * t1 + b2 * t2;
    UNDENORMAL(input);
    t2 = t1; t1 = t0;
    return input;
  }

 private:
  _FV3_(biquad)(const _FV3_(biquad)& x);
  _FV3_(biquad)& operator=(const _FV3_(biquad)& x);
  _fv3_float_t calcAlpha(_fv3_float_t fc, _fv3_float_t bw, _fv3_float_t fs, unsigned mode);

  _fv3_float_t a1, a2, b0, b1, b2;
  _fv3_float_t i1, i2, o1, o2, t0, t1, t2;
};

class _FV3_(iir_lr4)
{
 public:
  void setLPF(_fv3_float_t fc, _fv3_float_t fs)
  { iir1.setLPF_RBJ(fc,FV3_BIQUAD_RBJ_Q_BUTTERWORTH,fs,FV3_BIQUAD_RBJ_Q);
    iir2.setLPF_RBJ(fc,FV3_BIQUAD_RBJ_Q_BUTTERWORTH,fs,FV3_BIQUAD_RBJ_Q); }
  void setHPF(_fv3_float_t fc, _fv3_float_t fs)
  { iir1.setHPF_RBJ(fc,FV3_BIQUAD_RBJ_Q_BUTTERWORTH,fs,FV3_BIQUAD_RBJ_Q);
    iir2.setHPF_RBJ(fc,FV3_BIQUAD_RBJ_Q_BUTTERWORTH,fs,FV3_BIQUAD_RBJ_Q); }

  inline _fv3_float_t process(_fv3_float_t input){ return this->processd1(input); }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process(input); }
  inline _fv3_float_t processd1(_fv3_float_t input){ return iir2(iir1(input)); }
  inline _fv3_float_t processd2(_fv3_float_t input){ return iir2.processd2(iir1.processd2(input)); }

 private:
  _FV3_(iir_lr4)(const _FV3_(iir_lr4)& x );
  _FV3_(iir_lr4)& operator=(const _FV3_(iir_lr4)& x);
  _FV3_(biquad) iir1, iir2;
};
