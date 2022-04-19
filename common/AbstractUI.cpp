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

#include "AbstractUI.hpp"
#include "Bitstream_Vera_Sans_Regular.hpp"

DragonflyReverbAbstractUI::DragonflyReverbAbstractUI(
  uint width, uint height, const Param* params,
  const char* knobData, uint knobWidth, uint knobHeight,
  const char* questionData, uint questionWidth, uint questionHeight)
  : UI (width, height) {
    getWindow().setGeometryConstraints(width, height, true, true, true);
    NanoVG::FontId font  = nanoText.createFontFromMemory(
        "bitstream vera sans",
        font_bitstream_vera::bitstream_vera_sans_ttf,
        font_bitstream_vera::bitstream_vera_sans_ttf_size,
        false);
    nanoText.fontFaceId(font);
    this->params = params;
    this->knobImage = Image(knobData, knobWidth, knobHeight);

    displayAbout = false;
    aboutButton = new ImageButton(this, Image (questionData, questionWidth, questionHeight, kImageFormatBGRA));
    aboutButton->setCallback(this);

}

LabelledKnob * DragonflyReverbAbstractUI::createLabelledKnob(const Param *param, const char * numberFormat, int x, int y) {
  return new LabelledKnob(this, this, &knobImage, &nanoText, param, numberFormat, x, y);
}

void DragonflyReverbAbstractUI::imageButtonClicked(ImageButton* imageButton, int button)
{
  if (imageButton == aboutButton && button == 1) {
    aboutButton->setVisible(false);
    displayAbout = true;
    repaint();
  }
}

bool DragonflyReverbAbstractUI::onMouse(const MouseEvent& ev) {
  if (ev.press && displayAbout) {
    aboutButton->setVisible(true);
    displayAbout = false;
    repaint();
  }

  return UI::onMouse(ev);
}