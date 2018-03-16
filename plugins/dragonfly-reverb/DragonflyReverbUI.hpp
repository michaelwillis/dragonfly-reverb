/*
 * DISTRHO Kars Plugin, based on karplong by Chris Cannam.
 * Copyright (C) 2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef DRAGONFLY_REVERB_UI_HPP_INCLUDED
#define DRAGONFLY_REVERB_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "ImageWidgets.hpp"
#include "NanoVG.hpp"
#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class DragonflyReverbUI : public UI,
		 public ImageKnob::Callback,
//		 public ImageSwitch::Callback,
		 public ImageSlider::Callback
{
public:
    DragonflyReverbUI();

protected:
    // -------------------------------------------------------------------
    // DSP Callbacks

    void parameterChanged ( uint32_t index, float value ) override;
    void programLoaded (uint32_t index) override;

    // -------------------------------------------------------------------
    // Widget Callbacks
//    void imageSwitchClicked ( ImageSwitch* ImageSwitch, bool down ) override;
    void imageKnobDragStarted ( ImageKnob* knob ) override;
    void imageKnobDragFinished ( ImageKnob* knob ) override;
    void imageKnobValueChanged ( ImageKnob* knob, float value ) override;
    void imageSliderDragStarted(ImageSlider* slider) override;
    void imageSliderDragFinished(ImageSlider* slider) override;
    void imageSliderValueChanged(ImageSlider* slider, float value) override;

    // preset browser stuff
    bool onMouse(const MouseEvent&) override;



    void onDisplay() override;

private:
    Image fImgBackground,fImgTabOff,fImgTabOn;
    NanoVG fNanoText;
    NanoVG::FontId fNanoFont;
    
    ScopedPointer<ImageKnob> fKnobSize, fKnobPredelay, fKnobDiffuse, fKnobLow_cut, fKnobLow_xover, fKnobLow_mult, fKnobHigh_cut, fKnobHigh_xover, fKnobHigh_mult;
    ScopedPointer<ImageSlider> fSliderDry_level, fSliderEarly_level, fSliderLate_level;
    ScopedPointer<ImageSwitch> fSwitchPrograms, fSwitchAbout, fSwitchResponse;
    int currentProgram = 0;

    enum displayMode {
      displayResponse,
      displayPrograms,
      displayAbout
    };
    int currentDisplayMode = displayResponse;

    Rectangle<int> rectDisplay,rectTabResponse,rectTabPrograms,rectTabAbout;

    // boxes for programs
    Rectangle<int> rectPrograms[24]; // TODO check Programs constant

    // to fill sliders
    Rectangle<int> rectSliders[3];

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ( DragonflyReverbUI )
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif //
