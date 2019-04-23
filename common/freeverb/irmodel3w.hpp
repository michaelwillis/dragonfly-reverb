/**
 *  Impulse Response Processor model implementation
 *  Low Latency Version / Windows Multithread Version
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

#ifndef _FV3_IRMODEL3W_HPP
#define _FV3_IRMODEL3W_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <new>
#include <fftw3.h>

#include "freeverb/frag.hpp"
#include "freeverb/delay.hpp"
#include "freeverb/blockDelay.hpp"
#include "freeverb/efilter.hpp"
#include "freeverb/utils.hpp"
#include "freeverb/irmodel3.hpp"
#include "freeverb/fv3_defs.h"

#include <windows.h>
#include <process.h>

#define FV3_EVENT_PREFIX L"FV3_IR3W_EVENT"
#define FV3_EVENT_START L"START"
#define FV3_EVENT_ENDED L"ENDED"
#define FV3_THREAD_FLAG_0 (0)
#define FV3_THREAD_FLAG_EXIT (1)
#define FV3_THREAD_FLAG_RUN (2)

namespace fv3
{

#define _fv3_float_t float
#define _FV3_(name) name ## _f
#include "irmodel3w_t.hpp"
#undef _FV3_
#undef _fv3_float_t

#define _fv3_float_t double
#define _FV3_(name) name ## _
#include "irmodel3w_t.hpp"
#undef _FV3_
#undef _fv3_float_t

#define _fv3_float_t long double
#define _FV3_(name) name ## _l
#include "irmodel3w_t.hpp"
#undef _FV3_
#undef _fv3_float_t

};

#endif
