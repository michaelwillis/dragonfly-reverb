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

#include "DragonflyHallProcessor.h"
#include "DragonflyHallEditor.h"

#define HEADER_IMAGE_HEIGHT 118
#define HEADER_IMAGE_WIDTH (316 + 12)
#define HEADER_IMAGE_X 6
#define HEADER_IMAGE_Y 8
#define INFO_IMAGE_HEIGHT 250
#define GROUP_HEIGHT 140

#define KNOB_WIDTH 70
#define INSET 12
#define GUTTER 8
#define GAP 24
#define SMALL_GAP 10
#define CBHEIGHT 20
#define TOPOFFSET 12
#define NUDGE 3

#define BACKGROUND_COLOUR Colour(61, 61, 61)
#define ICON_COLOUR       Colour(185, 149, 91)
#define KNOB_COLOUR       Colour(80, 149, 85)
#define RING_COLOUR       Colour(231, 230, 231)

DragonflyHallEditor::DragonflyHallEditor (DragonflyHallProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , infoButton(BinaryData::helpcircle_svg, BACKGROUND_COLOUR, ICON_COLOUR, ICON_COLOUR.brighter(1.0f), RING_COLOUR)
    , dryLevelKnob(DragonflyHallParameters::dryLevelMin, DragonflyHallParameters::dryLevelMax, DragonflyHallParameters::dryLevelLabel)
    , labeledDryLevelKnob(DragonflyHallParameters::dryLevelName, dryLevelKnob)
    , earlyLevelKnob(DragonflyHallParameters::earlyLevelMin, DragonflyHallParameters::earlyLevelMax, DragonflyHallParameters::earlyLevelLabel)
    , labeledEarlyLevelKnob(DragonflyHallParameters::earlyLevelName, earlyLevelKnob)
    , lateLevelKnob(DragonflyHallParameters::lateLevelMin, DragonflyHallParameters::lateLevelMax, DragonflyHallParameters::lateLevelLabel)
    , labeledLateLevelKnob(DragonflyHallParameters::lateLevelName, lateLevelKnob)
    , sizeKnob(DragonflyHallParameters::sizeMin, DragonflyHallParameters::sizeMax, DragonflyHallParameters::sizeLabel)
    , labeledSizeKnob(DragonflyHallParameters::sizeName, sizeKnob)
    , widthKnob(DragonflyHallParameters::widthMin, DragonflyHallParameters::widthMax, DragonflyHallParameters::widthLabel)
    , labeledWidthKnob(DragonflyHallParameters::widthName, widthKnob)
    , predelayKnob(DragonflyHallParameters::predelayMin, DragonflyHallParameters::predelayMax, DragonflyHallParameters::predelayLabel)
    , labeledPredelayKnob(DragonflyHallParameters::predelayName, predelayKnob)
    , diffuseKnob(DragonflyHallParameters::diffuseMin, DragonflyHallParameters::diffuseMax, DragonflyHallParameters::diffuseLabel)
    , labeledDiffuseKnob(DragonflyHallParameters::diffuseName, diffuseKnob)
    , lowCutKnob(DragonflyHallParameters::lowCutMin, DragonflyHallParameters::lowCutMax, DragonflyHallParameters::lowCutLabel)
    , labeledLowCutKnob(DragonflyHallParameters::lowCutName, lowCutKnob)
    , lowCrossKnob(DragonflyHallParameters::lowCrossMin, DragonflyHallParameters::lowCrossMax, DragonflyHallParameters::lowCrossLabel)
    , labeledLowCrossKnob(DragonflyHallParameters::lowCrossName, lowCrossKnob)
    , lowMultKnob(DragonflyHallParameters::lowMultMin, DragonflyHallParameters::lowMultMax, DragonflyHallParameters::lowMultLabel)
    , labeledLowMultKnob(DragonflyHallParameters::lowMultName, lowMultKnob)
    , highCutKnob(DragonflyHallParameters::highCutMin, DragonflyHallParameters::highCutMax, DragonflyHallParameters::highCutLabel)
    , labeledHighCutKnob(DragonflyHallParameters::highCutName, highCutKnob)
    , highCrossKnob(DragonflyHallParameters::highCrossMin, DragonflyHallParameters::highCrossMax, DragonflyHallParameters::highCrossLabel)
    , labeledHighCrossKnob(DragonflyHallParameters::highCrossName, highCrossKnob)
    , highMultKnob(DragonflyHallParameters::highMultMin, DragonflyHallParameters::highMultMax, DragonflyHallParameters::highMultLabel)
    , labeledHighMultKnob(DragonflyHallParameters::highMultName, highMultKnob)
    , spinKnob(DragonflyHallParameters::spinMin, DragonflyHallParameters::spinMax, DragonflyHallParameters::spinLabel)
    , labeledSpinKnob(DragonflyHallParameters::spinName, spinKnob)
    , wanderKnob(DragonflyHallParameters::wanderMin, DragonflyHallParameters::wanderMax, DragonflyHallParameters::wanderLabel)
    , labeledWanderKnob(DragonflyHallParameters::wanderName, wanderKnob)
    , decayKnob(DragonflyHallParameters::decayMin, DragonflyHallParameters::decayMax, DragonflyHallParameters::decayLabel)
    , labeledDecayKnob(DragonflyHallParameters::decayName, decayKnob)
    , earlySendKnob(DragonflyHallParameters::earlySendMin, DragonflyHallParameters::earlySendMax, DragonflyHallParameters::earlySendLabel)
    , labeledEarlySendKnob(DragonflyHallParameters::earlySendName, earlySendKnob)
    , modulationKnob(DragonflyHallParameters::modulationMin, DragonflyHallParameters::modulationMax, DragonflyHallParameters::modulationLabel)
    , labeledModulationKnob(DragonflyHallParameters::modulationName, modulationKnob)
{
    setLookAndFeel(lookAndFeel);
    lookAndFeel->setColour(ResizableWindow::backgroundColourId, BACKGROUND_COLOUR);
    lookAndFeel->setColour(GroupComponent::outlineColourId, ICON_COLOUR);
    lookAndFeel->setColour(Slider::rotarySliderFillColourId, KNOB_COLOUR);
    lookAndFeel->setColour(Slider::rotarySliderOutlineColourId, RING_COLOUR);

    headerImage = ImageCache::getFromMemory(BinaryData::hallheader_png, BinaryData::hallheader_pngSize);

    topGroup.setText("Preset");
    addAndMakeVisible(topGroup);
    addAndMakeVisible(middleGroup);
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

    dryLevelKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::dryLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDryLevelKnob);
    earlyLevelKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::earlyLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEarlyLevelKnob);
    lateLevelKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::lateLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLateLevelKnob);
    sizeKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::sizeDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSizeKnob);
    widthKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::widthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWidthKnob);
    predelayKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::predelayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledPredelayKnob);
    diffuseKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::diffuseDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDiffuseKnob);
    lowCutKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::lowCutDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLowCutKnob);
    lowCrossKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::lowCrossDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLowCrossKnob);
    lowMultKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::lowMultDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLowMultKnob);
    highCutKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::highCutDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledHighCutKnob);
    highCrossKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::highCrossDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledHighCrossKnob);
    highMultKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::highMultDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledHighMultKnob);
    spinKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::spinDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSpinKnob);
    wanderKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::wanderDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWanderKnob);
    decayKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::decayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDecayKnob);
    earlySendKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::earlySendDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEarlySendKnob);
    modulationKnob.setDoubleClickReturnValue(true, double(DragonflyHallParameters::modulationDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledModulationKnob);

    // Add infoImage last so when it's displayed, it will cover the other controls
    infoImage.setImage(ImageCache::getFromMemory(BinaryData::hallinfo_png, BinaryData::hallinfo_pngSize));
    infoImage.onMouseDown = [this]() { infoImage.setVisible(false); };
    addChildComponent(infoImage);

    processor.parameters.attachControls(
        dryLevelKnob,
        earlyLevelKnob,
        lateLevelKnob,
        sizeKnob,
        widthKnob,
        predelayKnob,
        diffuseKnob,
        lowCutKnob,
        lowCrossKnob,
        lowMultKnob,
        highCutKnob,
        highCrossKnob,
        highMultKnob,
        spinKnob,
        wanderKnob,
        decayKnob,
        earlySendKnob,
        modulationKnob);

    processor.addChangeListener(this);

    setSize(2 * INSET + 2 * GUTTER + 7 * KNOB_WIDTH + 6 * GAP, HEADER_IMAGE_HEIGHT + 2 * GROUP_HEIGHT - 16);
}

DragonflyHallEditor::~DragonflyHallEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void DragonflyHallEditor::changeListenerCallback(ChangeBroadcaster*)
{
    presetsCombo.setSelectedItemIndex(processor.getCurrentProgram(), dontSendNotification);
}

void DragonflyHallEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(HEADER_IMAGE_HEIGHT);

    bounds = getLocalBounds().removeFromTop(GROUP_HEIGHT);
    bounds.removeFromLeft(HEADER_IMAGE_WIDTH);
    auto groupArea = bounds.reduced(INSET);
    topGroup.setBounds(groupArea);
    auto cbArea = groupArea;
    cbArea.removeFromLeft(70);
    cbArea = cbArea.removeFromTop(CBHEIGHT);
    cbArea.removeFromRight(16);
    infoButton.setBounds(cbArea.removeFromRight(cbArea.getHeight()).withSizeKeepingCentre(24, 24));
    cbArea.setY(cbArea.getY() - NUDGE);
    presetsCombo.setBounds(cbArea.removeFromLeft(210));

    groupArea.reduce(2 * GUTTER, GUTTER);
    groupArea.removeFromTop(TOPOFFSET);
    int width = (groupArea.getWidth() - 3 * SMALL_GAP) / 4;
    labeledDryLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(SMALL_GAP);
    labeledEarlyLevelKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(SMALL_GAP);
    labeledEarlySendKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(SMALL_GAP);
    labeledLateLevelKnob.setBounds(groupArea.removeFromLeft(width));

    bounds = getLocalBounds();
    bounds.removeFromTop(GROUP_HEIGHT - 16);
    groupArea = bounds.removeFromTop(GROUP_HEIGHT).reduced(INSET);
    middleGroup.setBounds(groupArea);
    groupArea.reduce(2 * GUTTER, GUTTER);
    groupArea.removeFromTop(TOPOFFSET);
    width = (groupArea.getWidth() - 6 * GAP) / 7;
    labeledSizeKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledDecayKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledPredelayKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledDiffuseKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledModulationKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledSpinKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledWanderKnob.setBounds(groupArea.removeFromLeft(width));

    infoImage.setBounds(getLocalBounds().removeFromBottom(INFO_IMAGE_HEIGHT));
    bounds = getLocalBounds();
    bounds.removeFromTop(2 * (GROUP_HEIGHT - 16));
    groupArea = bounds.removeFromTop(GROUP_HEIGHT).reduced(INSET);
    cbArea = groupArea.withHeight(CBHEIGHT);
    bottomGroup.setBounds(groupArea);
    cbArea.removeFromRight(16);
    infoButton.setBounds(cbArea.removeFromRight(cbArea.getHeight()).withSizeKeepingCentre(24, 24));

    groupArea.reduce(2 * GUTTER, GUTTER);
    groupArea.removeFromTop(TOPOFFSET);
    width = (groupArea.getWidth() - 6 * GAP) / 7;
    labeledWidthKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLowCutKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLowCrossKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledLowMultKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledHighCrossKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledHighMultKnob.setBounds(groupArea.removeFromLeft(width));
    groupArea.removeFromLeft(GAP);
    labeledHighCutKnob.setBounds(groupArea.removeFromLeft(width));
}

void DragonflyHallEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));

    g.drawImageAt(headerImage, HEADER_IMAGE_X, HEADER_IMAGE_Y);
}
