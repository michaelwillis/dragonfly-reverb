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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"float_cast.h"
#include	"common_c.h"
#include	"samplerate2_common.h"

const char * src_get_name (long converter_type)
{
  const char *desc ;
  if ((desc = sinc_get_name (converter_type)) != NULL)
    return desc ;
  if ((desc = zoh_get_name (converter_type)) != NULL)
    return desc ;
  if ((desc = linear_get_name (converter_type)) != NULL)
    return desc ;
  return NULL ;
}

const char * src_get_description (long converter_type)
{
  const char *desc ;
  if ((desc = sinc_get_description (converter_type)) != NULL)
    return desc ;
  if ((desc = zoh_get_description (converter_type)) != NULL)
    return desc ;
  if ((desc = linear_get_description (converter_type)) != NULL)
    return desc ;
  return NULL ;
}

/*
const char * src_get_version (void)
{
  return PACKAGE "-" VERSION ;
}
*/

long src_is_valid_ratio (double ratio)
{
  if (is_bad_src_ratio(ratio))
    return SRC_FALSE ;
  return SRC_TRUE ;
}

const char * src_strerror (long error)
{
  switch (error)
    {
    case SRC_ERR_NO_ERROR :
      return "No error." ;
    case SRC_ERR_MALLOC_FAILED :
      return "Malloc failed." ;
    case SRC_ERR_BAD_STATE :
      return "SR2_(SRC_STATE) pointer is NULL." ;
    case SRC_ERR_BAD_DATA :
      return "SR2_(SRC_DATA) pointer is NULL." ;
    case SRC_ERR_BAD_DATA_PTR :
      return "SR2_(SRC_DATA)->data_out is NULL." ;
    case SRC_ERR_NO_PRIVATE :
      return "Internal error. No private data." ;
    case SRC_ERR_BAD_SRC_RATIO :
      return "SRC ratio outside [1/" SRC_MAX_RATIO_STR ", " SRC_MAX_RATIO_STR "] range." ;
    case SRC_ERR_BAD_SINC_STATE :
      return "src_process() called without reset after end_of_input." ;
    case SRC_ERR_BAD_PROC_PTR :
      return "Internal error. No process pointer." ;
    case SRC_ERR_SHIFT_BITS :
      return "Internal error. SHIFT_BITS too large." ;
    case SRC_ERR_FILTER_LEN :
      return "Internal error. Filter length too large." ;
    case SRC_ERR_BAD_CONVERTER :
      return "Bad converter number." ;
    case SRC_ERR_BAD_CHANNEL_COUNT :
      return "Channel count must be >= 1." ;
    case SRC_ERR_SINC_BAD_BUFFER_LEN :
      return "Internal error. Bad buffer length. Please report this." ;
    case SRC_ERR_SIZE_INCOMPATIBILITY :
      return "Internal error. Input data / internal buffer size difference. Please report this." ;
    case SRC_ERR_BAD_PRIV_PTR :
      return "Internal error. Private pointer is NULL. Please report this." ;
    case SRC_ERR_DATA_OVERLAP :
      return "Input and output data arrays overlap." ;
    case SRC_ERR_BAD_CALLBACK :
      return "Supplied callback function pointer is NULL." ;
    case SRC_ERR_BAD_MODE :
      return "Calling mode differs from initialisation mode (ie process v callback)." ;
    case SRC_ERR_NULL_CALLBACK :
      return "Callback function pointer is NULL in src_callback_read ()." ;
    case SRC_ERR_NO_VARIABLE_RATIO :
      return "This converter only allows constant conversion ratios." ;
    case SRC_ERR_SINC_PREPARE_DATA_BAD_LEN :
      return "Internal error : Bad length in prepare_data ()." ;
    case SRC_ERR_MAX_ERROR :
      return "Placeholder. No error defined for this error number." ;
    default:
      break ;
    }
  return NULL ;
}

void src_short_to_float_array (const short *in, float *out, long len)
{
  while (len)
    {
      len -- ;
      out [len] = (float) (in [len] / (1.0 * 0x8000));
    } ;
  return ;
}

void src_float_to_short_array (const float *in, short *out, long len)
{
  double scaled_value ;
  while (len)
    {
      len -- ;
      scaled_value = in [len] * (8.0 * 0x10000000) ;
      if (scaled_value >= (1.0 * 0x7FFFFFFF))
	{
	  out [len] = 32767 ;
	  continue ;
	} ;
      if (scaled_value <= (-8.0 * 0x10000000))
	{
	  out [len] = -32768 ;
	  continue ;
	} ;
      out [len] = (short)(lrintf (scaled_value) >> 16) ;
    } ;
}


void src_short_to_double_array (const short *in, double *out, long len)
{
  while (len)
    {
      len -- ;
      out [len] = in [len] / (1.0 * 0x8000) ;
    } ;
  return ;
}

void src_double_to_short_array (const double *in, short *out, long len)
{
  float scaled_value ;
  while (len)
    {
      len -- ;
      scaled_value = in [len] * (8.0 * 0x10000000) ;
      if (scaled_value >= (1.0 * 0x7FFFFFFF))
	{
	  out [len] = 32767 ;
	  continue ;
	} ;
      if (scaled_value <= (-8.0 * 0x10000000))
	{
	  out [len] = -32768 ;
	  continue ;
	} ;
      out [len] = (lrint (scaled_value) >> 16) ;
    } ;
}

int is_bad_src_ratio (double ratio)
{
  return (ratio < (1.0 / SRC_MAX_RATIO) || ratio > (1.0 * SRC_MAX_RATIO)) ;
}
