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
