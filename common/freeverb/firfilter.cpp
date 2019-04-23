/**
 *  FIR filter
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

#include "freeverb/firfilter.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

void FV3_(firfilter)::lpf(fv3_float_t h[], const long N, const long WINDOW, const fv3_float_t fc, const fv3_float_t param)
{
  long i;
  fv3_float_t * w = new fv3_float_t[N];
  fv3_float_t * sinc = new fv3_float_t[N];
  FV3_(firwindow)::Sinc(sinc, N, fc);
  FV3_(firwindow)::getWindow(WINDOW, w, N, fc, param);
  for(i = 0; i < N; i++) h[i] = sinc[i] * w[i];
  delete[] w;
  delete[] sinc;
  return;
}

void FV3_(firfilter)::hpf(fv3_float_t h[], const long N, const long WINDOW, const fv3_float_t fc, const fv3_float_t param)
{
  long i;
  lpf(h, N, WINDOW, fc, param);
  for(i = 0;i < N;i ++)
    {
      h[i] *= -1.0;     // = 0 - h[i]
    }
  h[(N-1)/2] += 1.0;    // = 1 - h[(N-1)/2]
  return;
}

void FV3_(firfilter)::bef(fv3_float_t h[], const long N, const long WINDOW, const fv3_float_t fc1, const fv3_float_t fc2, const fv3_float_t param)
{
  long i;
  fv3_float_t *h1 = new fv3_float_t[N];
  fv3_float_t *h2 = new fv3_float_t[N];
  lpf(h1, N, WINDOW, fc1, param);
  hpf(h2, N, WINDOW, fc2, param);
  for(i = 0;i < N;i ++)
    {
      h[i] = h1[i] + h2[i];
    }
  delete[] h1;
  delete[] h2;
  return;
}

void FV3_(firfilter)::bpf(fv3_float_t h[], const long N, const long WINDOW, const fv3_float_t fc1, const fv3_float_t fc2, const fv3_float_t param)
{
  long i;
  bef(h, N, WINDOW, fc1, fc2, param);
  for(i = 0;i < N;i ++)
    {
      h[i] *= -1.0;     // = 0 - h[i]
    }
  h[(N-1)/2] += 1.0;    // = 1 - h[(N-1)/2]
  return;
}

long FV3_(firfilter)::predictFilterLength(long window, fv3_float_t fc)
{
  long N = 4096;
  switch(window)
    {
    case FV3_W_SQUARE:
      N = (long)std::ceil((1.8*M_PI)/(fc*M_PI))+1;
      break;
    case FV3_W_BLACKMAN:
      N = (long)std::ceil((11*M_PI)/(fc*M_PI))+1;
      break;
    case FV3_W_HANNING:
      N = (long)std::ceil((6.2*M_PI)/(fc*M_PI))+1;
      break;
    case FV3_W_HAMMING:
      N = (long)std::ceil((6.6*M_PI)/(fc*M_PI))+1;
      break;
    case FV3_W_KAISER:
      //if(attenuation <= 21)
      N = (long)std::ceil(10/fc)+1;
      //else
      //N = (long)ceil((attenuation-7.95)/(14.36*fc))+1;
      break;
    case FV3_W_COSRO:
      break;
    default:
      break;
    }
  return N;
}

#include "freeverb/fv3_ns_end.h"
