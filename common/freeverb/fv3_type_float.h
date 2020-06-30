/**
 *  Freeverb3 floating point typdef
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

#ifndef _FV3_TYPE_FLOAT_H
#define _FV3_TYPE_FLOAT_H

#include "fv3_config.h"

#ifdef LIBFV3_FLOAT
typedef float fv3_float_t;
#define FV3_(name) name ## _f
#define FFTW_(name) fftwf_ ## name
#else
#ifdef LIBFV3_DOUBLE
typedef double fv3_float_t;
#define FV3_(name) name ## _
#define FFTW_(name) fftw_ ## name
#define SRC_(name) name ## _
#else
#ifdef LIBFV3_LDOUBLE
typedef long double fv3_float_t;
#define FV3_(name) name ## _l
#define FFTW_(name) fftwl_ ## name
#define SRC_(name) name ## _l
#else
#ifdef LIBFV3_DEFAULT
typedef float fv3_float_t;
#define FV3_(name) name
#define FFTW_(name) fftwf_ ## name
#define SRC_(name) name ## _f
#define LIBFV3_FLOAT
#else
#error "No float type was defined."
#endif
#endif
#endif
#endif

#endif
