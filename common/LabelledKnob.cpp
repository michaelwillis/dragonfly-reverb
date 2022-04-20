/*
 * Dragonfly Reverb, copyright (c) 2022 Michael Willis
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

#include "LabelledKnob.hpp"

LabelledKnob::LabelledKnob(Widget* widget, ImageKnob::Callback* callback, Image* image, NanoVG* nanoText, const Param* param, const char * numberFormat, int x, int y)
    : SubWidget(widget)
{
  setWidth(image->getWidth() + 20);
  setHeight(image->getHeight() + 30);
  setAbsolutePos(x, y);

  fNumberFormat = numberFormat;
  fNanoText = nanoText;
  fName = param->name;

  knob = new ImageKnob(this, *image); // Image ( raster, width, height, GL_BGRA )
  knob->setId(param->id);
  knob->setAbsolutePos(x + 10, y + 14);
  knob->setRange(param->range_min, param->range_max);
  knob->setRotationAngle(300);
  knob->setCallback(callback);
}

void LabelledKnob::setDefault(float value) noexcept
{
    knob->setDefault(value);
}

float LabelledKnob::getValue() const noexcept
{
  return knob->getValue();
}

void LabelledKnob::setValue(float value)
{
  knob->setValue(value);
}

void LabelledKnob::onDisplay()
{
  fNanoText->beginFrame ( this );
  fNanoText->textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  fNanoText->fontSize ( 15 );
  fNanoText->fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  fNanoText->textBox ( 0, 7, getWidth(), fName, nullptr );

  char strBuf[32+1];
  strBuf[32] = '\0';
  std::snprintf ( strBuf, 32, fNumberFormat, knob->getValue() );

  fNanoText->fontSize ( 14 );
  fNanoText->fillColor ( Color ( 0.9f, 0.9f, 0.9f ) );
  fNanoText->textBox ( 0, getHeight() - 7, getWidth(), strBuf, nullptr );

  fNanoText->endFrame();
}
