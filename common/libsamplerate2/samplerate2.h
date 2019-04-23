/*
 * Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef SAMPLERATE2_H
#define SAMPLERATE2_H

#include "samplerate2_common.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#ifdef _SR2_
#error "_SR2_ is defined."
#endif

#ifdef _sr2_float_t
#error "_sr2_float_t is defined."
#endif

#define _sr2_float_t float
#define _SR2_(name) name ## _f
#include "samplerate2_t.h"
#undef _SR2_
#undef _sr2_float_t

#define _sr2_float_t double
#define _SR2_(name) name ## _
#include "samplerate2_t.h"
#undef _SR2_
#undef _sr2_float_t

#define _sr2_float_t long double
#define _SR2_(name) name ## _l
#include "samplerate2_t.h"
#undef _SR2_
#undef _sr2_float_t

#ifdef __cplusplus
}		/* extern "C" */
#endif	/* __cplusplus */

#endif	/* SAMPLERATE2_H */
