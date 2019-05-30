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

#include "LabelledKnob.hpp"
#include "DistrhoPluginInfo.h"
#include "DragonflyReverbArtwork.hpp"

namespace Art = DragonflyReverbArtwork;

LabelledKnob::LabelledKnob(Widget* widget, ImageKnob::Callback *callback, NanoVG *nanoText, uint id, const char * numberFormat, int x, int y)
    : Widget(widget->getParentWindow())
{
  setWidth(Art::knobWidth + 20);
  setHeight(Art::knobHeight + 30);
  setAbsolutePos(x, y);

  fNumberFormat = numberFormat;
  fNanoText = nanoText;

  fKnob = new ImageKnob(this, Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ));
  fKnob->setId(id);
  fKnob->setAbsolutePos(x + 10, y + 14);
  fKnob->setRange(params[id].range_min, params[id].range_max);
  fKnob->setRotationAngle(300);
  fKnob->setCallback(callback);
}

void LabelledKnob::setDefault(float value) noexcept
{
    fKnob->setDefault(value);
}

float LabelledKnob::getValue() const noexcept
{
  return fKnob->getValue();
}

void LabelledKnob::setValue(float value)
{
  fKnob->setValue(value);
}

void LabelledKnob::onDisplay()
{
  fNanoText->beginFrame ( this );
  fNanoText->textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  fNanoText->fontSize ( 18 );
  fNanoText->fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  fNanoText->textBox ( 0, 7, getWidth(), params[fKnob->getId()].name, nullptr );

  char strBuf[32+1];
  strBuf[32] = '\0';
  std::snprintf ( strBuf, 32, fNumberFormat, fKnob->getValue() );

  fNanoText->fontSize ( 16 );
  fNanoText->fillColor ( Color ( 0.9f, 0.9f, 0.9f ) );
  fNanoText->textBox ( 0, getHeight() - 7, getWidth(), strBuf, nullptr );

  fNanoText->endFrame();
}
