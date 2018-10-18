/**
 *  Freeverb3 definition
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

#ifndef _FV3_DEFS_H
#define _FV3_DEFS_H

#include <cmath>

#include "fv3_config.h"

#ifndef isfinite
#define isfinite(v) std::isfinite(v)
#endif
#ifndef isnormal
#define isnormal(v) std::isnormal(v)
#endif
#ifndef fpclassify
#define fpclassify(v) std::fpclassify(v)
#endif
#define STRINGIZEx(x) #x
#define STRINGIZE(x) STRINGIZEx(x)

#ifdef DISABLE_UNDENORMAL
#define UNDENORMAL(v)
#else
#define UNDENORMAL(v) if(fpclassify(v) != FP_NORMAL&&fpclassify(v) != FP_ZERO){v=0;}
#endif

#ifndef LIMIT_PLUSMINUS_ONE
#define LIMIT_PLUSMINUS_ONE(v) if(v < -1.){ v = -1.; } if(v > 1.){ v = 1.; }
#endif

#ifndef M_PI
#define M_PI   3.1415926535897932384626433832795028841971693993751
#endif
#ifndef M_LN2
#define M_LN2  0.69314718055994530941723212145817656807550013436
#endif
#ifndef LN_2_2
#define LN_2_2 0.34657359027997265470861606072908828403775006718
#endif

#define FV3_X86SIMD_NULL              0x00000000
#define FV3_X86SIMD_CPUID_FPU         0x00000001 // edx/eax=1
#define FV3_X86SIMD_CPUID_MMX         0x00800000 // edx/eax=1
#define FV3_X86SIMD_CPUID_SSE         0x02000000 // edx/eax=1
#define FV3_X86SIMD_CPUID_SSE2        0x04000000 // edx/eax=1
#define FV3_X86SIMD_CPUID_SSE3        0x00000001 // ecx/eax=1
#define FV3_X86SIMD_CPUID_SSE4_1      0x00080000 // ecx/eax=1
#define FV3_X86SIMD_CPUID_SSE4_2      0x00100000 // ecx/eax=1
#define FV3_X86SIMD_CPUID_XSAVE       0x04000000 // ecx/eax=1
#define FV3_X86SIMD_CPUID_OSXSAVE     0x08000000 // ecx/eax=1
#define FV3_X86SIMD_CPUID_AVX         0x10000000 // ecx/eax=1
#define FV3_X86SIMD_CPUID_FMA3        0x00001000 // ecx/eax=1

#define FV3_X86SIMD_CPUID_3DNOW       0x80000000 // edx/eax=0x80000001
#define FV3_X86SIMD_CPUID_3DNOWEXT    0x40000000 // edx/eax=0x80000001
#define FV3_X86SIMD_CPUID_3DNOW_PREF  0x00000100 // ecx/eax=0x80000001
#define FV3_X86SIMD_CPUID_XOP         0x00000800 // ecx/eax=0x80000001
#define FV3_X86SIMD_CPUID_FMA4        0x00010000 // ecx/eax=0x80000001

// SIMD code select, size div (X:depreciated F:float D:double L:long double)
#define FV3_X86SIMD_FLAG_NULL         0x00000000
#define FV3_X86SIMD_FLAG_FPU          0x00000001 //  -    FDL
#define FV3_X86SIMD_FLAG_SSE_V1       0x00000002 //  4    F   To use SSE version 1 code, use with FV3_X86SIMD_FLAG_SSE (see sample)
#define FV3_X86SIMD_FLAG_SSE          0x00000004 //  4    F
#define FV3_X86SIMD_FLAG_SSE2         0x00000008 //   /2   D
#define FV3_X86SIMD_FLAG_SSE3         0x00000010 //  4    F
#define FV3_X86SIMD_FLAG_SSE4_1       0x00000020 //   /2   D
#define FV3_X86SIMD_FLAG_AVX          0x00000040 // 16/8  FD
#define FV3_X86SIMD_FLAG_FMA3         0x00000080 // 16/8  FD  Not AVX2
#define FV3_X86SIMD_FLAG_3DNOWP       0x00000100 //  2   XF   AMD 3DNow! with prefetch, depreciated: Bulldozer/Bobcat~ no-support
#define FV3_X86SIMD_FLAG_FMA4         0x00000200 // 16/8 XFD  AMD, depreciated: Ryzen~ no-support

#define FV3_X86SIMD_MXCSR_FZ          0x00008000 // Flush To Zero
#define FV3_X86SIMD_MXCSR_DAZ         0x00000040 // Denormals Are Zero
#define FV3_X86SIMD_MXCSR_EMASK_ALL   0x00001F80 // All Exceptions Masks

// for maximum support
// AVX FMA3 FMA4
#define FV3_PTR_ALIGN_BYTE 32
// SSE SSE2 SSE3 SSE4
// #define FV3_PTR_ALIGN_BYTE 16
// FPU
// #define FV3_PTR_ALIGN_BYTE 8

#define FV3_IR_DEFAULT     (0U)
#define FV3_IR_MUTE_DRY    (1U << 1)
#define FV3_IR_MUTE_WET    (1U << 2)
#define FV3_IR_SKIP_FILTER (1U << 3)
#define FV3_IR_MONO2STEREO (1U << 4)
#define FV3_IR_SKIP_INIT   (1U << 5)
#define FV3_IR_SWAP_LR     (1U << 6)

/* SIMD size */
#define FV3_IR_Min_FragmentSize 16
#define FV3_IR2_DFragmentSize 16384
#define FV3_IR3_DFragmentSize 1024
#define FV3_IR3_DefaultFactor 16

#define FV3_3BS_IR2_DFragmentSize 1024
#define FV3_3BS_IR3_DFragmentSize 256
#define FV3_3BS_IR3_DefaultFactor 4

#define FV3_LFO_RCOUNT 10000

#define FV3_EARLYREF_PRESET_DEFAULT 0
#define FV3_EARLYREF_PRESET_0 0
#define FV3_EARLYREF_PRESET_1 1
#define FV3_EARLYREF_PRESET_2 2

#define FV3_EARLYREF_PRESET_11 11
#define FV3_EARLYREF_PRESET_12 12
#define FV3_EARLYREF_PRESET_13 13
#define FV3_EARLYREF_PRESET_14 14
#define FV3_EARLYREF_PRESET_15 15
#define FV3_EARLYREF_PRESET_16 16
#define FV3_EARLYREF_PRESET_17 17
#define FV3_EARLYREF_PRESET_18 18
#define FV3_EARLYREF_PRESET_19 19
#define FV3_EARLYREF_PRESET_20 20
#define FV3_EARLYREF_PRESET_21 21
#define FV3_EARLYREF_PRESET_22 22

#define FV3_REVBASE_DEFAULT_FS 48000
#define FV3_REVTYPE_SELF    0
#define FV3_REVTYPE_PROG   30
#define FV3_REVTYPE_PROG2  31
#define FV3_REVTYPE_ZREV   40
#define FV3_REVTYPE_ZREV2  41

#define FV3_NOISEGEN_PINK_FRACTAL_1_DEFAULT_HURST_CONST 0.5
#define FV3_NOISEGEN_PINK_FRACTAL_1_DEFAULT_BUFSIZE 15
#define FV3_NOISEGEN_GAUSSIAN_WHITE_3_DEFAULT_PRECISION 32

#define FV3_MLS_INT_BIT 32
#define FV3_MLS_MAX_BITS 168
#define FV3_MLS_MAX_TAPS 6
#define FV3_MLS_MAX_COUNTBIT 60

#define FV3_SWP_DEFAULT 0
#define FV3_SWP_EXP 0
#define FV3_SWP_LIN 1

#ifdef __cplusplus
extern "C" {
#endif
  enum
    {
      FV3_W_BLACKMAN = 1,
      FV3_W_HANNING  = 2,
      FV3_W_HAMMING  = 3,
      FV3_W_KAISER   = 4,
      FV3_W_COSRO    = 5,
      FV3_W_SQUARE   = 6,
    };
  
  enum
    {
      FV3_3BSPLIT_IR_IR2 = 0,
      FV3_3BSPLIT_IR_IR3 = 1,
    };
  
  enum
    {
      FV3_SRC_SINC_BEST_QUALITY        =   0,
      FV3_SRC_SINC_MEDIUM_QUALITY      =   1,
      FV3_SRC_SINC_FASTEST             =   2,
      FV3_SRC_ZERO_ORDER_HOLD          =   3,
      FV3_SRC_LINEAR                   =   4,
      FV3_SRC_SINC_SLOW_BEST_QUALITY   =  10,
      FV3_SRC_SINC_SLOW_MEDIUM_QUALITY =  11,
      FV3_SRC_LPF_IIR_1                = 100,
      FV3_SRC_LPF_IIR_2                = 101,
    };
#ifdef __cplusplus
}
#endif
#endif
