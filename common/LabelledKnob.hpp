/*
 * Dragonfly Reverb, copyright (c) 2019 Michael Willis, Rob van den Berg
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

#include "NanoVG.hpp"
#include "ImageWidgets.hpp"
#include "Param.hpp"

class LabelledKnob : public SubWidget
{
public:
  LabelledKnob(
    Widget* parent,
    ImageKnob::Callback* callback,
    Image* image,
    NanoVG* nanoText,
    const Param* param,
    const char* numberFormat,
    int x, int y);
  float getValue() const noexcept;
  void setValue(float value);
  void setDefault(float value) noexcept;

protected:
  void onDisplay() override;

private:
  NanoVG *fNanoText;
  const char *fNumberFormat;
  const char *fName;
  ScopedPointer<ImageKnob> fKnob;
};

#endif
