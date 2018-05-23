/**
 *  Simple Delay
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

/**
 * A simple delay class.
 */
class _FV3_(delay)
{
 public:
  _FV3_(delay)();
  _FV3_(~delay)();
  void free();
  
  /**
   * Set delay size. This preserves previous data.
   * @param[in] size The delay size.
   */
  void setsize(long size) throw(std::bad_alloc);
  long getsize();

  /**
   * Retrive the last signal of the delayline.
   * @return The last signal (z^(-bufsize)) of the delayline. This is the same as get_z(getsize()).
   */
  inline _fv3_float_t getlast()
  {
    if(bufsize == 0) return 0;
    return _getlast();
  }
  inline _fv3_float_t _getlast(){ return buffer[bufidx]; }
  
  /**
   * Retrive the signal of the delayline.
   * @param[in] index specifies the index of the delayline.
   * @return The signal value (z^(-index)).
   */
  inline _fv3_float_t get_z(long index)
  {
    if(bufsize == 0) return 0;
    if(index > bufsize) index = bufsize;
    if(index <= 0) index = 1;
    return _get_z(index);
  }
  inline _fv3_float_t _get_z(long index)
  {
#ifdef DEBUG
    if(index > bufsize||index <= 0) std::fprintf(stderr, "delay::_get_z(%ld,%ld)!\n", index, bufsize);
#endif
    long readpoint = bufidx - index;
    if(readpoint < 0) readpoint += bufsize;
    return buffer[readpoint];
  }

  inline _fv3_float_t process(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){ return process(input); }
  inline _fv3_float_t _process(_fv3_float_t input)
  {
    _fv3_float_t bufout = buffer[bufidx];
    buffer[bufidx] = input;
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    return bufout;
  }
  
  inline _fv3_float_t process_wf(_fv3_float_t input)
  {
    if(bufsize == 0) return feedback*input;
    return _process_wf(input);
  }
  inline _fv3_float_t _process_wf(_fv3_float_t input)
  {
    _fv3_float_t bufout = buffer[bufidx];
    buffer[bufidx] = feedback*input;
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    return bufout;
  }

  void mute();
  void setfeedback(_fv3_float_t val);
  _fv3_float_t getfeedback();
  
 private:
  _FV3_(delay)(const _FV3_(delay)& x);
  _FV3_(delay)& operator=(const _FV3_(delay)& x);  
  _fv3_float_t feedback, *buffer;
  long bufsize, bufidx;
};

/**
 * A delay modulation class.
 */
class _FV3_(delaym)
{
 public:
  _FV3_(delaym)();
  _FV3_(~delaym)();
  void free();

  void setsize(long size) throw(std::bad_alloc);
  void setsize(long size, long modsize) throw(std::bad_alloc);
  long getsize();
  long getdelaysize();
  long getmodulationsize();
  void mute();
  void setfeedback(_fv3_float_t val);
  _fv3_float_t getfeedback();
  
  inline _fv3_float_t process(_fv3_float_t input){ return process(input, 0); }
  inline _fv3_float_t operator()(_fv3_float_t input){ return process(input); }

  /**
   * A simple delay with a allpass interpolated modulation.
   * @param[in] input signal.
   * @return processed signal.
   */  
  inline _fv3_float_t process(_fv3_float_t input, _fv3_float_t modulation)
  {
    if(bufsize == 0) return input;
    return _process(input, modulation);
  }
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
    buffer[writeidx] = feedback*input;
    writeidx ++; if(writeidx >= bufsize) writeidx = 0;
    return z_1;
  }
  inline _fv3_float_t operator()(_fv3_float_t input, _fv3_float_t modulation){ return process(input,modulation); }

  inline _fv3_float_t _getlast(){ return z_1; }
  
 private:
  _FV3_(delaym)(const _FV3_(delaym)& x);
  _FV3_(delaym)& operator=(const _FV3_(delaym)& x);  
  _fv3_float_t feedback, *buffer, z_1, modulationsize_f;
  long bufsize, readidx, writeidx, modulationsize;
};
