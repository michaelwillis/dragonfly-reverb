/**
 *  Utility
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

#include "freeverb/utils.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

fv3_float_t FV3_(utils)::dB2R(fv3_float_t dB)
{
  return std::pow(10., dB/20.);
}

fv3_float_t FV3_(utils)::R2dB(fv3_float_t real)
{
  return 20.*std::log(real)/std::log(10.0f);
}

long FV3_(utils)::ms2sample(fv3_float_t msec, long fs)
{
  return FV3_(utils)::ms2sample(msec, (fv3_float_t)fs);
}

long FV3_(utils)::ms2sample(fv3_float_t msec, fv3_float_t fs)
{
  return (long)(msec*fs*0.001);
}

void FV3_(utils)::mute(fv3_float_t * f, long t)
{
  std::memset(f, 0, sizeof(fv3_float_t)*t);
}

long FV3_(utils)::checkPow2(long i)
{
  long p = 2;
  while(p < i) p *= 2;
  return p;
}

bool FV3_(utils)::isPrime(long number)
{
  if(number == 2) return true;
  if(number & 1)
    {
      for (long i=3; i<(long)std::sqrt((double)number)+1; i+=2)
	if ( (number % i) == 0) return false;
      return true; // prime
    }
  else
    return false; // even
}

void * FV3_(utils)::aligned_malloc(size_t size, size_t align_size)
{
  // [...padding {1~align_size byte(s)}...|<void*>|...aligned data...]
  void * actualAddress = std::malloc(size+sizeof(void*)+align_size);
  if(actualAddress == NULL) return NULL;
  void * returnAddress = NULL;
  uintptr_t bitmask = align_size-1; bitmask = ~bitmask;
  uintptr_t adPtr = reinterpret_cast<uintptr_t>(actualAddress);
  uintptr_t aAPtr = (bitmask & (adPtr + sizeof(void*))) + align_size;
  
  returnAddress = reinterpret_cast<void*>(aAPtr);
  char * ptChar = static_cast<char*>(returnAddress); ptChar -= sizeof(void*);
  std::memcpy(ptChar, &actualAddress, sizeof(void*));
  return returnAddress;
}

void FV3_(utils)::aligned_free(void *ptr)
{
  if(ptr == NULL) return;
  char * ptChar = static_cast<char*>(ptr); ptChar -= sizeof(void*);
  void * actualAddress = NULL;
  std::memcpy(&actualAddress, ptChar, sizeof(void*));
  std::free(actualAddress);
}

uint16_t FV3_(utils)::getX87CW()
{
  uint16_t x87cw = 0;
#if defined(ENABLE_X86SIMD)
  __asm__ __volatile__ ("fstcw %0" : "=m" (x87cw));
#endif
  return x87cw;
}

void FV3_(utils)::setX87CW(uint16_t cw)
{
#if defined(ENABLE_X86SIMD)
  __asm__  __volatile__ ("fldcw %0" : : "m"(cw));
#endif
}

uint32_t FV3_(utils)::getMXCSR()
{
  uint32_t mxcsr = 0;
#if defined(ENABLE_X86SIMD)
  if(getSIMDFlag()&FV3_X86SIMD_FLAG_SSE)
    {
      __asm__ __volatile__ ("stmxcsr %0" : "=m" (mxcsr));
    }
#endif
  return mxcsr;
}

uint32_t FV3_(utils)::getMXCSR_MASK()
{
  uint32_t mxcsr_mask = 0;
#if defined(ENABLE_X86SIMD)
  if(getSIMDFlag()&FV3_X86SIMD_FLAG_SSE)
    {
      unsigned char * fxsave_s = NULL;
      fxsave_s = (unsigned char *)aligned_malloc(512, FV3_PTR_ALIGN_BYTE);
      if(fxsave_s == NULL) return 0;
      memset(fxsave_s, 0, 512);
      __asm__ __volatile__ ("fxsave %0" : : "m" (*fxsave_s));
      memcpy(&mxcsr_mask, fxsave_s+28, 4);
      FV3_(utils)::aligned_free(fxsave_s);
      if(mxcsr_mask == 0) mxcsr_mask = 0xFFBF;
    }
#endif
  return mxcsr_mask;
}

void FV3_(utils)::setMXCSR(uint32_t mxcsr)
{
#if defined(ENABLE_X86SIMD)
  if(getSIMDFlag()&FV3_X86SIMD_FLAG_SSE)
    {
      mxcsr &= getMXCSR_MASK();
      __asm__  __volatile__ ("ldmxcsr %0" : : "m"(mxcsr));
    }
#endif
}

void FV3_(utils)::XGETBV(uint32_t op, uint32_t * _eax, uint32_t *_edx)
{
#if defined(ENABLE_X86SIMD)
#if __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 4
  uint32_t j[5] = {0,0,0,0,0,};
  cpuid(0x1,&j[1],&j[2],&j[3],&j[4]);
  if((j[3] & (FV3_X86SIMD_CPUID_OSXSAVE)) == (FV3_X86SIMD_CPUID_OSXSAVE))
    {
      __asm__ __volatile__ ("xgetbv" : "=a" (*_eax), "=d" (*_edx) : "c" (op) :);
    }
#endif
#endif
}

void FV3_(utils)::cpuid(uint32_t op, uint32_t *out_eax, uint32_t *out_ebx, uint32_t *out_ecx, uint32_t *out_edx)
{
#if defined(ENABLE_X86SIMD)
  uint32_t c_eax, c_ebx, c_ecx, c_edx;
#if defined(__amd64__)||defined(__x86_64__)
  __asm__ __volatile__("cpuid\n\t"
		       :[eax]"=a"(c_eax),[ebx]"=b"(c_ebx),[ecx]"=c"(c_ecx),[edx]"=d"(c_edx) :"a"(op) : "cc" );
#else
  __asm__ __volatile__("xchgl %%ebx,%[ebx]\n\t"
		       "cpuid\n\t"
		       "xchgl %%ebx,%[ebx]\n\t"
		       :[eax]"=a"(c_eax),[ebx]"=r"(c_ebx),[ecx]"=c"(c_ecx),[edx]"=d"(c_edx) :"a"(op) : "cc" );
#endif
  *out_eax = c_eax, *out_ebx = c_ebx, *out_ecx = c_ecx, *out_edx = c_edx;
#endif
}

uint32_t FV3_(utils)::getSIMDFlag()
{
  uint32_t simdFlag = FV3_X86SIMD_FLAG_FPU;
#if defined(ENABLE_X86SIMD)
  uint32_t j[5];
  memset(j,0,sizeof(uint32_t)*5);
  cpuid(0x80000001,&j[1],&j[2],&j[3],&j[4]);
  if((j[4] & FV3_X86SIMD_CPUID_3DNOW)&&
     (j[3] & FV3_X86SIMD_CPUID_3DNOW_PREF))
    {
      simdFlag |= FV3_X86SIMD_FLAG_3DNOWP;
    }

  memset(j,0,sizeof(uint32_t)*5);
  cpuid(0x1,&j[1],&j[2],&j[3],&j[4]);
  if(j[4] & FV3_X86SIMD_CPUID_SSE)
    {
      simdFlag |= FV3_X86SIMD_FLAG_SSE;
    }
  if(j[4] & FV3_X86SIMD_CPUID_SSE2)
    {
      simdFlag |= FV3_X86SIMD_FLAG_SSE2;
    }
  if(j[3] & FV3_X86SIMD_CPUID_SSE3)
    {
      simdFlag |= FV3_X86SIMD_FLAG_SSE3;
    }
  if(j[3] & FV3_X86SIMD_CPUID_SSE4_1)
    {
      simdFlag |= FV3_X86SIMD_FLAG_SSE4_1;
    }
  if((j[3] & (FV3_X86SIMD_CPUID_OSXSAVE|FV3_X86SIMD_CPUID_AVX)) == (FV3_X86SIMD_CPUID_OSXSAVE|FV3_X86SIMD_CPUID_AVX))
    {
      uint32_t k[2] = {0,0,};
      XGETBV(0, &k[0], &k[1]);
      if((k[0] & 0x6) == 0x6)
	{
	  simdFlag |= FV3_X86SIMD_FLAG_AVX;
	  if(j[3] & FV3_X86SIMD_CPUID_FMA3)
	    {
	      simdFlag |= FV3_X86SIMD_FLAG_FMA3;
	    }
	  cpuid(0x80000001,&j[1],&j[2],&j[3],&j[4]);
	  if(j[3] & FV3_X86SIMD_CPUID_FMA4)
	    {
	      simdFlag |= FV3_X86SIMD_FLAG_FMA4;
	    }
	}
    }
#endif
  return simdFlag;
}

#include "freeverb/fv3_ns_end.h"
