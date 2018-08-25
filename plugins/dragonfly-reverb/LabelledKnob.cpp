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

LabelledKnob::LabelledKnob(Widget* widget, ImageKnob::Callback *callback, NanoVG *nanoText, uint id, int x, int y)
    : Widget(widget->getParentWindow())
{
  setWidth(Art::knobWidth);
  setHeight(Art::knobHeight + 20);
  setAbsolutePos(x, y);

  fNanoText = nanoText;

  fKnob = new ImageKnob(this, Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ));
  fKnob->setId(id);
  fKnob->setAbsolutePos(x, y + 10);
  fKnob->setRange(params[id].range_min, params[id].range_max);
  fKnob->setRotationAngle(300);
  fKnob->setCallback(callback);
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
  float r,g,b;
  r = 205.0f / 256;
  g = 241.0f / 256;
  b = 255.0f / 256;

  fNanoText->beginFrame ( this );
  fNanoText->fontSize ( 16 );
  fNanoText->textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  fNanoText->fillColor ( Color ( r, g, b ) );
  fNanoText->textBox ( 0, 5, 60.0f, params[fKnob->getId()].name, nullptr );

  char strBuf[32+1];
  strBuf[32] = '\0';
  std::snprintf ( strBuf, 32, " %4.2f m", fKnob->getValue() );
  fNanoText->textBox ( 0, getHeight() - 5, 60.0f, strBuf, nullptr );

  fNanoText->endFrame();
}
