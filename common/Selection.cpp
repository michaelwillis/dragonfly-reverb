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

#include "Selection.hpp"

#define ROW_HEIGHT 21
#define TEXT_X_OFFSET 5
#define TEXT_Y_OFFSET 11

Selection::Selection(
    Widget* parent,
    Selection::Callback* callback,
    int width,
    NanoVG* nanoText,
    int numberOfOptions) : SubWidget(parent)
{
    setHeight(numberOfOptions * ROW_HEIGHT);
    setWidth(width);

    this->callback = callback;
    this->nanoText = nanoText;
    this->align = NanoVG::ALIGN_LEFT;
    this->numberOfOptions = numberOfOptions;
    this->options = new char const*[numberOfOptions];
    this->selectedOption = 0;

    this->selectedImage = nullptr;
    this->unselectedImage = nullptr;
}

Selection::~Selection()
{
  delete[] this->options;
}

void Selection::setSelectedImage(Image *image)
{
  selectedImage = image;
}

void Selection::setUnselectedImage(Image *image)
{
  unselectedImage = image;
}

void Selection::setTextAlign(int align)
{
  this->align = align;
}

void Selection::setOptionName(int option, char const* name)
{
  this->options[option] = name;
}

void Selection::setSelectedOption(int selectedOption)
{
  this->selectedOption = selectedOption;
  repaint();
}

void Selection::onDisplay()
{
  const GraphicsContext& context(getGraphicsContext());

  nanoText->beginFrame(this);
  nanoText->fontSize ( 15 );
  nanoText->textAlign(this->align | NanoVG::ALIGN_MIDDLE);

  Color bright = Color ( 0.90f, 0.95f, 1.00f );
  Color dim    = Color ( 0.65f, 0.65f, 0.65f );

  for (int i = 0; i < numberOfOptions; i++) {
    int y = i * ROW_HEIGHT;
    if (selectedOption == i) {
      if (selectedImage != nullptr) {
        selectedImage->drawAt(context, 0, y);
      }
      nanoText->fillColor(bright);
    } else {
      if (unselectedImage != nullptr) {
        unselectedImage->drawAt(context, 0, y);
      }
      nanoText->fillColor(dim);
    }
    nanoText->textBox(
      this->align == NanoVG::ALIGN_RIGHT ? -TEXT_X_OFFSET : TEXT_X_OFFSET,
      y + TEXT_Y_OFFSET,
      getWidth(), options[i], nullptr);
  }

  nanoText->endFrame();
}

bool Selection::onMouse(const MouseEvent& ev)
{
  if (ev.press && ev.button == 1 && this->contains(ev.pos)) {
    int option = (ev.pos.getY() / ROW_HEIGHT);
    callback->selectionClicked(this, option);
    return true;
  }

  return false;
}
