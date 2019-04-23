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

/*
** API documentation is available here:
**     http://www.mega-nerd.com/SRC/api.html
*/

#ifndef SAMPLERATE2_COMMON_H
#define SAMPLERATE2_COMMON_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
  enum
    {
      SRC_SINC_BEST_QUALITY		= 0,
      SRC_SINC_MEDIUM_QUALITY		= 1,
      SRC_SINC_FASTEST			= 2,
      SRC_ZERO_ORDER_HOLD		= 3,
      SRC_LINEAR			= 4,
      SRC_SINC_SLOW_BEST_QUALITY	= 10,
      SRC_SINC_SLOW_MEDIUM_QUALITY	= 11,
    } ;
  long src_is_valid_ratio (double ratio) ;
  const char* src_strerror (long error) ;
  const char *src_get_name (long converter_type) ;
  const char *src_get_description (long converter_type) ;
  const char *src_get_version (void) ;  
  void src_short_to_float_array(const short *in, float *out, long len);
  void src_float_to_short_array(const float *in, short *out, long len);
  void src_short_to_double_array(const short *in, double *out, long len);
  void src_double_to_short_array(const double *in, short *out, long len);
  void src_short_to_ldouble_array(const short *in, long double *out, long len);
  void src_ldouble_to_short_array(const long double *in, short *out, long len);
  int is_bad_src_ratio (double ratio);
#ifdef __cplusplus
}
#endif
#endif
