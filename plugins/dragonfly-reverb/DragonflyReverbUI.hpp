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

#ifndef DRAGONFLY_REVERB_UI_HPP_INCLUDED
#define DRAGONFLY_REVERB_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "ImageWidgets.hpp"
#include "NanoVG.hpp"
#include "DistrhoPlugin.hpp"
#include "Spectrogram.hpp"
#include "LabelledKnob.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class DragonflyReverbUI : public UI,
		 public ImageKnob::Callback,
		 public ImageSlider::Callback
{
public:
    DragonflyReverbUI();

protected:
    // -------------------------------------------------------------------
    // DSP Callbacks

    void parameterChanged ( uint32_t index, float value ) override;
    void stateChanged(const char* key, const char* value) override;

    void programLoaded (uint32_t index);

    // -------------------------------------------------------------------
    // Widget Callbacks
//    void imageSwitchClicked ( ImageSwitch* ImageSwitch, bool down ) override;
    void imageKnobDragStarted ( ImageKnob* knob ) override;
    void imageKnobDragFinished ( ImageKnob* knob ) override;
    void imageKnobValueChanged ( ImageKnob* knob, float value ) override;
    void imageSliderDragStarted(ImageSlider* slider) override;
    void imageSliderDragFinished(ImageSlider* slider) override;
    void imageSliderValueChanged(ImageSlider* slider, float value) override;

    bool onMouse(const MouseEvent&) override;
    void onDisplay() override;
    void uiIdle() override;

private:
    Image fImgBackground,fImgTabOff,fImgTabOn,fImgQuestion;
    NanoVG fNanoText;
    NanoVG::FontId fNanoFont;

    ScopedPointer<LabelledKnob> fKnobSize, fKnobDecay, fKnobSpin, fKnobWander, fKnobPredelay, fKnobDiffuse, fKnobLowCut, fKnobLowXover, fKnobLowMult, fKnobHighCut, fKnobHighXover, fKnobHighMult;
    ScopedPointer<ImageSlider> fSliderDry_level, fSliderEarly_level, fSliderLate_level;
    ScopedPointer<ImageSwitch> fSwitchPrograms, fSwitchAbout, fSwitchResponse;
    ScopedPointer<Spectrogram> spectrogram;
    ScopedPointer<Image> spectrogramImage;

    int currentBank;
    int currentProgram[NUM_BANKS];

    bool displayAbout;

    DGL::Rectangle<int> rectSliders[3];

    DGL::Rectangle<int> rectDisplay;

    DGL::Rectangle<int> rectBanks[5];
    DGL::Rectangle<int> rectPrograms[5];

    DGL::Rectangle<int> rectAbout;

    void updatePresetDefaults();

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ( DragonflyReverbUI )
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif //
