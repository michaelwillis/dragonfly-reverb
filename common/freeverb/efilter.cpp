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

#include "freeverb/efilter.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// class iir_1st

FV3_(iir_1st)::FV3_(iir_1st)()
{
  mute();
}

void FV3_(iir_1st)::mute()
{
  y1 = 0;
}

void FV3_(iir_1st)::printconfig()
{
  std::fprintf(stderr, "<< 1st order IIR Filter Coefficients >>\n");
  std::fprintf(stderr, "(in)--+----*b1-->+----------+->(out) \n");
  std::fprintf(stderr, "      |          ^          |        \n");
  std::fprintf(stderr, "      v          |          v        \n");
  std::fprintf(stderr, "  [z^-1]---*b2-->+<--*a2---[z^-1]    \n");
  std::fprintf(stderr, "b1 = %f, b2 = %f\n", b1, b2);
  std::fprintf(stderr, "a1 = 1, a2 = %f\n", a2);
}

void FV3_(iir_1st)::setCoefficients(fv3_float_t _b1, fv3_float_t _b2, fv3_float_t _a2)
{
  b1 = _b1; b2 = _b2; a2 = _a2;
}

/*
  First-Order IIR Butterworth Low-Pass Filter
*/

void FV3_(iir_1st)::setLPF_BW(fv3_float_t fc, fv3_float_t fs)
{
  fv3_float_t omega_2 = M_PI*fc/fs;
  fv3_float_t tan_omega_2 = std::tan(omega_2);
  b1 = b2 = tan_omega_2/(1+tan_omega_2);
  a2 = (1-tan_omega_2)/(1+tan_omega_2);
  /*
    AudioFilteringToolkit
    John Lane, Jayant Datta, Brent Karley, Jay Norwood, "DSP Filters",
    PROMPT Publications(an imprint of Sams Technical Publishing),
    Indianapolis, IN, 2001, page 43. Book's website: www.dspaudiocookbook.com
    
    beta = 0.5 * ( ( 1 - sin( 2 * pi * (fc / fs) ) ) / ( 1 + sin( 2 * pi * (fc / fs) ) ) );
    gamma = ( 0.5 + beta ) * cos ( 2 * pi * (fc / fs) );
    alfa = ( 1 - gamma ) / 2;
    a(1)*y(n) = b(1)*x(n) + b(2)*x(n-1) - a(2)*y(n-1)
    a(1) = 1;
    a(2) = -gamma; % notice the sign!
    b(1) = alfa;
    b(2) = alfa;
  */
}

void FV3_(iir_1st)::setHPF_BW(fv3_float_t fc, fv3_float_t fs)
{
  fv3_float_t omega_2 = M_PI*fc/fs;
  fv3_float_t tan_omega_2 = std::tan(omega_2);
  b1 = 1/(1+tan_omega_2);
  b2 = -1 * b1;
  a2 = (1-tan_omega_2)/(1+tan_omega_2);
  /*
    beta = 0.5 * ( ( 1 - sin( 2 * pi * (fc / fs) ) ) / ( 1 + sin( 2 * pi * (fc / fs) ) ) );
    gamma = ( 0.5 + beta ) * cos ( 2 * pi * (fc / fs) );
    alfa = ( 1 + gamma ) / 2;
    a = zeros(1,2);
    b = zeros(1,2);
    a(1)*y(n) = b(1)*x(n) + b(2)*x(n-1) - a(2)*y(n-1)
    a(1) = 1;
    a(2) = -gamma; % notice the sign!
    b(1) = alfa;
    b(2) = -alfa;
  */
}

/*
  The functions below have been derived from the book of An Audio Cookbook by Christopher Moore.
  First Order Digital Filters--An Audio Cookbook. Application Note AN-11 by Christopher Moore
  http://www.sevenwoodsaudio.com/AN11.pdf
*/

void FV3_(iir_1st)::setLPF_A(fv3_float_t fc, fv3_float_t fs)
{
  a2 = std::exp(-1*M_PI*fc/(fs/2.));
  b1 = 1.; b2 = .12;
  fv3_float_t norm = (1-a2)/(b1+b2);
  b1 *= norm; b2 *= norm;
}

void FV3_(iir_1st)::setHPF_A(fv3_float_t fc, fv3_float_t fs)
{
  a2 = std::exp(-1*M_PI*fc/(fs/2.));
  b1 = 1.; b2 = -1.;
  fv3_float_t norm = (1+a2)/2.;
  b1 *= norm; b2 *= norm;
}

void FV3_(iir_1st)::setLSF_A(fv3_float_t f1, fv3_float_t f2, fv3_float_t fs)
{
  a2 = -1 * std::exp(-1*M_PI*f1/(fs/2.));
  b1 = -1.;
  b2 = std::exp(-1*M_PI*f2/(fs/2.));
}

void FV3_(iir_1st)::setHSF_A(fv3_float_t f1, fv3_float_t f2, fv3_float_t fs)
{
  a2 = std::exp(-1*M_PI*f1/(fs/2.));
  b1 = -1.;
  b2 = std::exp(-1*M_PI*f2/(fs/2.));
  fv3_float_t norm = (1-a2)/(b1+b2);
  b1 *= norm; b2 *= norm;
}

void FV3_(iir_1st)::setHPFwLFS_A(fv3_float_t fc, fv3_float_t fs)
{
  b1 = -1.;
  b2 = std::exp(-1*M_PI*fc/(fs/2.));
  a2 = -.12;
  fv3_float_t norm = (1-a2)/std::abs(b1+b2);
  b1 *= norm; b2 *= norm;
}

void FV3_(iir_1st)::setLPF_C(fv3_float_t fc, fv3_float_t fs)
{
  b1 = b2 = fc/(fs+fc);
  a2 = (fs-fc)/(fs+fc);
}

void FV3_(iir_1st)::setHPF_C(fv3_float_t fc, fv3_float_t fs)
{
  b1 = fs/(fs+fc);
  b2 = -1 * b1;
  a2 = (fs-fc)/(fs+fc);
}

void FV3_(iir_1st)::setPole(fv3_float_t v)
{
  a2 = v; b1 = 1; b2 = 0;
  fv3_float_t norm = 1.-std::abs(a2);
  b1 *= norm; b2 *= norm;
}

void FV3_(iir_1st)::setZero(fv3_float_t v)
{
  a2 = 0; b1 = -1.; b2 = v;
  fv3_float_t norm = std::abs(b1) + std::abs(b2);
  b1 *= norm; b2 *= norm;
}

void FV3_(iir_1st)::setPoleLPF(fv3_float_t fc, fv3_float_t fs)
{
  fv3_float_t omega = 2.*M_PI*fc/fs;
  fv3_float_t cos_omega = std::cos(omega);
  fv3_float_t coeff = (2-cos_omega)-std::sqrt((2-cos_omega)*(2-cos_omega) - 1);
  a2 = coeff;
  b1 = 1-coeff; b2 = 0;
}

void FV3_(iir_1st)::setPoleHPF(fv3_float_t fc, fv3_float_t fs)
{
  fv3_float_t omega = 2.*M_PI*fc/fs;
  fv3_float_t cos_omega = std::cos(omega);
  fv3_float_t coeff = (2+cos_omega)-std::sqrt((2+cos_omega)*(2+cos_omega) - 1);
  a2 = -1 * coeff;
  b1 = coeff-1; b2 = 0;
}

void FV3_(iir_1st)::setZeroLPF(fv3_float_t fc, fv3_float_t fs)
{
  // fc > fs/4
  fv3_float_t omega = 2.*M_PI*fc/fs;
  fv3_float_t cos_omega = std::cos(omega);
  fv3_float_t coeff = (1-2*cos_omega)-std::sqrt((1-2*cos_omega)*(1-2*cos_omega)-1);
  a2 = 0;
  b1 = 1/(1+coeff);
  b2 = coeff/(1+coeff);
}

void FV3_(iir_1st)::setZeroHPF(fv3_float_t fc, fv3_float_t fs)
{
  // fc < fs/4
  fv3_float_t omega = 2.*M_PI*fc/fs;
  fv3_float_t cos_omega = std::cos(omega);
  fv3_float_t coeff = (1+2*cos_omega)-std::sqrt((1+2*cos_omega)*(1+2*cos_omega)-1);
  a2 = 0;
  b1 = 1/(1+coeff);
  b2 = -1 * coeff/(1+coeff);
}

// class efilter

FV3_(efilter)::FV3_(efilter)()
{
  setLPF(0); setHPF(0); mute();
}

void FV3_(efilter)::mute()
{
  lpfL.mute(); lpfR.mute();
  hpfL.mute(); hpfR.mute();
}

void FV3_(efilter)::setLPF(fv3_float_t val)
{
  pole = val;
  lpfL.setPole(val);
  lpfR.setPole(val);
}

void FV3_(efilter)::setHPF(fv3_float_t val)
{
  zero = val;
  hpfL.setZero(val);
  hpfR.setZero(val);
}

fv3_float_t FV3_(efilter)::getLPF()
{
  return pole;
}

fv3_float_t FV3_(efilter)::getHPF()
{
  return zero;
}

// class dccut

FV3_(dccut)::FV3_(dccut)()
{
  gain = .9999f; mute();
}

void FV3_(dccut)::mute()
{
  y1 = y2 = 0;
}

void FV3_(dccut)::seta(fv3_float_t val) 
{
  gain = val;
}

fv3_float_t FV3_(dccut)::geta() 
{
  return gain;
}

void FV3_(dccut)::setCutOnFreq(fv3_float_t fc, fv3_float_t fs)
{
  fv3_float_t _fc = 2*fc/fs;
  gain = (std::sqrt(3.) - 2.*std::sin(M_PI*_fc))/(std::sin(M_PI*_fc) + std::sqrt(3.)*std::cos(M_PI*_fc));
}

fv3_float_t FV3_(dccut)::getCutOnFreq()
{
  return  std::atan(std::sqrt(3.)*(1.-gain*gain)/(1.+4.*gain+gain*gain))/M_PI;
}

fv3_float_t FV3_(dccut)::getCutOnFreq(fv3_float_t fs)
{
  return getCutOnFreq()*fs/2.;
}

// class ahdsr

FV3_(ahdsr)::FV3_(ahdsr)()
{
  loopMode = false;
  init();
}

void FV3_(ahdsr)::setLoopMode(bool mode)
{
  loopMode = mode;
}

void FV3_(ahdsr)::init()
{
  count = -1;
}

void FV3_(ahdsr)::setRAHDSR(long samples, fv3_float_t _attack, fv3_float_t _hold, fv3_float_t _decay, fv3_float_t _sustain, fv3_float_t _release)
{
  long rest = 0;
  countTotal = samples;
  release = (fv3_float_t)samples * _release;
  rest = samples - release;
  attack = (fv3_float_t)rest * _attack;
  rest -= attack;
  decay = (fv3_float_t)rest * _decay;
  rest -= decay;
  hold = (fv3_float_t)rest * _hold;
  rest -= hold;
  sustain = rest;
  sustainLevel = _sustain;
}

fv3_float_t FV3_(ahdsr)::process(fv3_float_t input)
{
  count ++;
  if(count < attack)
    return input * (fv3_float_t)count / (fv3_float_t)attack;
  if(count >= attack&&count < attack+hold)
    return input;
  if(count >= attack+hold&&count < attack+hold+decay)
    return input * (sustainLevel + (1.-sustainLevel)*(1. - (fv3_float_t)(count-(attack+hold)) / (fv3_float_t)decay));
  if(count >= attack+hold+decay&&count < attack+hold+decay+sustain)
    return input * sustainLevel;
  if(count >= attack+hold+decay+sustain&&count < attack+hold+decay+sustain+release)
    return input * sustainLevel*(1. - (fv3_float_t)(count-(attack+hold+decay+sustain)) / (fv3_float_t)release);
  if(count >= attack+hold+decay+sustain+release)
    {
      if(loopMode)
	count = -1;
      else
	count --;
      return 0;
    }
  return 1;
}

#include "freeverb/fv3_ns_end.h"
