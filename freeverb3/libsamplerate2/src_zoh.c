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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "float_cast.h"
#include "samplerate2.h"
#include "common.h"

#define	ZOH_MAGIC_MARKER	MAKE_MAGIC ('s', 'r', 'c', 'z', 'o', 'h')

typedef struct
{
  long zoh_magic_marker;
  long channels;
  long reset;
  long in_count, in_used;
  long out_count, out_gen;
  sr2_float_t last_value [1];
} SR2_(ZOH_DATA);

static void
SR2_(zoh_reset) (SR2_(SRC_PRIVATE) *psrc)
{
  SR2_(ZOH_DATA) *priv;
  priv = (SR2_(ZOH_DATA)*) psrc->private_data;
  if(priv == NULL) return;
  priv->channels = psrc->channels;
  priv->reset = 1;
  memset (priv->last_value, 0, sizeof (priv->last_value [0]) * priv->channels);
  return;
}

static long SR2_(zoh_vari_process) (SR2_(SRC_PRIVATE) *psrc, SR2_(SRC_DATA) *data)
{
  SR2_(ZOH_DATA) *priv;
  double src_ratio, input_index, rem;
  long ch;

  if(psrc->private_data == NULL) return SRC_ERR_NO_PRIVATE;

  priv = (SR2_(ZOH_DATA)*)psrc->private_data;
  
  if(priv->reset)
    {
      /* If we have just been reset, set the last_value data. */
      for(ch = 0;ch < priv->channels;ch ++)
	priv->last_value [ch] = data->data_in [ch];
      priv->reset = 0;
    }
  
  priv->in_count = data->input_frames * priv->channels;
  priv->out_count = data->output_frames * priv->channels;
  priv->in_used = priv->out_gen = 0;
  
  src_ratio = psrc->last_ratio;
  input_index = psrc->last_position;
  
  /* Calculate samples before first sample in input array. */
  while(input_index < 1.0 && priv->out_gen < priv->out_count)
    {
      if(priv->in_used + priv->channels * input_index >= priv->in_count)
	break;
      
      if(priv->out_count > 0 && fabs (psrc->last_ratio - data->src_ratio) > SRC_MIN_RATIO_DIFF)
	src_ratio = psrc->last_ratio + priv->out_gen * (data->src_ratio - psrc->last_ratio) / priv->out_count;
      
      for(ch = 0;ch < priv->channels;ch ++)
	{
	  data->data_out [priv->out_gen] = priv->last_value [ch];
	  priv->out_gen ++;
	}
      
      /* Figure out the next index. */
      input_index += 1.0 / src_ratio;
    }
  
  rem = SR2_(fmod_one)(input_index);
  priv->in_used += priv->channels * lrint (input_index - rem);
  input_index = rem;
  
  /* Main processing loop. */
  while(priv->out_gen < priv->out_count && priv->in_used + priv->channels * input_index <= priv->in_count)
    {
      if(priv->out_count > 0&& fabs(psrc->last_ratio - data->src_ratio) > SRC_MIN_RATIO_DIFF)
	src_ratio = psrc->last_ratio + priv->out_gen * (data->src_ratio - psrc->last_ratio) / priv->out_count;
      
      for(ch = 0;ch < priv->channels;ch ++)
	{
	  data->data_out[priv->out_gen] = data->data_in[priv->in_used - priv->channels + ch];
	  priv->out_gen ++;
	}
      
      /* Figure out the next index. */
      input_index += 1.0 / src_ratio;
      rem = SR2_(fmod_one)(input_index);
      
      priv->in_used += priv->channels * lrint (input_index - rem);
      input_index = rem;
    }
  
  if(priv->in_used > priv->in_count)
    {
      input_index += (priv->in_used - priv->in_count) / priv->channels;
      priv->in_used = priv->in_count;
    }
  
  psrc->last_position = input_index;
  
  if(priv->in_used > 0)
    for (ch = 0; ch < priv->channels; ch++)
      priv->last_value [ch] = data->data_in [priv->in_used - priv->channels + ch];
  
  /* Save current ratio rather then target ratio. */
  psrc->last_ratio = src_ratio;
  
  data->input_frames_used = priv->in_used / priv->channels;
  data->output_frames_gen = priv->out_gen / priv->channels;
  
  return SRC_ERR_NO_ERROR;
}

long SR2_(zoh_set_converter) (SR2_(SRC_PRIVATE) *psrc, long src_enum)
{
  SR2_(ZOH_DATA) *priv = NULL;
  
  if(src_enum != SRC_ZERO_ORDER_HOLD) return SRC_ERR_BAD_CONVERTER;
  
  if(psrc->private_data != NULL)
    {
      free(psrc->private_data);
      psrc->private_data = NULL;
    }
  
  if(psrc->private_data == NULL)
    {
      priv = calloc (1, sizeof (*priv) + psrc->channels * sizeof (sr2_float_t));
      if(priv == NULL) return SRC_ERR_MALLOC_FAILED;
      psrc->private_data = priv;
    }
  
  priv->zoh_magic_marker = ZOH_MAGIC_MARKER;
  priv->channels = psrc->channels;
  
  psrc->const_process = SR2_(zoh_vari_process);
  psrc->vari_process = SR2_(zoh_vari_process);
  psrc->reset = SR2_(zoh_reset);
  
  SR2_(zoh_reset) (psrc);
  
  return SRC_ERR_NO_ERROR;
} /* zoh_set_converter */
