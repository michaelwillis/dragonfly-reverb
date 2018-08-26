/*
 * Dragonfly Reverb, a hall-style reverb plugin
 * Copyright (c) 2018 Michael Willis, Rob van den Berg
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

#ifndef LABELLED_KNOB_HPP_INCLUDED
#define LABELLED_KNOB_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "NanoVG.hpp"
#include "ImageWidgets.hpp"

class LabelledKnob : public Widget
{
public:
  LabelledKnob(
    Widget* parent,
    ImageKnob::Callback *callback,
    NanoVG *nanoText,
    uint id,
    char *numberFormat,
    int x, int y);
  float getValue() const noexcept;
  void setValue(float value);

protected:
  void onDisplay() override;

private:
  NanoVG *fNanoText;
  char *fNumberFormat;
  ScopedPointer<ImageKnob> fKnob;
};

#endif
