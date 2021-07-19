/*
 * Dragonfly Reverb, copyright (c) 2020 Michael Willis, Rob van den Berg
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

#ifndef COMMON_UI_HPP_INCLUDED
#define COMMON_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "ImageWidgets.hpp"
#include "NanoVG.hpp"
#include "DistrhoPlugin.hpp"
#include "Spectrogram.hpp"
#include "LabelledKnob.hpp"

class DragonflyReverbAbstractUI : public UI, public ImageKnob::Callback {
public:
  DragonflyReverbAbstractUI(uint width, uint height, const Param* params, const char* knobData, uint knobWidth, uint knobHeight);
protected:
  NanoVG nanoText;
  const Param* params;
  Image knobImage;

  LabelledKnob * createLabelledKnob(const Param *param, const char * numberFormat, int x, int y);
};

#endif
