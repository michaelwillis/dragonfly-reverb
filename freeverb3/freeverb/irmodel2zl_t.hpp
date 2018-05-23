/**
 *  Impulse Response Processor model implementation
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

class _FV3_(irmodel2zlm) : public _FV3_(irmodel2m)
{
 public:
  _FV3_(irmodel2zlm)();
  virtual _FV3_(~irmodel2zlm)();
  virtual void loadImpulse(const _fv3_float_t * inputL, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  virtual void processreplace(_fv3_float_t *inputL, long numsamples);
  virtual void mute();

 protected:
  void processZL(_fv3_float_t *inputL, _fv3_float_t *outputL, long numsamples);
  long ZLstart;
  _FV3_(slot) zlFrameSlot, zlOnlySlot;
  
 private:
  _FV3_(irmodel2zlm)(const _FV3_(irmodel2zlm)& x);
  _FV3_(irmodel2zlm)& operator=(const _FV3_(irmodel2zlm)& x);
};

class _FV3_(irmodel2zl) : public _FV3_(irmodel2)
{
 public:
  _FV3_(irmodel2zl)();
  virtual _FV3_(~irmodel2zl)();
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  
 private:
  _FV3_(irmodel2zl)(const _FV3_(irmodel2zl)& x);
  _FV3_(irmodel2zl)& operator=(const _FV3_(irmodel2zl)& x);
};
