/**
 *  Maximum Length Sequence generator based on Linear Feedback Shift Register
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

#include "freeverb/mls.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

// Reference: XILINX Linear Feedback Shift Register v3.0 DS257 (v1.0) March 28, 2003
// Polynominal 1-Terms(Xn)
const uint64_t FV3_(lfsr)::LFSR_Tap_Poly[FV3_MLS_MAX_BITS+1][FV3_MLS_MAX_TAPS] =
{{0,0,0,0,0,0,},
 {1,0,0,0,0,0,}, // 1 1+X
 {1,2,0,0,0,0,}, // 2 1+X+X^2
 {1,3,0,0,0,0,}, // 3 1+X+X^3
 {4,1,0,0,0,0,},
 {5,2,0,0,0,0,},
 {6,1,0,0,0,0,},
 {7,1,0,0,0,0,},
 {8,2,3,4,0,0,},
 {9,4,0,0,0,0,},
 {10,3,0,0,0,0,},
 {11,2,0,0,0,0,},
 {12,6,8,11,0,0,},
 {13,9,10,12,0,0,},
 {14,9,11,13,0,0,},
 {15,1,0,0,0,0,},
 {16,1,3,12,0,0,},
 {17,3,0,0,0,0,},
 {18,7,0,0,0,0,},
 {19,13,17,18,0,0,},
 {20,3,0,0,0,0,},
 {21,2,0,0,0,0,},
 {22,1,0,0,0,0,},
 {23,5,0,0,0,0,},
 {24,1,2,7,0,0,},
 {25,3,0,0,0,0,},
 {26,20,24,25,0,0,},
 {27,22,25,26,0,0,},
 {28,3,0,0,0,0,},
 {29,2,0,0,0,0,},
 {30,24,26,29,0,0,},
 {31,3,0,0,0,0,},
 {32,10,30,31,0,0,},
 {33,13,0,0,0,0,},
 {34,7,32,33,0,0,},
 {35,2,0,0,0,0,},
 {36,11,0,0,0,0,},
 {37,32,33,34,35,36,},
 {38,32,33,37,0,0,},
 {39,4,0,0,0,0,},
 {40,2,19,21,0,0,},
 {41,3,0,0,0,0,},
 {42,1,22,23,0,0,},
 {43,1,5,6,0,0,},
 {44,1,26,27,0,0,},
 {45,1,3,4,0,0,},
 {46,1,20,21,0,0,},
 {47,5,0,0,0,0,},
 {48,1,27,28,0,0,},
 {49,9,0,0,0,0,},
 {50,1,26,27,0,0,},
 {51,1,15,16,0,0,},
 {52,3,0,0,0,0,},
 {53,1,15,16,0,0,},
 {54,1,36,37,0,0,},
 {55,24,0,0,0,0,},
 {56,1,21,22,0,0,},
 {57,7,0,0,0,0,},
 {58,19,0,0,0,0,},
 {59,1,21,22,0,0,},
 {60,1,0,0,0,0,},
 {61,1,15,16,0,0,},
 {62,1,56,57,0,0,},
 {63,1,0,0,0,0,},
 {64,1,3,4,0,0,},
 {65,18,0,0,0,0,},
 {66,1,9,10,0,0,},
 {67,1,9,10,0,0,},
 {68,9,0,0,0,0,},
 {69,2,27,29,0,0,},
 {70,1,15,16,0,0,},
 {71,6,0,0,0,0,},
 {72,6,47,53,0,0,},
 {73,25,0,0,0,0,},
 {74,1,15,16,0,0,},
 {75,1,10,11,0,0,},
 {76,1,35,36,0,0,},
 {77,1,30,31,0,0,},
 {78,1,19,20,0,0,},
 {79,9,0,0,0,0,},
 {80,1,37,38,0,0,},
 {81,4,0,0,0,0,},
 {82,3,35,38,0,0,},
 {83,1,45,46,0,0,},
 {84,13,0,0,0,0,},
 {85,1,27,28,0,0,},
 {86,1,12,13,0,0,},
 {87,13,0,0,0,0,},
 {88,1,71,72,0,0,},
 {89,38,0,0,0,0,},
 {90,1,18,19,0,0,},
 {91,1,83,84,0,0,},
 {92,1,12,13,0,0,},
 {93,2,0,0,0,0,},
 {94,21,0,0,0,0,},
 {95,11,0,0,0,0,},
 {96,2,47,49,0,0,},
 {97,6,0,0,0,0,},
 {98,11,0,0,0,0,},
 {99,2,45,47,0,0,},
 {100,37,0,0,0,0,},
 {101,1,6,7,0,0,},
 {102,1,66,67,0,0,},
 {103,9,0,0,0,0,},
 {104,1,10,11,0,0,},
 {105,16,0,0,0,0,},
 {106,15,0,0,0,0,},
 {107,2,63,65,0,0,},
 {108,31,0,0,0,0,},
 {109,1,6,7,0,0,},
 {110,1,12,13,0,0,},
 {111,10,0,0,0,0,},
 {112,2,43,45,0,0,},
 {113,9,0,0,0,0,},
 {114,1,81,82,0,0,},
 {115,1,14,15,0,0,},
 {116,1,70,71,0,0,},
 {117,2,18,20,0,0,},
 {118,33,0,0,0,0,},
 {119,8,0,0,0,0,},
 {120,7,111,118,0,0,},
 {121,18,0,0,0,0,},
 {122,1,59,60,0,0,},
 {123,2,0,0,0,0,},
 {124,37,0,0,0,0,},
 {125,1,107,108,0,0,},
 {126,1,36,37,0,0,},
 {127,1,0,0,0,0,},
 {128,2,27,29,0,0,},
 {129,5,0,0,0,0,},
 {130,3,0,0,0,0,},
 {131,1,47,48,0,0,},
 {132,29,0,0,0,0,},
 {133,1,51,52,0,0,},
 {134,57,0,0,0,0,},
 {135,11,0,0,0,0,},
 {136,1,125,126,0,0,},
 {137,21,0,0,0,0,},
 {138,1,7,8,0,0,},
 {139,3,5,8,0,0,},
 {140,29,0,0,0,0,},
 {141,1,31,32,0,0,},
 {142,21,0,0,0,0,},
 {143,1,20,21,0,0,},
 {144,1,69,70,0,0,},
 {145,52,0,0,0,0,},
 {146,1,59,60,0,0,},
 {147,1,37,38,0,0,},
 {148,27,0,0,0,0,},
 {149,1,109,110,0,0,},
 {150,53,0,0,0,0,},
 {151,3,0,0,0,0,},
 {152,1,65,66,0,0,},
 {153,1,0,0,0,0,},
 {154,2,127,129,0,0,},
 {155,1,31,32,0,0,},
 {156,1,115,116,0,0,},
 {157,1,26,27,0,0,},
 {158,1,26,27,0,0,},
 {159,31,0,0,0,0,},
 {160,1,18,19,0,0,},
 {161,18,0,0,0,0,},
 {162,1,87,88,0,0,},
 {163,1,59,60,0,0,},
 {164,1,13,14,0,0,},
 {165,1,30,31,0,0,},
 {166,1,38,39,0,0,},
 {167,6,0,0,0,0,},
 {168,2,15,17,0,0,},};

FV3_(lfsr)::FV3_(lfsr)()
{
  setBitSize(4);
  initState();
  setDefaultTap();
}

FV3_(lfsr)::~FV3_(lfsr)()
{
  ;
}

void FV3_(lfsr)::setBitSize(long size)
{
  if(size > FV3_MLS_MAX_BITS||size <= 0) return;
  size_2 = size;
  setDefaultTap();
}

long FV3_(lfsr)::getBitSize()
{
  return size_2;
}

void FV3_(lfsr)::initState()
{
  currentLFSR = 1;
  cLFSR2 = 0;
  cLFSR3 = 0;
}

void FV3_(lfsr)::setDefaultTap()
{
  uint64_t tap = 0, tap2 = 0, tap3 = 0;
  for(long i = 0;i < FV3_MLS_MAX_TAPS;i ++)
    {
      uint64_t tbit = LFSR_Tap_Poly[size_2][i];
      if(tbit != 0)
	{
	  if(tbit <= 64)
	    {
	      tap |= (uint64_t)1 << (tbit-1);
	    }
	  else if(tbit <= 128)
	    {
	      tap2 |= (uint64_t)1 << (tbit-1-64);
	    }
	  else if(tbit <= 168)
	    {
	      tap3 |= (uint64_t)1 << (tbit-1-128);
	    }
	  else
	    {
	      // table error
	      throw std::bad_alloc();
	    }
	}
    }
  setTap(tap, tap2, tap3);
}

void FV3_(lfsr)::setTap(int64_t tap)
{
  currentTap = tap;
}

void FV3_(lfsr)::setTap(int64_t tap, int64_t tap2, int64_t tap3)
{
  currentTap = tap, cTap2 = tap2, cTap3 = tap3;
}

int64_t FV3_(lfsr)::getTap()
{
  return currentTap;
}

int64_t FV3_(lfsr)::getTap2()
{
  return cTap2;
}

int64_t FV3_(lfsr)::getTap3()
{
  return cTap3;
}

uint64_t FV3_(lfsr)::mls(uint32_t * buffer, uint64_t bufferSize)
{
  return mls32(buffer, bufferSize);
}

uint64_t FV3_(lfsr)::mls32(uint32_t * buffer, uint64_t bufferSize)
{
  long countBit = size_2;
  if(countBit > FV3_MLS_MAX_COUNTBIT) countBit = FV3_MLS_MAX_COUNTBIT;
  uint64_t count = ((uint64_t)1 << countBit)/FV3_MLS_INT_BIT;
  if(count > bufferSize) count = bufferSize;

  memset(buffer, 0, count*sizeof(uint32_t));

  if(size_2 <= 32)
    {
      uint32_t p = currentLFSR, t = currentTap;
      for(uint64_t i = 0;i < count;i ++)
	{
	  for(int b = 0;b < FV3_MLS_INT_BIT;b ++)
	    {
	      // save to each bit
	      buffer[i] = buffer[i] | ((p & 1u) << b);
	      // Galois LFSR
	      p = (p >> 1) ^ (-(int32_t)(p & 1u) & t);
	    }
	}
      currentLFSR = p;
    }
  else if(size_2 <= 64)
    {
      uint64_t p = currentLFSR, t = currentTap;
      for(uint64_t i = 0;i < count;i ++)
	{
	  for(int b = 0;b < FV3_MLS_INT_BIT;b ++)
	    {
	      buffer[i] = buffer[i] | ((p & 1u) << b);
	      p = (p >> 1) ^ (-(int64_t)(p & 1u) & t);
	    }
	}
      currentLFSR = p;
    }
  else if(size_2 <= 128)
    {
      uint64_t p1 = currentLFSR, p2 = cLFSR2, t1 = currentTap, t2 = cTap2;
      for(uint64_t i = 0;i < count;i ++)
	{
	  for(int b = 0;b < FV3_MLS_INT_BIT;b ++)
	    {
	      uint64_t pbit = p1 & 1u;
	      buffer[i] = buffer[i] | (pbit << b);
	      p1 = (p1 >> 1 | p2 << 63) ^ (-(int64_t)pbit & t1);
	      p2 = (p2 >> 1) ^ (-(int64_t)pbit & t2);
	    }
	}
      currentLFSR = p1, cLFSR2 = p2;
    }
  else if(size_2 <= 192)
    {
      uint64_t p1 = currentLFSR, p2 = cLFSR2, p3 = cLFSR3,
	t1 = currentTap, t2 = cTap2, t3 = cTap3;
      for(uint64_t i = 0;i < count;i ++)
	{
	  for(int b = 0;b < FV3_MLS_INT_BIT;b ++)
	    {
	      uint64_t pbit = p1 & 1u;
	      buffer[i] = buffer[i] | (pbit << b);
	      p1 = (p1 >> 1 | p2 << 63) ^ (-(int64_t)pbit & t1);
	      p2 = (p2 >> 1 | p3 << 63) ^ (-(int64_t)pbit & t2);
	      p3 = (p3 >> 1) ^ (-(int64_t)pbit & t3);
	    }
	}
      currentLFSR = p1, cLFSR2 = p2, cLFSR3 = p3;
    }
  else
    {
      throw std::bad_alloc();
    }
  return count;
}


void FV3_(lfsr)::UInt32ToFloat(uint32_t * buffer, fv3_float_t * signal, long bufferSize)
{
  UInt32ToFloat(buffer, signal, bufferSize, 1., -1.);
}

void FV3_(lfsr)::UInt32ToFloat(uint32_t * buffer, fv3_float_t * signal, long bufferSize, fv3_float_t high, fv3_float_t low)
{
  long p = 0;
  for(long j = 0;j < bufferSize;j ++)
    {
      for(int i = 0;i < FV3_MLS_INT_BIT;i ++)
        {
          signal[p] = (buffer[j] >> i) & 0x1 ? high : low;
          p ++;
        }
    }
}

#include "freeverb/fv3_ns_end.h"
