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

class _FV3_(irmodels) : public _FV3_(irbase)
{
 public:
  _FV3_(irmodels)();
  virtual _FV3_(~irmodels)();
  virtual void loadImpulse(const _fv3_float_t * inputL, const _fv3_float_t * inputR, long size)
    throw(std::bad_alloc);
  virtual void unloadImpulse();
  using _FV3_(irbase)::processreplace;
  virtual void processreplace(const _fv3_float_t *inputL, const _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples);
  virtual void mute();

 private:
  _FV3_(irmodels)(const _FV3_(irmodels)& x);
  _FV3_(irmodels)& operator=(const _FV3_(irmodels)& x);
  void allocImpulse(long size)
    throw(std::bad_alloc);
  void freeImpulse();
  void update();
  _FV3_(slot) impulse, delay;
  long current;
};
