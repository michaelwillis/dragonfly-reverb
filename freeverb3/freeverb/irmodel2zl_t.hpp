/**
 *  Impulse Response Processor model implementation
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

class _FV3_(irmodel2zl) : public _FV3_(irmodel2)
{
 public:
  _FV3_(irmodel2zl)();
  virtual _FV3_(~irmodel2zl)();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR,
			      _fv3_float_t *outputL, _fv3_float_t *outputR,
			      long numsamples, unsigned options);
  virtual void mute();
  virtual void setInitialDelay(long numsamples)
    throw(std::bad_alloc);
  virtual long getLatency();
  
 protected:
  virtual void allocSwap(long numsaples) throw(std::bad_alloc);
  virtual void freeSwap();
  long ZLstart;
  _FV3_(slot) zlFrameSlot, zlOnlySlot;
  _FV3_(delay) ZLdelayL, ZLdelayR, ZLdelayWL, ZLdelayWR;
  
 private:
  _FV3_(irmodel2zl)(const _FV3_(irmodel2zl)& x);
  _FV3_(irmodel2zl)& operator=(const _FV3_(irmodel2zl)& x);
  void processZL(_fv3_float_t *inputL, _fv3_float_t *inputR,
		 _fv3_float_t *outputL, _fv3_float_t *outputR,
		 long numsamples, unsigned options);
};
