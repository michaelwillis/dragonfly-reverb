/**
 *  Many Types of 1st order Filters
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

class _FV3_(iir_1st)
{
 public:
  _FV3_(iir_1st)();
  void printconfig();
  void mute();
  _fv3_float_t get_A1(){return 1;}
  _fv3_float_t get_A2(){return a2;}
  _fv3_float_t get_B1(){return b1;}
  _fv3_float_t get_B2(){return b2;}
  void set_A2(_fv3_float_t v){a2=v;}
  void set_B1(_fv3_float_t v){b1=v;}
  void set_B2(_fv3_float_t v){b2=v;}
  
  void setCoefficients(_fv3_float_t _b1, _fv3_float_t _b2, _fv3_float_t _a2);

  void setLPF_BW(_fv3_float_t fc, _fv3_float_t fs);
  void setHPF_BW(_fv3_float_t fc, _fv3_float_t fs);

  void setLPF_A(_fv3_float_t fc, _fv3_float_t fs);
  void setHPF_A(_fv3_float_t fc, _fv3_float_t fs);
  void setLSF_A(_fv3_float_t f1, _fv3_float_t f2, _fv3_float_t fs);
  void setHSF_A(_fv3_float_t f1, _fv3_float_t f2, _fv3_float_t fs);
  void setHPFwLFS_A(_fv3_float_t fc, _fv3_float_t fs);
  void setLPF_C(_fv3_float_t fc, _fv3_float_t fs);
  void setHPF_C(_fv3_float_t fc, _fv3_float_t fs);

  void setPole(_fv3_float_t v);
  void setZero(_fv3_float_t v);
  void setPoleLPF(_fv3_float_t fc, _fv3_float_t fs);
  void setPoleHPF(_fv3_float_t fc, _fv3_float_t fs);
  void setZeroLPF(_fv3_float_t fc, _fv3_float_t fs);
  void setZeroHPF(_fv3_float_t fc, _fv3_float_t fs);
  
  inline _fv3_float_t process(_fv3_float_t input)
  {
    return this->processd1(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){return this->process(input);}

  // Direct form I
  inline _fv3_float_t processd1(_fv3_float_t input)
  {
    _fv3_float_t output = input * b1 + y1;
    UNDENORMAL(output);
    y1 = output * a2 + input * b2;
    UNDENORMAL(y1);
    return output;
  }
  
 private:
  _FV3_(iir_1st)(const _FV3_(iir_1st)& x);
  _FV3_(iir_1st)& operator=(const _FV3_(iir_1st)& x);
  _fv3_float_t a2, b1, b2, y1;
};

class _FV3_(iir_lr2)
{
 public:
  void setLPF(_fv3_float_t fc, _fv3_float_t fs){ iir1.setLPF_BW(fc,fs); iir2.setLPF_BW(fc,fs); }
  void setHPF(_fv3_float_t fc, _fv3_float_t fs){ iir1.setHPF_BW(fc,fs); iir2.setHPF_BW(fc,fs); }

  inline _fv3_float_t process(_fv3_float_t input){ return iir2(iir1(input)); }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process(input); }

 private:
  _FV3_(iir_lr2)(const _FV3_(iir_lr2)& x );
  _FV3_(iir_lr2)& operator=(const _FV3_(iir_lr2)& x);
  _FV3_(iir_1st) iir1, iir2;
};

class _FV3_(efilter)
{
public:
  _FV3_(efilter)();
  inline _fv3_float_t processL(_fv3_float_t input)
  {
    return lpfL.process(hpfL.process(input*(-1)));
  }
  inline _fv3_float_t processR(_fv3_float_t input)
  {
    return lpfR.process(hpfR.process(input*(-1)));
  }
  void setLPF(_fv3_float_t val);
  _fv3_float_t getLPF();
  void setHPF(_fv3_float_t val);
  _fv3_float_t getHPF();
  void mute();
  
private:
  _FV3_(efilter)(const _FV3_(efilter)& x );
  _FV3_(efilter)& operator=(const _FV3_(efilter)& x);
  _FV3_(iir_1st) lpfL, lpfR, hpfL, hpfR;
  _fv3_float_t pole, zero;
};

class _FV3_(dccut)
{
 public:
  _FV3_(dccut)();

  inline _fv3_float_t process(_fv3_float_t input)
  {
    return this->processd1(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){return this->process(input);}

  inline _fv3_float_t processp(_fv3_float_t input)
  {
    // if you want precisely normalized signals,
    // the input signals should be *(1+gain)/2.
    return this->processd1(input*(1.+gain)/2.);
  }

  // Direct form I
  inline _fv3_float_t processd1(_fv3_float_t input)
  {
    _fv3_float_t output = input;
    output -= y1; y1 = input;
    output += gain * y2; y2 = output;
    UNDENORMAL(output);
    return output;
  }
  
  void mute();
  void seta(_fv3_float_t val);
  _fv3_float_t geta();
  void setCutOnFreq(_fv3_float_t fc, _fv3_float_t fs);
  _fv3_float_t getCutOnFreq();
  _fv3_float_t getCutOnFreq(_fv3_float_t fs);

 private:
  _FV3_(dccut)(const _FV3_(dccut)& x);
  _FV3_(dccut)& operator=(const _FV3_(dccut)& x);  
  _fv3_float_t gain, y1, y2;
};

class _FV3_(lfo)
{
 public:
  _FV3_(lfo)(){ setRCount(FV3_LFO_RCOUNT); mute(); }
  inline _fv3_float_t processarc()
  {
    _fv3_float_t out = im;
    _fv3_float_t new_re = re*arc_re - im*arc_im;
    _fv3_float_t new_im = re*arc_im + im*arc_re;
    UNDENORMAL(new_re); UNDENORMAL(new_im);
    re = new_re; im = new_im;
    if(count++ > count_max)
      {
	count = 0;
	_fv3_float_t arc_r = std::sqrt(re*re + im*im);
	UNDENORMAL(arc_r);
	re /= arc_r; im /= arc_r;
      }
    LIMIT_PLUSMINUS_ONE(out);
    return out;
  }
  inline _fv3_float_t process(_fv3_float_t input){ return this->process()*input; }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process()*input; }
  inline _fv3_float_t process(){ return this->processarc(); }
  inline _fv3_float_t operator()(){ return this->processarc(); }

  void mute(){ re = 1; im = 0; count = 0; }
  void setFreq(_fv3_float_t freq, _fv3_float_t fs){setFreq(freq/fs);}
  void setFreq(_fv3_float_t fc){ s_fc = fc; _fv3_float_t theta = 2.*M_PI*fc; arc_re = std::cos(theta); arc_im = std::sin(theta); }
  void setRCount(long v){if(v>0)count_max=v;}

 private:
  _FV3_(lfo)(const _FV3_(lfo)& x);
  _FV3_(lfo)& operator=(const _FV3_(lfo)& x);
  _fv3_float_t s_fc, re, im, arc_re, arc_im;
  long count_max, count;
};

class _FV3_(ahdsr)
{
 public:
  _FV3_(ahdsr)();
  _fv3_float_t process(_fv3_float_t input);
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process(input); }
  inline _fv3_float_t process(){ return this->process(1); }
  inline _fv3_float_t operator()(){ return this->process(1); }
  void setRAHDSR(long samples, _fv3_float_t _attack, _fv3_float_t _hold, _fv3_float_t _decay, _fv3_float_t _sustain, _fv3_float_t _release);
  void setLoopMode(bool mode);
  void mute(){init();}
  void init();

 private:
  _FV3_(ahdsr)(const _FV3_(ahdsr)& x);
  _FV3_(ahdsr)& operator=(const _FV3_(ahdsr)& x);
  _fv3_float_t Attack, Hold, Decay, Sustain, Release, sustainLevel;
  long count, countTotal, attack, hold, decay, sustain, release;
  bool loopMode;
};

class _FV3_(noisegen_pink_frac)
{
 public:
  _FV3_(noisegen_pink_frac)()
  {
    setParams(FV3_NOISEGEN_PINK_FRACTAL_1_DEFAULT_HURST_CONST, FV3_NOISEGEN_PINK_FRACTAL_1_DEFAULT_BUFSIZE);
  }

  /**
   * Set the pink noise parameters.
   * @param[in] param1 The Hurst constant, between 0 and 0.9999 (fractal dimension).
   * @param[in] length The fractal noise generator size will be 2^length.
   */
  void setParams(_fv3_float_t param1, long length)
  {
    pfn1_param = param1;
    pfn1_length = (1<<length);
    if(pfn1_slot.getsize() != pfn1_length)
      {
	pfn1_slot.alloc(pfn1_length, 1);
	pfn1_count = 0;
      }
    mute();
  }
  
  void mute(){ pfn1_slot.mute(); pfn1_count = 0; }
  
  inline _fv3_float_t process()
  {
    if(pfn1_count == 0)
      {
	fractal(pfn1_slot.L, pfn1_length, pfn1_param);
	pfn1_count = pfn1_length;
      }
    pfn1_count --;
    return pfn1_slot.L[pfn1_length - (pfn1_count+1)];
  }
  inline _fv3_float_t operator()(){ return this->process(); }
  inline _fv3_float_t process(_fv3_float_t input){ return this->process()*input; }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process()*input; }
  
 private:
  _FV3_(noisegen_pink_frac)(const _FV3_(noisegen_pink_frac)& x);
  _FV3_(noisegen_pink_frac)& operator=(const _FV3_(noisegen_pink_frac)& x);
  
  /**
   * generate fractal pattern using Midpoint Displacement Method
   * v: buffer of floats to output fractal pattern to
   * N: length of v, MUST be integer power of 2 (ie 128, 256, ...)
   * H: Hurst constant, between 0 and 0.9999 (fractal dimension)
   * based on the tap-plugins
   */
  void fractal(_fv3_float_t * v, int N, _fv3_float_t H)
  {
    int l = N, k, c;
    _fv3_float_t r = 2.0 * (H*H) + 0.3;
    v[0] = 0;
    while (l > 1)
      {
	k = N / l;
	for (c = 0; c < k; c++)
	  {
	    v[c*l + l/2] = (v[c*l] + v[((c+1) * l) % N]) / 2.0
	      + 2.0 * r * ((_fv3_float_t)std::rand() - (_fv3_float_t)RAND_MAX/2.0) / (_fv3_float_t)RAND_MAX;
	    if(v[c*l + l/2] < -1.) v[c*l + l/2] = -1.;
	    if(v[c*l + l/2] >  1.) v[c*l + l/2] =  1.;
	  }
	l /= 2;
	r /= std::pow((_fv3_float_t)2., H);
      }
  }
  
  _fv3_float_t pfn1_param;
  long pfn1_length, pfn1_count;
  _FV3_(slot) pfn1_slot;
};

class _FV3_(noisegen_gaussian_white_noise_1)
{
 public:
  _FV3_(noisegen_gaussian_white_noise_1)(){ mute(); }
  void mute(){ gwn1_pass = false; gwn1_y2 = 0; }
  inline _fv3_float_t process()
  {
    // http://www.musicdsp.org/archive.php?classid=1#109
    _fv3_float_t x1, x2, w, y1;
    if(gwn1_pass) y1 = gwn1_y2;
    else 
      {
	do
	  {
	    x1 = 2.0 * ((_fv3_float_t)std::rand() / (_fv3_float_t) RAND_MAX) - 1.0;
	    x2 = 2.0 * ((_fv3_float_t)std::rand() / (_fv3_float_t) RAND_MAX) - 1.0;
	    w = x1 * x1 + x2 * x2;
	  } while(w >= 1.0f);
	w = std::sqrt (-2.0 * std::log (w) / w);
	y1 = x1 * w;
	gwn1_y2 = x2 * w;
      }
    gwn1_pass = !gwn1_pass;
    return y1;
  }
  inline _fv3_float_t operator()(){ return this->process(); }
  inline _fv3_float_t process(_fv3_float_t input){ return this->process()*input; }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process()*input; }
  
 private:
  _FV3_(noisegen_gaussian_white_noise_1)(const _FV3_(noisegen_gaussian_white_noise_1)& x);
  _FV3_(noisegen_gaussian_white_noise_1)& operator=(const _FV3_(noisegen_gaussian_white_noise_1)& x);
  _fv3_float_t gwn1_y2; bool gwn1_pass;
};

class _FV3_(noisegen_gaussian_white_noise_2)
{
 public:
  inline _fv3_float_t process()
  {
    // http://www.musicdsp.org/archive.php?classid=1#113
    _fv3_float_t R1 = (_fv3_float_t)std::rand() / (_fv3_float_t)RAND_MAX;
    _fv3_float_t R2 = (_fv3_float_t)std::rand() / (_fv3_float_t)RAND_MAX;
    return (_fv3_float_t)std::sqrt(-2.0*std::log(R1))*std::cos(2.0*M_PI*R2);
  }
  inline _fv3_float_t operator()(){ return this->process(); }
  inline _fv3_float_t process(_fv3_float_t input){ return this->process()*input; }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process()*input; }

 private:
  _FV3_(noisegen_gaussian_white_noise_2)(const _FV3_(noisegen_gaussian_white_noise_2)& x);
  _FV3_(noisegen_gaussian_white_noise_2)& operator=(const _FV3_(noisegen_gaussian_white_noise_2)& x);
};

class _FV3_(noisegen_gaussian_white_noise_3)
{
 public:
  _FV3_(noisegen_gaussian_white_noise_3)(){ setParam(FV3_NOISEGEN_GAUSSIAN_WHITE_3_DEFAULT_PRECISION); }
  void setParam(long prec)
  {
    long q = prec;
    _fv3_float_t c1 = (1 << q) - 1;
    gwn3_c2 = ((long)(c1 / 3)) + 1;
    gwn3_c3 = 1. / c1;
  }
  
  inline _fv3_float_t process()
  {
    // http://www.musicdsp.org/archive.php?classid=0#129
    // http://www.musicdsp.org/archive.php?classid=1#168
    // This algorithm (adapted from "Natur als fraktale Grafik" by Reinhard Scholl) implements a generation method for gaussian
    // distributed random numbers with mean=0 and variance=1 (standard gaussian distribution) mapped to the range of
    // -1 to +1 with the maximum at 0.
    // For only positive results you might abs() the return value. The q variable defines the precision, with q=15 the smallest
    // distance between two numbers will be 1/(2^q div 3)=1/10922 which usually gives good results.
    // Note: the random() function used is the standard random function from Delphi/Pascal that produces *linear*
    // distributed numbers from 0 to parameter-1, the equivalent C function is probably rand().
    _fv3_float_t random = ((_fv3_float_t)std::rand() / (_fv3_float_t)(RAND_MAX));
    return (2. * ((random * gwn3_c2) + (random * gwn3_c2) + (random * gwn3_c2)) - 3. * (gwn3_c2 - 1.)) * gwn3_c3;
  }
  inline _fv3_float_t operator()(){ return this->process(); }
  inline _fv3_float_t process(_fv3_float_t input){ return this->process()*input; }
  inline _fv3_float_t operator()(_fv3_float_t input){ return this->process()*input; }

 private:
  _FV3_(noisegen_gaussian_white_noise_3)(const _FV3_(noisegen_gaussian_white_noise_3)& x);
  _FV3_(noisegen_gaussian_white_noise_3)& operator=(const _FV3_(noisegen_gaussian_white_noise_3)& x);
  _fv3_float_t gwn3_c2, gwn3_c3;
};
