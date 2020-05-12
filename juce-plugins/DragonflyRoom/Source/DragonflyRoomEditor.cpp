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

#include "DragonflyRoomProcessor.h"
#include "DragonflyRoomEditor.h"

#define HEADER_IMAGE_HEIGHT (118 + 4)
#define HEADER_IMAGE_WIDTH 350
#define HEADER_IMAGE_X 6
#define HEADER_IMAGE_Y 8
#define INFO_IMAGE_HEIGHT 140

#define KNOB_WIDTH 70
#define INSET 12
#define GUTTER 8
#define GAP 4
#define CBHEIGHT 20
#define TOPOFFSET 12
#define NUDGE 3

#define BACKGROUND_COLOUR Colour(61, 61, 61)
#define ICON_COLOUR       Colour(172, 202, 231)
//#define KNOB_COLOUR       Colour(79, 129, 162)
#define RING_COLOUR       Colour(223, 230, 225)

DragonflyRoomEditor::DragonflyRoomEditor (DragonflyRoomProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , infoButton(BinaryData::helpcircle_svg, BACKGROUND_COLOUR, ICON_COLOUR, ICON_COLOUR.brighter(1.0f), RING_COLOUR)
    , dryLevelKnob(DragonflyRoomParameters::dryLevelMin, DragonflyRoomParameters::dryLevelMax, DragonflyRoomParameters::dryLevelLabel)
    , labeledDryLevelKnob(DragonflyRoomParameters::dryLevelName, dryLevelKnob)
    , earlyLevelKnob(DragonflyRoomParameters::earlyLevelMin, DragonflyRoomParameters::earlyLevelMax, DragonflyRoomParameters::earlyLevelLabel)
    , labeledEarlyLevelKnob(DragonflyRoomParameters::earlyLevelName, earlyLevelKnob)
    , earlySendKnob(DragonflyRoomParameters::earlySendMin, DragonflyRoomParameters::earlySendMax, DragonflyRoomParameters::earlySendLabel)
    , labeledEarlySendKnob(DragonflyRoomParameters::earlySendName, earlySendKnob)
    , lateLevelKnob(DragonflyRoomParameters::lateLevelMin, DragonflyRoomParameters::lateLevelMax, DragonflyRoomParameters::lateLevelLabel)
    , labeledLateLevelKnob(DragonflyRoomParameters::lateLevelName, lateLevelKnob)
    , sizeKnob(DragonflyRoomParameters::sizeMin, DragonflyRoomParameters::sizeMax, DragonflyRoomParameters::sizeLabel)
    , labeledSizeKnob(DragonflyRoomParameters::sizeName, sizeKnob)
    , widthKnob(DragonflyRoomParameters::widthMin, DragonflyRoomParameters::widthMax, DragonflyRoomParameters::widthLabel)
    , labeledWidthKnob(DragonflyRoomParameters::widthName, widthKnob)
    , predelayKnob(DragonflyRoomParameters::predelayMin, DragonflyRoomParameters::predelayMax, DragonflyRoomParameters::predelayLabel)
    , labeledPredelayKnob(DragonflyRoomParameters::predelayName, predelayKnob)
    , decayKnob(DragonflyRoomParameters::decayMin, DragonflyRoomParameters::decayMax, DragonflyRoomParameters::decayLabel)
    , labeledDecayKnob(DragonflyRoomParameters::decayName, decayKnob)
    , diffuseKnob(DragonflyRoomParameters::diffuseMin, DragonflyRoomParameters::diffuseMax, DragonflyRoomParameters::diffuseLabel)
    , labeledDiffuseKnob(DragonflyRoomParameters::diffuseName, diffuseKnob)
    , spinKnob(DragonflyRoomParameters::spinMin, DragonflyRoomParameters::spinMax, DragonflyRoomParameters::spinLabel)
    , labeledSpinKnob(DragonflyRoomParameters::spinName, spinKnob)
    , wanderKnob(DragonflyRoomParameters::wanderMin, DragonflyRoomParameters::wanderMax, DragonflyRoomParameters::wanderLabel)
    , labeledWanderKnob(DragonflyRoomParameters::wanderName, wanderKnob)
    , highCutKnob(DragonflyRoomParameters::highCutMin, DragonflyRoomParameters::highCutMax, DragonflyRoomParameters::highCutLabel)
    , labeledHighCutKnob(DragonflyRoomParameters::highCutName, highCutKnob)
    , earlyDampKnob(DragonflyRoomParameters::earlyDampMin, DragonflyRoomParameters::earlyDampMax, DragonflyRoomParameters::earlyDampLabel)
    , labeledEarlyDampKnob(DragonflyRoomParameters::earlyDampName, earlyDampKnob)
    , lateDampKnob(DragonflyRoomParameters::lateDampMin, DragonflyRoomParameters::lateDampMax, DragonflyRoomParameters::lateDampLabel)
    , labeledLateDampKnob(DragonflyRoomParameters::lateDampName, lateDampKnob)
    , lowBoostKnob(DragonflyRoomParameters::lowBoostMin, DragonflyRoomParameters::lowBoostMax, DragonflyRoomParameters::lowBoostLabel)
    , labeledLowBoostKnob(DragonflyRoomParameters::lowBoostName, lowBoostKnob)
    , boostFreqKnob(DragonflyRoomParameters::boostFreqMin, DragonflyRoomParameters::boostFreqMax, DragonflyRoomParameters::boostFreqLabel)
    , labeledBoostFreqKnob(DragonflyRoomParameters::boostFreqName, boostFreqKnob)
    , lowCutKnob(DragonflyRoomParameters::lowCutMin, DragonflyRoomParameters::lowCutMax, DragonflyRoomParameters::lowCutLabel)
    , labeledLowCutKnob(DragonflyRoomParameters::lowCutName, lowCutKnob)
{
    setLookAndFeel(lookAndFeel);
    lookAndFeel->setColour(ResizableWindow::backgroundColourId, BACKGROUND_COLOUR);
    lookAndFeel->setColour(GroupComponent::outlineColourId, ICON_COLOUR);
    lookAndFeel->setColour(Slider::rotarySliderOutlineColourId, RING_COLOUR);

    headerImage = ImageCache::getFromMemory(BinaryData::roomheader_png, BinaryData::roomheader_pngSize);

    topGroup.setText("Preset");
    addAndMakeVisible(topGroup);
    addAndMakeVisible(bottomGroup);

    infoButton.onClick = [this]() { infoImage.setVisible(true); };
    addAndMakeVisible(infoButton);

    processor.populateProgramsComboBox(presetsCombo);
    presetsCombo.setSelectedItemIndex(processor.getCurrentProgram(), dontSendNotification);
    presetsCombo.onChange = [this]()
    {
        int progIndex = presetsCombo.getSelectedItemIndex();
        if (progIndex != processor.getCurrentProgram())
            processor.setCurrentProgram(progIndex);
    };
    addAndMakeVisible(presetsCombo);

    dryLevelKnob.bodyColour = BasicLookAndFeel::darkBlue;
    dryLevelKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::dryLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDryLevelKnob);
    earlyLevelKnob.bodyColour = BasicLookAndFeel::darkBlue;
    earlyLevelKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::earlyLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEarlyLevelKnob);
    earlySendKnob.bodyColour = BasicLookAndFeel::darkBlue;
    earlySendKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::earlySendDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEarlySendKnob);
    lateLevelKnob.bodyColour = BasicLookAndFeel::darkBlue;
    lateLevelKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::lateLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLateLevelKnob);
    sizeKnob.bodyColour = BasicLookAndFeel::lightBlue;
    sizeKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::sizeDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSizeKnob);
    widthKnob.bodyColour = BasicLookAndFeel::green;
    widthKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::widthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWidthKnob);
    predelayKnob.bodyColour = BasicLookAndFeel::lightBlue;
    predelayKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::predelayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledPredelayKnob);
    decayKnob.bodyColour = BasicLookAndFeel::lightBlue;
    decayKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::decayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDecayKnob);
    diffuseKnob.bodyColour = BasicLookAndFeel::lightBlue;
    diffuseKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::diffuseDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDiffuseKnob);
    spinKnob.bodyColour = BasicLookAndFeel::purple;
    spinKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::spinDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSpinKnob);
    wanderKnob.bodyColour = BasicLookAndFeel::purple;
    wanderKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::wanderDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWanderKnob);
    highCutKnob.bodyColour = BasicLookAndFeel::orange;
    highCutKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::highCutDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledHighCutKnob);
    earlyDampKnob.bodyColour = BasicLookAndFeel::orange;
    earlyDampKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::earlyDampDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEarlyDampKnob);
    lateDampKnob.bodyColour = BasicLookAndFeel::orange;
    lateDampKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::lateDampDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLateDampKnob);
    lowBoostKnob.bodyColour = BasicLookAndFeel::orange;
    lowBoostKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::lowBoostDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLowBoostKnob);
    boostFreqKnob.bodyColour = BasicLookAndFeel::orange;
    boostFreqKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::boostFreqDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledBoostFreqKnob);
    lowCutKnob.bodyColour = BasicLookAndFeel::orange;
    lowCutKnob.setDoubleClickReturnValue(true, double(DragonflyRoomParameters::lowCutDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLowCutKnob);

    // Add infoImage last so when it's displayed, it will cover the other controls
    infoImage.setImage(ImageCache::getFromMemory(BinaryData::DF_Room_JL_png, BinaryData::DF_Room_JL_pngSize));
    infoImage.onMouseDown = [this]() { infoImage.setVisible(false); };
    addChildComponent(infoImage);

    processor.parameters.attachControls(
        dryLevelKnob,
        earlyLevelKnob,
        earlySendKnob,
        lateLevelKnob,
        sizeKnob,
        widthKnob,
        predelayKnob,
        decayKnob,
        diffuseKnob,
        spinKnob,
        wanderKnob,
        highCutKnob,
        earlyDampKnob,
        lateDampKnob,
        lowBoostKnob,
        boostFreqKnob,
        lowCutKnob );

    processor.addChangeListener(this);

    setSize(2 * INSET + 2 * GUTTER + 11 * KNOB_WIDTH + 10 * GAP, HEADER_IMAGE_HEIGHT + INFO_IMAGE_HEIGHT);
}

DragonflyRoomEditor::~DragonflyRoomEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void DragonflyRoomEditor::changeListenerCallback(ChangeBroadcaster*)
{
    presetsCombo.setSelectedItemIndex(processor.getCurrentProgram(), dontSendNotification);
}

void DragonflyRoomEditor::resized()
{
    auto bounds = getLocalBounds();
    infoImage.setBounds(bounds);
    bounds.removeFromTop(HEADER_IMAGE_HEIGHT);

    auto groupArea = bounds.reduced(INSET);
    bottomGroup.setBounds(groupArea);
    groupArea.reduce(2 * GUTTER, GUTTER);
    groupArea.removeFromTop(TOPOFFSET);
    int width = (groupArea.getWidth() - 10 * GAP) / 11;
    labeledWidthKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledPredelayKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledSizeKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledDecayKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledDiffuseKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLowCutKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLowBoostKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledBoostFreqKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledHighCutKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledSpinKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledWanderKnob.setBounds(groupArea.removeFromLeft(width));

    bounds = getLocalBounds().removeFromTop(INFO_IMAGE_HEIGHT);
    bounds.removeFromLeft(HEADER_IMAGE_WIDTH);
    groupArea = bounds.reduced(INSET);
    topGroup.setBounds(groupArea);
    auto cbArea = groupArea;
    cbArea.removeFromLeft(70);
    cbArea = cbArea.removeFromTop(CBHEIGHT);
    cbArea.removeFromRight(16);
    infoButton.setBounds(cbArea.removeFromRight(cbArea.getHeight()).withSizeKeepingCentre(24, 24));
    cbArea.setY(cbArea.getY() - NUDGE);
    presetsCombo.setBounds(cbArea.removeFromLeft(180));

    groupArea.reduce(2 * GUTTER, GUTTER);
    groupArea.removeFromTop(TOPOFFSET);
    width = (groupArea.getWidth() - 5 * GAP) / 6;
    labeledDryLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledEarlyLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledEarlySendKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledEarlyDampKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLateLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLateDampKnob.setBounds(groupArea);
}

void DragonflyRoomEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));

    g.drawImageAt(headerImage, HEADER_IMAGE_X, HEADER_IMAGE_Y);
}
