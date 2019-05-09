/*
 * Dragonfly Reverb, Copyright (c) 2019 Michael Willis, Rob van den Berg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#ifndef ABSTRACT_DSP_HPP_INCLUDED
#define ABSTRACT_DSP_HPP_INCLUDED

#include "stdint.h"

class AbstractDSP {
public:
  virtual void setParameterValue(uint32_t index, float value) = 0;
  virtual void run(const float** inputs, float** outputs, uint32_t frames) = 0;
  virtual void mute() = 0;
  virtual ~AbstractDSP() { };
};

#endif
