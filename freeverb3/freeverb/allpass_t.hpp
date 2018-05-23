/**
 *  Allpass filter
 *
 *  Copyright (C) 2000 Jezar at Dreampoint
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

/**
 * A simple 1st-order allpass filter class.
 */
class _FV3_(allpass)
{
 public:
  _FV3_(allpass)();
  _FV3_(~allpass)();
  void free();
  
  /**
   * Set delay size. This preserves previous data.
   * @param[in] size The delay size.
   */
  void setsize(long size) throw(std::bad_alloc);
  long getsize();
  void mute();
  void         setfeedback(_fv3_float_t val);
  _fv3_float_t getfeedback();
  void         setdecay(_fv3_float_t val);
  _fv3_float_t getdecay();

  /**
   * Retrive the last signal of the delayline without index error checking.
   * @return The last signal (z^(-bufsize)) of the delayline. This is the same as get_z(getsize()).
   */
  inline _fv3_float_t _getlast()
  {
    return buffer[bufidx];
  }
  
  /**
   * Retrive the signal of the delayline without index error checking.
   * @param[in] index specifies the index of the delayline.
   * @return The signal value (z^(-index)).
   */
  inline _fv3_float_t _get_z(long index)
  {
#ifdef DEBUG
    if(index > bufsize||index <= 0) std::fprintf(stderr, "allpass::_get_z(%ld,%ld)!\n", index, bufsize);
#endif
    long readpoint = bufidx - index;
    if(readpoint < 0) readpoint += bufsize;
    return buffer[readpoint];
  }

  /**
   * A simple normal allpass filter process method.
   * @param[in] input The input signal.
   */  
  inline _fv3_float_t process(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){ return process(input); }
  inline _fv3_float_t _process(_fv3_float_t input)
  {
    _fv3_float_t buffer_tmp = buffer[bufidx];
    input += feedback * buffer_tmp;
    _fv3_float_t output = buffer_tmp - feedback * input;
    UNDENORMAL(output);
    buffer[bufidx] = input;
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    return output;
  }

  /**
   * An allpass filter with decay.
   * @param[in] input The input signal.
   */  
  inline _fv3_float_t process_dc(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process_dc(input);
  }
  inline _fv3_float_t _process_dc(_fv3_float_t input)
  {
    _fv3_float_t buffer_tmp = buffer[bufidx];
    input += feedback * buffer_tmp;
    _fv3_float_t output = decay * buffer_tmp - feedback * input;
    UNDENORMAL(output);
    buffer[bufidx] = input;
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    return output;
  }

  /**
   * An original freeverb version of allpass filter.
   * The true allpass filter is obtained if g = (sqrt(5)-1)/2 ~ 0.618 (reciprocal of the golden ratio)
   * while that of freeverb is 0.5.
   * @param[in] input signal.
   */
  inline _fv3_float_t process_ov(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process_ov(input);
  }
  inline _fv3_float_t _process_ov(_fv3_float_t input)
  {
    _fv3_float_t bufout = buffer[bufidx];
    UNDENORMAL(bufout);
    buffer[bufidx] = input + (bufout * feedback);
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    return (bufout - input);
  }

 private:
  _FV3_(allpass)(const _FV3_(allpass)& x);
  _FV3_(allpass)& operator=(const _FV3_(allpass)& x);
  _fv3_float_t feedback, *buffer, decay;
  long bufsize, bufidx;
};

/**
 * A 1st-order allpass filter with a allpass interplated delay modulation class.
 */
class _FV3_(allpassm)
{
 public:
  _FV3_(allpassm)();
  _FV3_(~allpassm)();
  void free();

  /**
   * Set delay size. This does not preserve previous data.
   * @param[in] size The delay size.
   */
  void setsize(long size) throw(std::bad_alloc);
  void setsize(long size, long modsize) throw(std::bad_alloc);
  long getsize();
  long getdelaysize();
  long getmodulationsize();
  void mute();
  void         setfeedback(_fv3_float_t val);
  _fv3_float_t getfeedback();
  void         setdecay(_fv3_float_t val);
  _fv3_float_t getdecay();
  void         set_90degfq(_fv3_float_t fc, _fv3_float_t fs);

  inline _fv3_float_t process(_fv3_float_t input){return process(input, 0);}
  inline _fv3_float_t operator()(_fv3_float_t input){return process(input);}

  /**
   * An allpass filter with a allpass interpolated modulation without a allpass feedback modulation and a decay.
   * @param[in] input The input signal.
   * @param[in] modulation The delayline modulation difference. This must be -1~+1.
   * @return The processed signal.
   */  
  inline _fv3_float_t process(_fv3_float_t input, _fv3_float_t modulation)
  {
    if(bufsize == 0) return input;
    return _process(input, modulation);
  }
  inline _fv3_float_t operator()(_fv3_float_t input, _fv3_float_t modulation){ return process(input,modulation); }
  inline _fv3_float_t _process(_fv3_float_t input, _fv3_float_t modulation)
  {
    modulation = (modulation + 1.) * modulationsize_f;
    _fv3_float_t floor_mod = std::floor(modulation); // >= 0
    _fv3_float_t m_frac = 1. - (modulation - floor_mod); // >= 0
    
    long readidx_a = readidx - (long)floor_mod; if(readidx_a < 0) readidx_a += bufsize;
    long readidx_b = readidx_a - 1; if(readidx_b < 0) readidx_b += bufsize;

    z_1 = buffer[readidx_b] + m_frac * (buffer[readidx_a] - z_1);
    UNDENORMAL(z_1);
    readidx ++; if(readidx >= bufsize) readidx = 0;

    buffer[writeidx] = input + z_1 * feedback_mod;
    input = z_1 - buffer[writeidx] * feedback_mod;
    writeidx ++; if(writeidx >= bufsize) writeidx = 0;

    return input;
  }

  /**
   * An allpass filter with a allpass interpolated modulation and a allpass feedback modulation without a decay.
   * @param[in] input The input signal.
   * @param[in] modulation The delayline modulation difference. This must be  -1~+1.
   * @param[in] fmod The feedback modulation difference.
   * @return The processed signal.
   */
  inline _fv3_float_t process(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod)
  {
    feedback_mod = feedback + fmod;
    return process(input,modulation);
  }
  inline _fv3_float_t operator()(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod){return process(input,modulation,fmod);}

  inline _fv3_float_t _process(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod)
  {
    feedback_mod = feedback + fmod;
    return _process(input,modulation);
  }

  /**
   * An allpass filter with a allpass interpolated modulation, a allpass feedback modulation and a decay.
   * @param[in] input The input signal.
   * @param[in] modulation The delayline modulation difference. This must be  -1~+1.
   * @param[in] fmod The feedback modulation difference.
   * @return The processed signal.
   */  
  inline _fv3_float_t process_dc(_fv3_float_t input, _fv3_float_t modulation)
  {
    if(bufsize == 0) return input;
    return _process_dc(input, modulation);
  }
  inline _fv3_float_t _process_dc(_fv3_float_t input, _fv3_float_t modulation)
  {
    modulation = (modulation + 1.) * modulationsize_f;
    _fv3_float_t floor_mod = std::floor(modulation); // >= 0
    _fv3_float_t m_frac = 1. - (modulation - floor_mod); // >= 0
    
    long readidx_a = readidx - (long)floor_mod; if(readidx_a < 0) readidx_a += bufsize;
    long readidx_b = readidx_a - 1; if(readidx_b < 0) readidx_b += bufsize;
    
    z_1 = buffer[readidx_b] + m_frac * (buffer[readidx_a] - z_1);
    UNDENORMAL(z_1);
    readidx ++; if(readidx >= bufsize) readidx = 0;

    buffer[writeidx] = input + z_1 * feedback_mod;
    input = decay * z_1 - buffer[writeidx] * feedback_mod;
    writeidx ++; if(writeidx >= bufsize) writeidx = 0;
    
    return input;
  }

  inline _fv3_float_t process_dc(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod)
  {
    feedback_mod = feedback + fmod;
    return process_dc(input,modulation);
  }

  inline _fv3_float_t _process_dc(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod)
  {
    feedback_mod = feedback + fmod;
    return _process_dc(input,modulation);
  }

  /**
   * An allpass filter with a linear interpolated modulation without a allpass feedback modulation and a decay.
   * @param[in] input The input signal.
   * @param[in] modulation The delayline modulation difference. This must be -1~+1.
   * @return The processed signal.
   */  
  inline _fv3_float_t process_li(_fv3_float_t input, _fv3_float_t modulation)
  {
    if(bufsize == 0) return input;
    return _process_li(input, modulation);
  }
  inline _fv3_float_t _process_li(_fv3_float_t input, _fv3_float_t modulation)
  {
    modulation = (modulation + 1.) * modulationsize_f;
    _fv3_float_t floor_mod = std::floor(modulation); // >= 0
    _fv3_float_t frac = modulation - floor_mod; // >= 0
    
    long readidx_a = readidx - (long)floor_mod; if(readidx_a < 0) readidx_a += bufsize;
    long readidx_b = readidx_a - 1; if(readidx_b < 0) readidx_b += bufsize;

    _fv3_float_t temp = buffer[readidx_b]*frac + buffer[readidx_a]*(1.-frac);
    readidx ++; if(readidx >= bufsize) readidx = 0;

    buffer[writeidx] = input + temp * feedback_mod;
    input = temp - buffer[writeidx] * feedback_mod;
    writeidx ++; if(writeidx >= bufsize) writeidx = 0;
    
    return input;
  }

  /**
   * An allpass filter with a linear interpolated modulation and a allpass feedback modulation without a decay.
   * @param[in] input The input signal.
   * @param[in] modulation The delayline modulation difference. This must be  -1~+1.
   * @param[in] fmod The feedback modulation difference.
   * @return The processed signal.
   */
  inline _fv3_float_t process_li(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod)
  {
    feedback_mod = feedback + fmod;
    return process_li(input,modulation);
  }

  inline _fv3_float_t _process_li(_fv3_float_t input, _fv3_float_t modulation, _fv3_float_t fmod)
  {
    feedback_mod = feedback + fmod;
    return _process_li(input,modulation);
  }
 
 private:
  _FV3_(allpassm)(const _FV3_(allpassm)& x);
  _FV3_(allpassm)& operator=(const _FV3_(allpassm)& x);
  _fv3_float_t feedback, feedback_mod, *buffer, z_1, decay, modulationsize_f;
  long bufsize, readidx, writeidx, modulationsize;
};

/**
 * A simple 2nd-order allpass filter class.
 */
class _FV3_(allpass2)
{
 public:
  _FV3_(allpass2)();
  _FV3_(~allpass2)();
  void free();

  /**
   * set allpass delay size.
   * @param[in] size1 The inner allpass delay size.
   * @param[in] size2 The outer allpass delay size.
   */
  void setsize(long size1, long size2) throw(std::bad_alloc);

  /**
   * set the innter allpass feedback.
   * @param[in] val The innter allpass feedback.
   */
  void setfeedback1(_fv3_float_t val);
  
  /**
   * set the outer allpass feedback.
   * @param[in] val The outer allpass feedback.
   */
  void setfeedback2(_fv3_float_t val);
  
  /**
   * set the innter allpass decay.
   * @param[in] val The innter allpass decay.
   */
  void setdecay1(_fv3_float_t val);
  
  /**
   * set the outer allpass decay.
   * @param[in] val The outer allpass decay.
   */
  void setdecay2(_fv3_float_t val);
  
  /**
   * Retrive the last signal of the inner delayline.
   * @return The last signal (z^(-bufsize)) of the inner delayline.
   */
  inline _fv3_float_t _getlast1(){ return buffer1[bufidx1]; }

  /**
   * Retrive the last signal of the outer delayline.
   * @return The last signal (z^(-bufsize)) of the outer delayline.
   */
  inline _fv3_float_t _getlast2(){ return buffer2[bufidx2]; }

  /**
   * Retrive the signal of the innner delayline without index error checking.
   * @param[in] index specifies the index of the delayline.
   * @return The signal value (z^(-index)).
   */
  inline _fv3_float_t _get_z1(long index)
  {
#ifdef DEBUG
    if(index > bufsize1||index <= 0) std::fprintf(stderr, "allpass2::_get_z(%ld,%ld)!\n", index, bufsize1);
#endif
    long readpoint = bufidx1 - index;
    if(readpoint < 0) readpoint += bufsize1;
    return buffer1[readpoint];
  }

  /**
   * Retrive the signal of the outer delayline without index error checking.
   * @param[in] index specifies the index of the delayline.
   * @return The signal value (z^(-index)).
   */
  inline _fv3_float_t _get_z2(long index)
  {
#ifdef DEBUG
    if(index > bufsize2||index <= 0) std::fprintf(stderr, "allpass2::_get_z(%ld,%ld)!\n", index, bufsize2);
#endif
    long readpoint = bufidx2 - index;
    if(readpoint < 0) readpoint += bufsize2;
    return buffer2[readpoint];
  }

  /**
   * A 2nd order simple allpass filter with decay.
   * @param[in] input signal.
   * @return processed signal.
   */  
  inline _fv3_float_t process(_fv3_float_t input)
  {
    if(bufsize1 == 0||bufsize2 == 0) return input;
    return _process(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){ return process(input); }
  inline _fv3_float_t _process(_fv3_float_t input)
  {
    input += feedback2*buffer2[bufidx2];
    _fv3_float_t output = decay2 * buffer2[bufidx2] - input*feedback2;
    UNDENORMAL(output);

    input += feedback1*buffer1[bufidx1];
    buffer2[bufidx2] = decay1 * buffer1[bufidx1] - input*feedback1;
    UNDENORMAL(buffer2[bufidx2]);

    buffer1[bufidx1] = input;

    bufidx1 ++; if(bufidx1 >= bufsize1) bufidx1 = 0;
    bufidx2 ++; if(bufidx2 >= bufsize2) bufidx2 = 0;
    return output;
  }
  
  void mute();
 
 private:
  _FV3_(allpass2)(const _FV3_(allpass2)& x);
  _FV3_(allpass2)& operator=(const _FV3_(allpass2)& x);
  _fv3_float_t feedback1, feedback2, decay1, decay2, *buffer1, *buffer2;
  long bufsize1, bufidx1, bufsize2, bufidx2;
};

/**
 * A simple 3rd-order allpass filter.
 */
class _FV3_(allpass3)
{
 public:
  _FV3_(allpass3)();
  _FV3_(~allpass3)();
  void free();

  void setsize(long size1, long size2, long size3) throw(std::bad_alloc);
  void setsize(long size1, long size1mod, long size2, long size3) throw(std::bad_alloc);
  
  inline _fv3_float_t _getlast1(){ return buffer1[readidx1]; }
  inline _fv3_float_t _getlast2(){ return buffer2[bufidx2]; }
  inline _fv3_float_t _getlast3(){ return buffer3[bufidx3]; }

  inline _fv3_float_t _get_z1(long index)
  {
#ifdef DEBUG
    if(index > bufsize1||index <= 0) std::fprintf(stderr, "allpass3::_get_z(%ld,%ld)!\n", index, bufsize1);
#endif
    long readpoint = readidx1 - index;
    if(readpoint < 0) readpoint += bufsize1;
    return buffer1[readpoint];
  }

  inline _fv3_float_t _get_z2(long index)
  {
#ifdef DEBUG
    if(index > bufsize2||index <= 0) std::fprintf(stderr, "allpass3::_get_z(%ld,%ld)!\n", index, bufsize2);
#endif
    long readpoint = bufidx2 - index;
    if(readpoint < 0) readpoint += bufsize2;
    return buffer2[readpoint];
  }

  inline _fv3_float_t _get_z3(long index)
  {
#ifdef DEBUG
    if(index > bufsize3||index <= 0) std::fprintf(stderr, "allpass3::_get_z(%ld,%ld)!\n", index, bufsize3);
#endif
    long readpoint = bufidx3 - index;
    if(readpoint < 0) readpoint += bufsize3;
    return buffer3[readpoint];
  }

  inline _fv3_float_t process(_fv3_float_t input)
  {
    if(bufsize1 == 0||bufsize2 == 0||bufsize3 == 0) return input;
    return _process(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){ return process(input); }
  inline _fv3_float_t _process(_fv3_float_t input)
  {
    input += feedback3 * buffer3[bufidx3];
    _fv3_float_t output = decay3 * buffer3[bufidx3] - feedback3 * input;
    UNDENORMAL(output);

    input += feedback2 * buffer2[bufidx2];
    buffer3[bufidx3] = decay2 * buffer2[bufidx2] - feedback2 * input;
    UNDENORMAL(buffer3[bufidx3]);

    input += feedback1 * buffer1[readidx1];
    buffer2[bufidx2] = decay1 * buffer1[readidx1] - feedback1 * input;
    UNDENORMAL(buffer2[bufidx2]);

    buffer1[writeidx1] = input;

    writeidx1 ++; if(writeidx1 >= bufsize1) writeidx1 = 0;
    readidx1 ++; if(readidx1 >= bufsize1) readidx1 = 0;
    bufidx2 ++; if(bufidx2 >= bufsize2) bufidx2 = 0;
    bufidx3 ++; if(bufidx3 >= bufsize3) bufidx3 = 0;
    return output;
  }

  inline _fv3_float_t process(_fv3_float_t input, _fv3_float_t modulation)
  {
    if(bufsize1 == 0||bufsize2 == 0||bufsize3 == 0) return input;
    return _process(input, modulation);
  }
  inline _fv3_float_t operator()(_fv3_float_t input, _fv3_float_t modulation){ return process(input,modulation); }
  inline _fv3_float_t _process(_fv3_float_t input, _fv3_float_t modulation)
  {
    modulation = (modulation + 1.) * modulationsize_f;
    _fv3_float_t floor_mod = std::floor(modulation); // >= 0
    _fv3_float_t frac = modulation - floor_mod; // >= 0
    
    long readidx_a = readidx1 - (long)floor_mod;
    if(readidx_a < 0) readidx_a += bufsize1;
    
    long readidx_b = readidx_a - 1;
    if(readidx_b < 0) readidx_b += bufsize1;

    input += feedback3 * buffer3[bufidx3];
    _fv3_float_t output = decay3 * buffer3[bufidx3] - feedback3 * input;
    UNDENORMAL(output);

    input += feedback2 * buffer2[bufidx2];
    buffer3[bufidx3] = decay2 * buffer2[bufidx2] - feedback2 * input;
    UNDENORMAL(buffer3[bufidx3]);

    _fv3_float_t temp1 = buffer1[readidx_b]*frac + buffer1[readidx_a]*(1-frac);
    input += feedback1 * temp1;
    buffer2[bufidx2] = decay1 * temp1 - feedback1 * input;
    UNDENORMAL(buffer2[bufidx2]);

    buffer1[writeidx1] = input;

    writeidx1 ++; if(writeidx1 >= bufsize1) writeidx1 = 0;
    readidx1 ++; if(readidx1 >= bufsize1) readidx1 = 0;
    bufidx2 ++; if(bufidx2 >= bufsize2) bufidx2 = 0;
    bufidx3 ++; if(bufidx3 >= bufsize3) bufidx3 = 0;
    return output;
  }

  void mute();
  void setfeedback1(_fv3_float_t val);
  void setfeedback2(_fv3_float_t val);
  void setfeedback3(_fv3_float_t val);
  void setdecay1(_fv3_float_t val);
  void setdecay2(_fv3_float_t val);
  void setdecay3(_fv3_float_t val);

 private:
  _FV3_(allpass3)(const _FV3_(allpass3)& x);
  _FV3_(allpass3)& operator=(const _FV3_(allpass3)& x);
  _fv3_float_t feedback1, feedback2, feedback3, *buffer1, *buffer2, *buffer3, decay1, decay2, decay3, modulationsize_f;
  long bufsize1, readidx1, writeidx1, bufsize2, bufidx2, bufsize3, bufidx3, modulationsize;
};
