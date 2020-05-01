/*
 * Dragonfly Reverb
 * copyright (c) 2019-2020 Michael Willis, Rob van den Berg, Shane Dunne
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

#include "IconButton.h"

static String codeForSvg(const Colour colour)
{
    return String("#") + colour.toDisplayString(false);
}

IconButton::IconButton(const char* iconSvg, Colour fillColour, Colour normalColour, Colour overColour, Colour downColour)
    : DrawableButton({}, ButtonStyle::ImageFitted)
    , backgroundColour(fillColour)
{
    setWantsKeyboardFocus(false);
    setMouseClickGrabsKeyboardFocus(false);

    std::unique_ptr<XmlElement> svgXml(XmlDocument::parse(iconSvg));

    svgXml->setAttribute("stroke", codeForSvg(normalColour));
    std::unique_ptr<Drawable> normal(Drawable::createFromSVG(*svgXml));

    svgXml->setAttribute("stroke", codeForSvg(overColour));
    std::unique_ptr<Drawable> over(Drawable::createFromSVG(*svgXml));

    svgXml->setAttribute("stroke", codeForSvg(downColour));
    std::unique_ptr<Drawable> down(Drawable::createFromSVG(*svgXml));

    setImages(normal.get(), over.get(), down.get(), nullptr, normal.get(), over.get(), down.get());
}

void IconButton::paint(Graphics& g)
{
    g.fillAll(backgroundColour);
    DrawableButton::paint(g);
}
