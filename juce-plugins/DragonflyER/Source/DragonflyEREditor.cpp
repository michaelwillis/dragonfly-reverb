#include "DragonflyERProcessor.h"
#include "DragonflyEREditor.h"

#define HEADER_IMAGE_HEIGHT 118
#define HEADER_IMAGE_X 10
#define HEADER_IMAGE_Y 0
#define INFO_IMAGE_HEIGHT 140

#define KNOB_WIDTH 70
#define INSET 12
#define GUTTER 8
#define GAP 4
#define CBHEIGHT 20
#define TOPOFFSET 12
#define NUDGE 3

DragonflyEREditor::DragonflyEREditor (DragonflyERProcessor& p)
    : AudioProcessorEditor(&p)
    , processor(p)
    , infoButton(BinaryData::helpcircle_svg, Colour(61, 61, 61),
                 Colour(225, 206, 105), Colour(225, 206, 105).brighter(1.0f), Colour(220, 227, 233))
    , dryLevelKnob(DragonflyERParameters::dryLevelMin, DragonflyERParameters::dryLevelMax, DragonflyERParameters::dryLevelLabel)
    , labeledDryLevelKnob(DragonflyERParameters::dryLevelName, dryLevelKnob)
    , wetLevelKnob(DragonflyERParameters::wetLevelMin, DragonflyERParameters::wetLevelMax, DragonflyERParameters::wetLevelLabel)
    , labeledWetLevelKnob(DragonflyERParameters::wetLevelName, wetLevelKnob)
    , sizeKnob(DragonflyERParameters::sizeMin, DragonflyERParameters::sizeMax, DragonflyERParameters::sizeLabel)
    , labeledSizeKnob(DragonflyERParameters::sizeName, sizeKnob)
    , widthKnob(DragonflyERParameters::widthMin, DragonflyERParameters::widthMax, DragonflyERParameters::widthLabel)
    , labeledWidthKnob(DragonflyERParameters::widthName, widthKnob)
    , lowCutKnob(DragonflyERParameters::lowCutMin, DragonflyERParameters::lowCutMax, DragonflyERParameters::lowCutLabel)
    , labeledLowCutKnob(DragonflyERParameters::lowCutName, lowCutKnob)
    , highCutKnob(DragonflyERParameters::highCutMin, DragonflyERParameters::highCutMax, DragonflyERParameters::highCutLabel)
    , labeledHighCutKnob(DragonflyERParameters::highCutName, highCutKnob)
{
    setLookAndFeel(lookAndFeel);
    lookAndFeel->setColour(ResizableWindow::backgroundColourId, Colour(61, 61, 61));
    lookAndFeel->setColour(GroupComponent::outlineColourId, Colour(225, 206, 105));
    lookAndFeel->setColour(Slider::rotarySliderFillColourId, Colour(203, 128, 22));
    lookAndFeel->setColour(Slider::rotarySliderOutlineColourId, Colour(220, 227, 233));

    headerImage = ImageCache::getFromMemory(BinaryData::erheader_png, BinaryData::erheader_pngSize);

    mainGroup.setText("Reflection Type");
    addAndMakeVisible(&mainGroup);

    infoButton.onClick = [this]() { infoImage.setVisible(true); };
    addAndMakeVisible(infoButton);

    dryLevelKnob.setDoubleClickReturnValue(true, double(DragonflyERParameters::dryLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDryLevelKnob);
    wetLevelKnob.setDoubleClickReturnValue(true, double(DragonflyERParameters::wetLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWetLevelKnob);
    progIndexLabel.setText("Program", dontSendNotification);
    progIndexLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&progIndexLabel);
    progIndexCombo.setEditableText(false);
    progIndexCombo.setJustificationType(Justification::centredLeft);
    DragonflyERParameters::populateProgramsComboBox(progIndexCombo);
    addAndMakeVisible(progIndexCombo);
    sizeKnob.setDoubleClickReturnValue(true, double(DragonflyERParameters::sizeDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSizeKnob);
    widthKnob.setDoubleClickReturnValue(true, double(DragonflyERParameters::widthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWidthKnob);
    lowCutKnob.setDoubleClickReturnValue(true, double(DragonflyERParameters::lowCutDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLowCutKnob);
    highCutKnob.setDoubleClickReturnValue(true, double(DragonflyERParameters::highCutDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledHighCutKnob);

    // Add infoImage last so when it's displayed, it will cover the other controls
    infoImage.setImage(ImageCache::getFromMemory(BinaryData::erinfo_png, BinaryData::erinfo_pngSize));
    infoImage.onMouseDown = [this]() { infoImage.setVisible(false); };
    addChildComponent(infoImage);

    processor.parameters.attachControls(
        dryLevelKnob,
        wetLevelKnob,
        progIndexCombo,
        sizeKnob,
        widthKnob,
        lowCutKnob,
        highCutKnob );

    setSize (2*INSET + 2*GUTTER + 6*KNOB_WIDTH + 5*GAP, HEADER_IMAGE_HEIGHT + INFO_IMAGE_HEIGHT);
}

DragonflyEREditor::~DragonflyEREditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void DragonflyEREditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(HEADER_IMAGE_HEIGHT);
    infoImage.setBounds(bounds);

    auto groupArea = bounds.reduced(INSET);
    mainGroup.setBounds(groupArea);
    auto cbArea = groupArea;
    cbArea.removeFromLeft(126);
    cbArea = cbArea.removeFromTop(CBHEIGHT);
    cbArea.removeFromRight(16);
    infoButton.setBounds(cbArea.removeFromRight(cbArea.getHeight()).withSizeKeepingCentre(24, 24));
    cbArea.setY(cbArea.getY() - NUDGE);
    progIndexCombo.setBounds(cbArea.removeFromLeft(140));

    groupArea.reduce(2 * GUTTER, GUTTER);
    groupArea.removeFromTop(TOPOFFSET);
    int width = (groupArea.getWidth() - 5 * GAP) / 6;
    labeledDryLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledWetLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledSizeKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledWidthKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLowCutKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledHighCutKnob.setBounds(groupArea);
}

void DragonflyEREditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));

    g.drawImageAt(headerImage, HEADER_IMAGE_X, HEADER_IMAGE_Y);
}
