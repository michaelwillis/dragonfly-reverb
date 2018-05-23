/**
 *  FIR window
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

#include "freeverb/firwindow.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

void FV3_(firwindow)::Square(fv3_float_t w[], const long N)
{
  long i;
  for(i = 0;i < N;i ++) w[i] = 1;
  return;
}

void FV3_(firwindow)::Hamming(fv3_float_t w[], const long N)
{
  long i;
  const fv3_float_t M = N-1;
  for(i = 0; i < N; i++)
    {
      w[i] = 0.54 - (0.46*std::cos(2.0*M_PI*(fv3_float_t)i/M));
    }
  return;
}

void FV3_(firwindow)::Hanning(fv3_float_t w[], const long N)
{
  long i;
  const fv3_float_t M = N-1;
  for(i = 0; i < N; i++)
    {
      w[i] = 0.5 * (1.0 - std::cos(2.0*M_PI*(fv3_float_t)i/M));
    }
  return;
}

void FV3_(firwindow)::Blackman(fv3_float_t w[], const long N)
{
  long i;
  const fv3_float_t M = N-1;
  for(i = 0; i < N; i++)
    {
      w[i] = 0.42 - (0.5 * std::cos(2.0*M_PI*(fv3_float_t)i/M)) + (0.08 * std::cos(4.0*M_PI*(fv3_float_t)i/M));
    }
  return;
}

fv3_float_t FV3_(firwindow)::i_zero(const fv3_float_t x)
{
  /*
    The zeroth order Modified Bessel function:
    
    I_0(x) = 1 + 
    x^2 / 2^2 + 
    x^4 / (2^2 * 4^2) +
    x^6 / (2^2 * 4^2 * 6^2) +
    ... to infinity        
    
    This function grows quite quickly towards very large numbers.
    By re-organising the calculations somewhat we minimise the dynamic
    range in the floating polong numbers, and can thus calculate the
    function for larger x than we could do with a naive implementation.  
  */
  fv3_float_t n, a, halfx, sum;
  halfx = x / 2.0;
  sum = 1.0;
  a = 1.0;
  n = 1.0;
  do {
    a *= halfx;
    a /= n;
    sum += a * a;
    n += 1.0;
    /* either 'sum' will reach +inf or 'a' zero... */
  } while (a != 0.0 && isfinite(sum));
  return sum;
}

fv3_float_t FV3_(firwindow)::KaiserBeta(const fv3_float_t dB)
{
  fv3_float_t beta;
  if(dB > 50)
    beta = 0.1102*(dB-8.7);
  else if(dB >= 20)
    beta = 0.5842*std::pow((double)dB-21.0, 0.4) + 0.07886*(dB-21.0);
  else
    beta = 0; 
  return beta;
}

void FV3_(firwindow)::Kaiser(fv3_float_t w[], const long N, fv3_float_t beta)
{
  // beta = 0.0 = square window
  // beta = 1.5 ~ hamming window
  // beta = 2.0 ~ hann window
  // beta = 3.0 ~ Blackman window
  long i;
  const fv3_float_t M = N-1;
  fv3_float_t inv_izbeta = 1.0 / i_zero(M_PI*beta);
  for(i = 0;i < N;i ++)
    {
      w[i] = i_zero(M_PI*beta*std::sqrt(1-pow(2*(fv3_float_t)i/M-1, 2))) * inv_izbeta;
    }
}

void FV3_(firwindow)::CosROW(fv3_float_t w[], const long N, const fv3_float_t fc, const fv3_float_t alpha)
{
  long i;
  const fv3_float_t M = N-1;
  fv3_float_t n;
  for(i = 0; i < N; i++)
    {
      n = (fv3_float_t)i - M/2.0;
      w[i] = std::cos(M_PI*n*2.0*fc*M_PI*alpha) / (1 - std::pow((M_PI*n*2.0*fc*2*alpha),2));
    }        
  return;
}

void FV3_(firwindow)::getWindow(const long WINDOW, fv3_float_t w[], const long N, const fv3_float_t fc, const fv3_float_t param)
{
  switch(WINDOW)
    {
    case FV3_W_BLACKMAN:
      FV3_(firwindow)::Blackman(w, N);
      break;
    case FV3_W_HANNING:
      FV3_(firwindow)::Hanning(w, N);
      break;
    case FV3_W_HAMMING:
      FV3_(firwindow)::Hamming(w, N);
      break;
    case FV3_W_KAISER:
      FV3_(firwindow)::Kaiser(w, N, param);
      break;
    case FV3_W_COSRO:
      FV3_(firwindow)::CosROW(w, N, fc, param);
      break;
    case FV3_W_SQUARE:
    default:
      FV3_(firwindow)::Square(w, N);
      break;
    }
}

void FV3_(firwindow)::Sinc(fv3_float_t sinc[], const long N, const fv3_float_t fc)
{
  long i;
  const fv3_float_t M = N-1;
  fv3_float_t n;
  // Generate sinc delayed by (N-1)/2
  for(i = 0; i < N; i++)
    {
      if(i == M/2.0)
	{
	  sinc[i] = 2.0 * fc;
	}
      else
	{
	  n = (fv3_float_t)i - M/2.0;
	  sinc[i] = std::sin(M_PI*n*2.0*fc) / (M_PI*n);
	}
    }        
  return;
}

#include "freeverb/fv3_ns_end.h"
