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

#ifndef SELECTION_HPP_INCLUDED
#define SELECTION_HPP_INCLUDED

#include "Image.hpp"
#include "NanoVG.hpp"
#include "Widget.hpp"

class Selection : public SubWidget
{
public:
  class Callback
  {
  public:
    virtual ~Callback() {}
    virtual void selectionClicked(Selection* selection, int selectedOption) = 0;
  };

  Selection(
    Widget* parent,
    Selection::Callback* callback,
    int width,
    NanoVG* nanoText,
    int numberOfOptions);

  ~Selection();

  void setTextAlign(int align);
  void setSelectedImage(Image *image);
  void setUnselectedImage(Image *image);

  void setOptions(char const* options[]);
  void setSelectedOption(int option);

protected:
  void onDisplay() override;
  bool onMouse(const MouseEvent&) override;

private:
  Selection::Callback* callback;
  Image* selectedImage;
  Image* unselectedImage;
  int align;
  NanoVG* nanoText;
  int numberOfOptions;
  int selectedOption;
  char const** options;
};

#endif
