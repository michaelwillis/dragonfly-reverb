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

class _FV3_(lfsr)
{
 public:
  _FV3_(lfsr)();
  _FV3_(~lfsr)();

  void setBitSize(long size);
  long getBitSize();
  void initState();

  void setDefaultTap();
  void setTap(int64_t tap);
  void setTap(int64_t tap, int64_t tap2, int64_t tap3);
  int64_t getTap();
  int64_t getTap2();
  int64_t getTap3();

  uint64_t mls(uint32_t * buffer, uint64_t bufferSize);

  void UInt32ToFloat(uint32_t * buffer, _fv3_float_t * signal, long bufferSize);
  void UInt32ToFloat(uint32_t * buffer, _fv3_float_t * signal, long bufferSize, _fv3_float_t high, _fv3_float_t low);

 protected:
  const static uint64_t LFSR_Tap_Poly[FV3_MLS_MAX_BITS+1][FV3_MLS_MAX_TAPS];

 private:
  _FV3_(lfsr)(const _FV3_(lfsr)& x);
  _FV3_(lfsr)& operator=(const _FV3_(lfsr)& x);

  uint64_t mls32(uint32_t * buffer, uint64_t bufferSize);

  long size_2;
  int64_t currentLFSR, currentTap, cLFSR2, cTap2, cLFSR3, cTap3;
};
