#include "samplerate2_common.h"

const char * linear_get_name (long src_enum)
{
  if (src_enum == SRC_LINEAR)
    return "Linear Interpolator" ;
  return NULL ;
}

const char * linear_get_description (long src_enum)
{
  if (src_enum == SRC_LINEAR)
    return "Linear interpolator, very fast, poor quality." ;
  return NULL ;
}

const char * sinc_get_name (long src_enum)
{
  switch (src_enum)
    {
    case SRC_SINC_SLOW_BEST_QUALITY :
      return "Slow Best Sinc Interpolator" ;
    case SRC_SINC_SLOW_MEDIUM_QUALITY :
      return "Slow Medium Sinc Interpolator" ;
    case SRC_SINC_BEST_QUALITY :
      return "Best Sinc Interpolator" ;
    case SRC_SINC_MEDIUM_QUALITY :
      return "Medium Sinc Interpolator" ;
    case SRC_SINC_FASTEST :
      return "Fastest Sinc Interpolator" ;
    };
  return NULL ;
}

const char * sinc_get_description (long src_enum)
{
  switch (src_enum)
    {
    case SRC_SINC_SLOW_MEDIUM_QUALITY :
      return "Band limited sinc interpolation, medium quality, 121dB SNR, 90% BW." ;
    case SRC_SINC_SLOW_BEST_QUALITY :
      return "Band limited sinc interpolation, best quality, 145dB SNR, 96% BW." ;
    case SRC_SINC_BEST_QUALITY :
      return "Band limited sinc interpolation, best quality, 97dB SNR, 96% BW." ;
    case SRC_SINC_MEDIUM_QUALITY :
      return "Band limited sinc interpolation, medium quality, 97dB SNR, 90% BW." ;
    case SRC_SINC_FASTEST :
      return "Band limited sinc interpolation, fastest, 97dB SNR, 80% BW." ;
    } ;
  return NULL ;
}

const char * zoh_get_name (long src_enum)
{
  if (src_enum == SRC_ZERO_ORDER_HOLD)
    return "ZOH Interpolator" ;	
  return NULL ;
}

const char * zoh_get_description (long src_enum)
{
  if (src_enum == SRC_ZERO_ORDER_HOLD)
    return "Zero order hold interpolator, very fast, poor quality." ;
  return NULL ;
}
