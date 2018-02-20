/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
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


#include "DistrhoUI.hpp"
#include "DragonflyReverbUI.hpp"
#include "DragonflyReverbArtwork.hpp"
#include "DistrhoPluginInfo.h"

START_NAMESPACE_DISTRHO

namespace Art = DragonflyReverbArtwork;

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : UI ( Art::backgroundWidth, Art::backgroundHeight ),
    fImgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGR )
{
  // knobs
  
  fKnobSize = new ImageKnob ( this,
                              Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobSize->setId ( paramSize );
  fKnobSize->setAbsolutePos ( 279, 79 );
  fKnobSize->setRange ( 0.0f, 1.0f );
  fKnobSize->setDefault ( 0.0f );
  fKnobSize->setValue ( 0.0f );
  fKnobSize->setCallback ( this );

  //
  fKnobPredelay = new ImageKnob ( this,
                                  Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobPredelay->setId ( paramPredelay );
  fKnobPredelay->setAbsolutePos ( 560, 255 );
  fKnobPredelay->setRange ( 0.05f, 1.0f );
  fKnobPredelay->setDefault ( 0.05f );
  fKnobPredelay->setValue ( 0.05f );
  fKnobPredelay->setRotationAngle ( 300 );
  fKnobPredelay->setCallback ( this );

  fKnobDiffuse = new ImageKnob ( this,
                                 Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobDiffuse->setId ( paramDiffuse );
  fKnobDiffuse->setAbsolutePos ( 637, 255 );
  fKnobDiffuse->setRange ( 0.05f, 1.0f );
  fKnobDiffuse->setDefault ( 0.0f );
  fKnobDiffuse->setValue ( 0.05f );
  fKnobDiffuse->setRotationAngle ( 300 );
  fKnobDiffuse->setCallback ( this );

  fKnobLow_cut = new ImageKnob ( this,
                                 Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobLow_cut->setId ( paramLow_cut );
  fKnobLow_cut->setAbsolutePos ( 715, 255 );
  fKnobLow_cut->setRange ( 0.0f, 1.0f );
  fKnobLow_cut->setDefault ( 1.0f );
  fKnobLow_cut->setValue ( 1.0f );
  fKnobLow_cut->setRotationAngle ( 300 );
  fKnobLow_cut->setCallback ( this );

  fKnobLow_xover = new ImageKnob ( this,
                                   Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobLow_xover->setId ( paramLow_xover );
  fKnobLow_xover->setAbsolutePos ( 792, 255 );
  fKnobLow_xover->setRange ( 0.05f, 1.0f );
  fKnobLow_xover->setDefault ( 0.05f );
  fKnobLow_xover->setValue ( 0.05f );
  fKnobLow_xover->setRotationAngle ( 300 );
  fKnobLow_xover->setCallback ( this );

  fKnobLow_mult = new ImageKnob ( this,
                                  Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobLow_mult->setId ( paramLow_mult );
  fKnobLow_mult->setAbsolutePos ( 792, 255 );
  fKnobLow_mult->setRange ( 0.05f, 1.0f );
  fKnobLow_mult->setDefault ( 0.05f );
  fKnobLow_mult->setValue ( 0.05f );
  fKnobLow_mult->setRotationAngle ( 300 );
  fKnobLow_mult->setCallback ( this );

  fKnobHigh_cut = new ImageKnob ( this,
                                  Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobHigh_cut->setId ( paramHigh_cut );
  fKnobHigh_cut->setAbsolutePos ( 715, 255 );
  fKnobHigh_cut->setRange ( 0.0f, 1.0f );
  fKnobHigh_cut->setDefault ( 1.0f );
  fKnobHigh_cut->setValue ( 1.0f );
  fKnobHigh_cut->setRotationAngle ( 300 );
  fKnobHigh_cut->setCallback ( this );

  fKnobHigh_xover = new ImageKnob ( this,
                                    Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobHigh_xover->setId ( paramHigh_xover );
  fKnobHigh_xover->setAbsolutePos ( 792, 255 );
  fKnobHigh_xover->setRange ( 0.05f, 1.0f );
  fKnobHigh_xover->setDefault ( 0.05f );
  fKnobHigh_xover->setValue ( 0.05f );
  fKnobHigh_xover->setRotationAngle ( 300 );
  fKnobHigh_xover->setCallback ( this );

  fKnobHigh_mult = new ImageKnob ( this,
                                   Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobHigh_mult->setId ( paramHigh_mult );
  fKnobHigh_mult->setAbsolutePos ( 792, 255 );
  fKnobHigh_mult->setRange ( 0.05f, 1.0f );
  fKnobHigh_mult->setDefault ( 0.05f );
  fKnobHigh_mult->setValue ( 0.05f );
  fKnobHigh_mult->setRotationAngle ( 300 );
  fKnobHigh_mult->setCallback ( this );

  // sliders


  fSliderDry_level = new ImageSlider ( this,
                                       Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGR ) );
  fSliderDry_level->setId ( paramDry_level );
  fSliderDry_level->setStartPos ( 0.0f, 0.0f );
  fSliderDry_level->setEndPos ( 0.0f, 100.0f );
  fSliderDry_level->setRange ( 0.0f, 1.0f );
  fSliderDry_level->setValue ( 0.0f );
  fSliderDry_level->setCallback ( this );

  fSliderEarly_level = new ImageSlider ( this,
                                         Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGR ) );
  fSliderEarly_level->setId ( paramEarly_level );
  fSliderEarly_level->setStartPos ( 0.0f, 0.0f );
  fSliderEarly_level->setEndPos ( 0.0f, 100.0f );
  fSliderEarly_level->setRange ( 0.0f, 1.0f );
  fSliderEarly_level->setValue ( 0.0f );
  fSliderEarly_level->setCallback ( this );

  fSliderLate_level = new ImageSlider ( this,
                                        Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGR ) );
  fSliderLate_level->setId ( paramLate_level );
  fSliderLate_level->setStartPos ( 0.0f, 0.0f );
  fSliderLate_level->setEndPos ( 0.0f, 100.0f );
  fSliderLate_level->setRange ( 0.0f, 1.0f );
  fSliderLate_level->setValue ( 0.0f );
  fSliderLate_level->setCallback ( this );


}

/**
   A parameter has changed on the plugin side.
   This is called by the host to inform the UI about parameter changes.
 */
void DragonflyReverbUI::parameterChanged ( uint32_t index, float value )
{
  switch ( index )
    {
      // sliders

    case paramDry_level:
      fSliderDry_level->setValue ( value );
      break;

    case paramEarly_level:
      fSliderEarly_level->setValue ( value );
      break;

    case paramLate_level:
      fSliderLate_level->setValue ( value );
      break;

      // knobs

    case paramSize:
      fKnobSize->setValue ( value );
      break;

    case paramPredelay:
      fKnobPredelay->setValue ( value );
      break;

    case paramDiffuse:
      fKnobDiffuse->setValue ( value );
      break;

    case paramLow_cut:
      fKnobLow_cut->setValue ( value );
      break;

    case paramLow_xover:
      fKnobLow_xover->setValue ( value );
      break;

    case paramLow_mult:
      fKnobLow_mult->setValue ( value );
      break;

    case paramHigh_cut:
      fKnobHigh_cut->setValue ( value );
      break;

    case paramHigh_xover:
      fKnobHigh_xover->setValue ( value );
      break;

    case paramHigh_mult:
      fKnobHigh_mult->setValue ( value );
      break;

    }
}

/* ----------------------------------------------------------------------------------------------------------
 * Widget Callbacks
 *----------------------------------------------------------------------------------------------------------*/



void DragonflyReverbUI::imageKnobDragStarted ( ImageKnob* knob )
{
  editParameter ( knob->getId(), true );
}

void DragonflyReverbUI::imageKnobDragFinished ( ImageKnob* knob )
{
  editParameter ( knob->getId(), false );
}

void DragonflyReverbUI::imageKnobValueChanged ( ImageKnob* knob, float value )
{
  int KnobID = knob->getId();

  setParameterValue ( KnobID,value );
}

void  DragonflyReverbUI::imageSliderDragStarted ( ImageSlider* slider )
{
  editParameter ( slider->getId(), true );
}

void  DragonflyReverbUI::imageSliderDragFinished ( ImageSlider* slider )
{
  editParameter ( slider->getId(), false );
}

void  DragonflyReverbUI::imageSliderValueChanged ( ImageSlider* slider, float value )
{
  int SliderID = slider->getId();
  setParameterValue(SliderID,value);
  }
  
void DragonflyReverbUI::programLoaded ( uint32_t index)
{
  // do stuff
}


void DragonflyReverbUI::onDisplay()
{
  fImgBackground.draw();
   // repsonse graph here
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO


