/**
 *  Sampling Rate Converter
 *
 *  Copyright (C) 2006-2014 Teru Kamogashira
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

#include "freeverb/src.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(src)::FV3_(src)()
{
  src_stateL = src_stateR = src_stateLV = src_stateRV = NULL;
  latency = 0;
  setLPFBW(FV3_BIQUAD_RBJ_Q_BESSEL);
  setSRCFactor(1, FV3_SRC_LPF_IIR_2);
}

FV3_(src)::~FV3_(src)()
{
  freeSRC();
}

void FV3_(src)::setSRCFactor(long factor, long converter_type)
{
  latency = 0;
  
  if (src_is_valid_ratio((fv3_float_t)factor) == 0)
    {
      std::fprintf(stderr, "libsamplerate: Sample rate change out of valid range:%ld\n", factor);
      return;
    }
#ifdef DEBUG
  std::fprintf(stderr, "libsamplerate: ratio = %f converter_type = %ld\n", (fv3_float_t)factor, converter_type);
#endif
  overSamplingFactor = factor;
  src_converter = converter_type;
  freeSRC();

  if(overSamplingFactor == 1) return;

  switch(src_converter)
    {
    case FV3_SRC_ZERO_ORDER_HOLD:
      break;

    case FV3_SRC_LPF_IIR_1:
      up1L.setLPF_BW(1, 2*overSamplingFactor); down1L.setLPF_BW(1, 2*overSamplingFactor);
      up1R.setLPF_BW(1, 2*overSamplingFactor); down1R.setLPF_BW(1, 2*overSamplingFactor);
      break;

    case FV3_SRC_LPF_IIR_2:
      up2L.setLPF_RBJ  (1, lpf_iir2_bw, 2*overSamplingFactor, FV3_BIQUAD_RBJ_Q);
      down2L.setLPF_RBJ(1, lpf_iir2_bw, 2*overSamplingFactor, FV3_BIQUAD_RBJ_Q);
      up2R.setLPF_RBJ  (1, lpf_iir2_bw, 2*overSamplingFactor, FV3_BIQUAD_RBJ_Q);
      down2R.setLPF_RBJ(1, lpf_iir2_bw, 2*overSamplingFactor, FV3_BIQUAD_RBJ_Q);
      break;
      
    default:
      src_stateL = SRC_(src_new)(src_converter, 1, &src_errorL);
      src_stateR = SRC_(src_new)(src_converter, 1, &src_errorR);
      if (src_stateL == NULL||src_stateR == NULL)
	{
	  std::fprintf(stderr, "src_new(): %s|%s.\n\n", src_strerror(src_errorL), src_strerror(src_errorR)) ;
	  freeSRC();
	  return;
	}
      src_stateLV = SRC_(src_new)(src_converter, 1, &src_errorL);
      src_stateRV = SRC_(src_new)(src_converter, 1, &src_errorR);
      if (src_stateL == NULL||src_stateR == NULL)
	{
	  std::fprintf(stderr, "src_new(): %s|%s.\n\n", src_strerror(src_errorL), src_strerror(src_errorR)) ;
	  freeSRC();
	  return;
	}
      
      src_dataL.src_ratio = src_dataR.src_ratio = (fv3_float_t)factor;
      src_dataLV.src_ratio = src_dataRV.src_ratio = 1.0f/(fv3_float_t)factor;
      latency = filloutSRC();
      break;
    }
  mute();
}

void FV3_(src)::setSRCFactor(long factor)
{
  setSRCFactor(factor, src_converter);
}

long FV3_(src)::getLatency()
{
  if(overSamplingFactor == 1||src_converter == FV3_SRC_ZERO_ORDER_HOLD||src_converter == FV3_SRC_LPF_IIR_1||src_converter == FV3_SRC_LPF_IIR_2) return 0;
  return latency;
}

void FV3_(src)::mute()
{
  up1L.mute(), up1R.mute(), down1L.mute(), down1R.mute();
  up2L.mute(), up2R.mute(), down2L.mute(), down2R.mute();
  if(src_stateL == NULL||src_stateR == NULL||src_stateLV == NULL||src_stateRV == NULL) return;
  SRC_(src_reset)(src_stateL); SRC_(src_reset)(src_stateR);
  SRC_(src_reset)(src_stateLV); SRC_(src_reset)(src_stateRV);
}

void FV3_(src)::freeSRC()
{
  if(src_stateL != NULL) src_stateL = SRC_(src_delete)(src_stateL);
  if(src_stateR != NULL) src_stateR = SRC_(src_delete)(src_stateR);
  if(src_stateLV != NULL) src_stateLV = SRC_(src_delete)(src_stateLV);
  if(src_stateRV != NULL) src_stateRV = SRC_(src_delete)(src_stateRV);
  src_stateL = src_stateR = src_stateLV = src_stateRV = NULL;
}

void FV3_(src)::src_uzoh(fv3_float_t *input, fv3_float_t *output, long factor, long numsamples)
{
  for(long i = 0;i < factor;i ++)
    for(long j = 0;j < numsamples;j ++) output[factor*j+i] = input[j];
}

void FV3_(src)::src_u_iir1(fv3_float_t *input, fv3_float_t *output, long factor, long numsamples, FV3_(iir_1st) * iir)
{
  FV3_(utils)::mute(output, numsamples*factor);
  for(long i = 0;i < numsamples;i ++) output[factor*i] = input[i]*(fv3_float_t)factor;
  for(long i = 0;i < numsamples*factor;i ++) output[i] = iir->process(output[i]);
}

void FV3_(src)::src_u_iir2(fv3_float_t *input, fv3_float_t *output, long factor, long numsamples, FV3_(biquad) * iir)
{
  FV3_(utils)::mute(output, numsamples*factor);
  for(long i = 0;i < numsamples;i ++) output[factor*i] = input[i]*(fv3_float_t)factor;
  for(long i = 0;i < numsamples*factor;i ++) output[i] = iir->process(output[i]);
}

void FV3_(src)::src_dzoh(fv3_float_t *input, fv3_float_t *output, long factor, long numsamples)
{
  for(long i = 0;i < numsamples;i ++) output[i] = input[i*factor];
}

void FV3_(src)::src_d_iir1(fv3_float_t *input, fv3_float_t *output, long factor, long numsamples, FV3_(iir_1st) * iir)
{
  for(long i = 0;i < numsamples*factor;i ++) input[i] = iir->process(input[i]);
  for(long i = 0;i < numsamples;i ++) output[i] = input[factor*i];
}

void FV3_(src)::src_d_iir2(fv3_float_t *input, fv3_float_t *output, long factor, long numsamples, FV3_(biquad) * iir)
{
  for(long i = 0;i < numsamples*factor;i ++) input[i] = iir->process(input[i]);
  for(long i = 0;i < numsamples;i ++) output[i] = input[factor*i];
}

long FV3_(src)::usrc(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  if(overSamplingFactor == 1)
    {
      memcpy(outputL, inputL, sizeof(fv3_float_t)*numsamples); 
      memcpy(outputR, inputR, sizeof(fv3_float_t)*numsamples);
      return numsamples;
    }

  switch(src_converter)
    {
    case SRC_ZERO_ORDER_HOLD:
      src_uzoh(inputL, outputL, overSamplingFactor, numsamples);
      src_uzoh(inputR, outputR, overSamplingFactor, numsamples);
      break;

    case FV3_SRC_LPF_IIR_1:
      src_u_iir1(inputL, outputL, overSamplingFactor, numsamples, &up1L);
      src_u_iir1(inputR, outputR, overSamplingFactor, numsamples, &up1R);
      break;

    case FV3_SRC_LPF_IIR_2:
      src_u_iir2(inputL, outputL, overSamplingFactor, numsamples, &up2L);
      src_u_iir2(inputR, outputR, overSamplingFactor, numsamples, &up2R);
      break;

    default:
      src_dataL.data_in = inputL; src_dataL.data_out = outputL;
      src_dataR.data_in = inputR; src_dataR.data_out = outputR;
      src_dataL.end_of_input = src_dataR.end_of_input = 0;
      src_dataL.input_frames = src_dataR.input_frames = numsamples;
      src_dataL.output_frames = src_dataR.output_frames = numsamples*overSamplingFactor;
      for(long i = 0;i < numsamples;i ++)
	{
	  UNDENORMAL(inputL[i]); UNDENORMAL(inputR[i]);
	}
      process(src_stateL, &src_dataL);
      process(src_stateR, &src_dataR);
      for(long i = 0;i < numsamples;i ++)
	{
	  UNDENORMAL(outputL[i]); UNDENORMAL(outputR[i]);
	}
      numsamples = src_dataL.output_frames_gen;
      break;
    }
  return numsamples;
}

long FV3_(src)::dsrc(fv3_float_t *inputL, fv3_float_t *inputR,
		     fv3_float_t *outputL, fv3_float_t *outputR,
		     long numsamples)
{
  if(overSamplingFactor == 1)
    {
      memcpy(outputL, inputL, sizeof(fv3_float_t)*numsamples); 
      memcpy(outputR, inputR, sizeof(fv3_float_t)*numsamples);
      return numsamples;
    }

  switch(src_converter)
    {
    case FV3_SRC_ZERO_ORDER_HOLD:
      src_dzoh(inputL, outputL, overSamplingFactor, numsamples);
      src_dzoh(inputR, outputR, overSamplingFactor, numsamples);
      break;
      
    case FV3_SRC_LPF_IIR_1:
      src_d_iir1(inputL, outputL, overSamplingFactor, numsamples, &down1L);
      src_d_iir1(inputR, outputR, overSamplingFactor, numsamples, &down1R);
      break;
      
    case FV3_SRC_LPF_IIR_2:
      src_d_iir2(inputL, outputL, overSamplingFactor, numsamples, &down2L);
      src_d_iir2(inputR, outputR, overSamplingFactor, numsamples, &down2R);
      break;

    default:  
      src_dataLV.data_in = inputL; src_dataLV.data_out = outputL;
      src_dataRV.data_in = inputR; src_dataRV.data_out = outputR;
      src_dataLV.end_of_input = src_dataRV.end_of_input = 0;
      src_dataLV.input_frames =	src_dataRV.input_frames = numsamples*overSamplingFactor;
      src_dataLV.output_frames = src_dataRV.output_frames = numsamples;
      for(long i = 0;i < numsamples;i ++)
	{
	  UNDENORMAL(inputL[i]); UNDENORMAL(inputR[i]);
	}
      process(src_stateLV, &src_dataLV);
      process(src_stateRV, &src_dataRV);
      for(long i = 0;i < numsamples;i ++)
	{
	  UNDENORMAL(outputL[i]); UNDENORMAL(outputR[i]);
	}
      numsamples = src_dataLV.output_frames_gen;
    }
  return numsamples;
}

void FV3_(src)::process(SRC_(SRC_STATE) * state, SRC_(SRC_DATA) * data)
{
  if (long src_error = SRC_(src_process)(state, data))
    {
      std::fprintf(stderr, "src_process():%s\n", src_strerror(src_error));
      return;
    }  
}

long FV3_(src)::filloutSRC()
{
#ifdef DEBUG
  std::fprintf(stderr, "filling SRC:");
#endif
  if(overSamplingFactor == 1) return 0;
  if(src_converter == SRC_ZERO_ORDER_HOLD) return 0;
    
  fv3_float_t L = 0.0, R = 0.0;
  fv3_float_t L2[overSamplingFactor], R2[overSamplingFactor];
  long ref1 = 0, ref2 = 0, uDelay = 0, dDelay = 0;
  long count = 0;
  while(1)
    {
      L = 0.0; R = 0.0;
      ref1 = usrc(&L, &R, L2, R2, 1);
      uDelay += (overSamplingFactor-ref1);
#ifdef DEBUG
      std::fprintf(stderr, "%ld>", ref1);
#endif
      ref2 = dsrc(L2, R2, &L, &R, 1);
      dDelay += (1-ref2);
#ifdef DEBUG
      std::fprintf(stderr, "%ld,", ref2);
#endif
      if(ref1 > 0&&ref2)
	count = uDelay / overSamplingFactor + dDelay;
      if(ref1 > 0&&ref2 > 0&&L == 0&&R == 0)
	break;
    }
#ifdef DEBUG
  std::fprintf(stderr, "latency: %ld/%ld,%ld\n", uDelay, dDelay, count);
#endif
  return count;
}

#include "freeverb/fv3_ns_end.h"
