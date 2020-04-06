#include "BasicKnob.h"

BasicKnob::BasicKnob(float minValue, float maxValue, const String& unit)
    : Slider()
    , unitOfMeasure(unit)
{
    setTextBoxStyle(NoTextBox, false, 0, 0);
    setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    setRange(minValue, maxValue);
}

double BasicKnob::getValueFromText(const String& text)
{
    auto valueText = text.upToFirstOccurrenceOf(" ", false, false).trim();
    return valueText.getDoubleValue();
}

String BasicKnob::getTextFromValue(double value)
{
    String text = String(value);
    if (unitOfMeasure.isNotEmpty()) text += " " + unitOfMeasure;
    return text;
}
