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

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "common_c.h"
#include "type_float.h"

typedef struct SR2_(SRC_PRIVATE_tag)
{
  double	last_ratio, last_position ;
  long 	error ;
  long 	channels ;
  /* SRC_MODE_PROCESS or SRC_MODE_CALLBACK */
  long 	mode ;
  /* Pointer to data to converter specific data. */
  void	*private_data ;
  long (*vari_process) (struct SR2_(SRC_PRIVATE_tag) *psrc, SR2_(SRC_DATA) *data) ;
  long (*const_process) (struct SR2_(SRC_PRIVATE_tag) *psrc, SR2_(SRC_DATA) *data) ;

  void (*reset) (struct SR2_(SRC_PRIVATE_tag) *psrc) ;
  /* Data specific to SRC_MODE_CALLBACK. */
  SR2_(src_callback_t)	callback_func ;
  void *user_callback_data ;
  long saved_frames ;
  sr2_float_t		*saved_data ;
} SR2_(SRC_PRIVATE) ;

long SR2_(sinc_set_converter) (SR2_(SRC_PRIVATE) *psrc, long src_enum) ;
long SR2_(linear_set_converter) (SR2_(SRC_PRIVATE) *psrc, long src_enum) ;
long SR2_(zoh_set_converter) (SR2_(SRC_PRIVATE) *psrc, long src_enum) ;

#endif	/* COMMON_H_INCLUDED */
