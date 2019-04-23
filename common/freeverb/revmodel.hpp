/**
 *  Enhanced Freeverb
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

#ifndef _FV3_REVMODEL_HPP
#define _FV3_REVMODEL_HPP

#include "freeverb/revbase.hpp"
#include "freeverb/comb.hpp"
#include "freeverb/allpass.hpp"
#include "freeverb/slot.hpp"
#include "freeverb/src.hpp"
#include "freeverb/delay.hpp"
#include "freeverb/fv3_defs.h"

#define FV3_FREV_DEFAULT_FS 44100
#define FV3_FREV_FIXED_GAIN 0.015
#define FV3_FREV_SCALE_WET 3.0
#define FV3_FREV_SCALE_DRY 2.0
#define FV3_FREV_SCALE_DAMP 0.4
#define FV3_FREV_SCALE_ROOM 0.28
#define FV3_FREV_OFFSET_ROOM 0.7
#define FV3_FREV_STEREO_SPREAD441 23
#define FV3_FREV_ALLPASS_FEEDBACK 0.5
#define FV3_FREV_NUM_COMB 8
#define FV3_FREV_NUM_ALLPASS 4

namespace fv3
{

#define _fv3_float_t float
#define _FV3_(name) name ## _f
#include "freeverb/revmodel_t.hpp"
#undef _FV3_
#undef _fv3_float_t

#define _fv3_float_t double
#define _FV3_(name) name ## _
#include "freeverb/revmodel_t.hpp"
#undef _FV3_
#undef _fv3_float_t

#define _fv3_float_t long double
#define _FV3_(name) name ## _l
#include "freeverb/revmodel_t.hpp"
#undef _FV3_
#undef _fv3_float_t

};

#endif
