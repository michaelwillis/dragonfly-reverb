/**
 *  Comb (delay) filter with LPF
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
 * A feedback delayed comb filter with LPF in original freeverb.
 */
class _FV3_(comb)
{
public:
  _FV3_(comb)();
  _FV3_(~comb)();
  void free();

  /**
   * Set delay size. This preserves previous data.
   * @param[in] size The delay size.
   */
  void setsize(long size) throw(std::bad_alloc);
  long getsize();
  void mute();
  void          setdamp(_fv3_float_t val);
  _fv3_float_t  getdamp();
  inline void   setfeedback(_fv3_float_t val){ feedback = val; }
  _fv3_float_t  getfeedback();

  /**
   * A feedback delayed comb filter with internal LPF.
   * The original feedback comb filter's output is taken from the head of the delayline,
   * but this filter, which is used in many reverb algorithms takes its output from
   * the last of the delayline.
   * @param[in] input The input signal.
   */
  inline _fv3_float_t process(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process(input);
  }
  inline _fv3_float_t operator()(_fv3_float_t input){return process(input);}
  inline _fv3_float_t _process(_fv3_float_t input)
  {
    _fv3_float_t output = buffer[bufidx];
    UNDENORMAL(output);
    filterstore = (output * damp2) + (filterstore * damp1);
    buffer[bufidx] = input + (filterstore * feedback);
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    return output;
  }

  /**
   * A simple feedforward delayed comb filter.
   * @param[in] input The input signal.
   */
  inline _fv3_float_t process_ff(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process_ff(input);
  }
  inline _fv3_float_t process_ff(_fv3_float_t input, _fv3_float_t fb){ setfeedback(fb); return process_ff(input); }
  inline _fv3_float_t _process_ff(_fv3_float_t input)
  {
    _fv3_float_t output = buffer[bufidx] * feedback + input;
    buffer[bufidx] = input;
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    UNDENORMAL(output);
    return output;
  }
  inline _fv3_float_t _process_ff(_fv3_float_t input, _fv3_float_t fb){ setfeedback(fb); return _process_ff(input); }

  /**
   * A simple feedback delayed comb filter.
   * @param[in] input The input signal.
   */
  inline _fv3_float_t process_fb(_fv3_float_t input)
  {
    if(bufsize == 0) return input;
    return _process_fb(input);
  }
  inline _fv3_float_t process_fb(_fv3_float_t input, _fv3_float_t fb){ setfeedback(fb); return process_fb(input); }
  inline _fv3_float_t _process_fb(_fv3_float_t input)
  {
    input = buffer[bufidx] * feedback + input;
    buffer[bufidx] = input;
    bufidx ++; if(bufidx >= bufsize) bufidx = 0;
    UNDENORMAL(input);
    return input;
  }
  inline _fv3_float_t _process_fb(_fv3_float_t input, _fv3_float_t fb){ setfeedback(fb); return _process_fb(input); }

private:
  _FV3_(comb)(const _FV3_(comb)& x);
  _FV3_(comb)& operator=(const _FV3_(comb)& x);
  _fv3_float_t *buffer, feedback, filterstore, damp1, damp2;
  long bufsize, bufidx;
};

/**
 * A modulated feedback delayed comb filter with LPF.
 */
class _FV3_(combm)
{
public:
  _FV3_(combm)();
  _FV3_(~combm)();
  void free();

  void setsize(long size) throw(std::bad_alloc);
  void setsize(long size, long modsize) throw(std::bad_alloc);
  long getsize();
  long getdelaysize();
  long getmodulationsize();
  void mute();
  void          setdamp(_fv3_float_t val);
  _fv3_float_t	getdamp();
  void	        setfeedback(_fv3_float_t val);
  _fv3_float_t	getfeedback();
  
  inline _fv3_float_t process(_fv3_float_t input){return process(input, 0);}
  inline _fv3_float_t operator()(_fv3_float_t input){return process(input);}

  inline _fv3_float_t process(_fv3_float_t input, _fv3_float_t modulation)
  {
    if(bufsize == 0) return input;
    modulation = (modulation + 1.) * modulationsize_f;
    _fv3_float_t floor_mod = std::floor(modulation); // >= 0
    _fv3_float_t m_frac = 1. - (modulation - floor_mod); // >= 0
    
    long readidx_a = readidx - (long)floor_mod; if(readidx_a < 0) readidx_a += bufsize;
    long readidx_b = readidx_a - 1; if(readidx_b < 0) readidx_b += bufsize;
    
    z_1 = buffer[readidx_b] + m_frac * (buffer[readidx_a] - z_1);
    UNDENORMAL(z_1);
    
    readidx ++; if(readidx >= bufsize) readidx = 0;

    filterstore = z_1 * damp2 + filterstore * damp1;
    UNDENORMAL(filterstore);

    buffer[writeidx] = input + filterstore * feedback;
    writeidx ++; if(writeidx >= bufsize) writeidx = 0;
    
    return z_1;
  }
  inline _fv3_float_t operator()(_fv3_float_t input, _fv3_float_t modulation){return process(input,modulation);}

private:
  _FV3_(combm)(const _FV3_(combm)& x);
  _FV3_(combm)& operator=(const _FV3_(combm)& x);
  _fv3_float_t *buffer, feedback, filterstore, damp1, damp2, z_1, modulationsize_f;
  long bufsize, readidx, writeidx, delaysize, modulationsize;
};
