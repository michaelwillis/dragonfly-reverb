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
#include <array>

START_NAMESPACE_DISTRHO

namespace Art = DragonflyReverbArtwork;
using DGL::Color;

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : UI ( Art::backgroundWidth, Art::backgroundHeight ),
    fImgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGRA )
{
  // text
  fNanoText.loadSharedResources();
  fNanoFont  = fNanoText.createFontFromFile ( "lcd_solid","/home/rob/git/dragonfly-reverb/plugins/dragonfly-reverb/Artwork/LCD_Solid.ttf" );

  // knobs
  std::array <int, 3> knobx {77,174, 270};
  std::array <int, 3> knoby {34, 135, 233 };

  fKnobSize = new ImageKnob ( this,
                              Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobSize->setId ( paramSize );
  fKnobSize->setAbsolutePos ( knobx[1], knoby[0] );
  fKnobSize->setRange ( 8.0f, 100.0f );
  fKnobSize->setDefault ( 40.0f );
  fKnobSize->setValue ( 40.0f );
  fKnobSize->setRotationAngle ( 300 );
  fKnobSize->setCallback ( this );

  //
  fKnobPredelay = new ImageKnob ( this,
                                  Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobPredelay->setId ( paramPredelay );
  fKnobPredelay->setAbsolutePos ( knobx[0], knoby[0] );
  fKnobPredelay->setRange ( 0.0f, 100.0f );
  fKnobPredelay->setDefault ( 14.0f );
  fKnobPredelay->setValue ( 14.0f );
  fKnobPredelay->setRotationAngle ( 300 );
  fKnobPredelay->setCallback ( this );

  fKnobDiffuse = new ImageKnob ( this,
                                 Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobDiffuse->setId ( paramDiffuse );
  fKnobDiffuse->setAbsolutePos ( knobx[2], knoby[0] );
  fKnobDiffuse->setRange ( 0.00f, 100.0f );
  fKnobDiffuse->setDefault ( 80.0f );
  fKnobDiffuse->setValue ( 80.0f );
  fKnobDiffuse->setRotationAngle ( 300 );
  fKnobDiffuse->setCallback ( this );

  fKnobLow_cut = new ImageKnob ( this,
                                 Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobLow_cut->setId ( paramLow_cut );
  fKnobLow_cut->setAbsolutePos ( knobx[0], knoby[2] );
  fKnobLow_cut->setRange ( 0.0f, 100.0f );
  fKnobLow_cut->setDefault ( 4.0f );
  fKnobLow_cut->setValue ( 4.0f );
  fKnobLow_cut->setRotationAngle ( 300 );
  fKnobLow_cut->setCallback ( this );

  fKnobLow_xover = new ImageKnob ( this,
                                   Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobLow_xover->setId ( paramLow_xover );
  fKnobLow_xover->setAbsolutePos ( knobx[1], knoby[2] );
  fKnobLow_xover->setRange ( 100.0f, 1000.0f );
  fKnobLow_xover->setDefault ( 600.0f );
  fKnobLow_xover->setValue ( 600.0f );
  fKnobLow_xover->setRotationAngle ( 300 );
  fKnobLow_xover->setCallback ( this );

  fKnobLow_mult = new ImageKnob ( this,
                                  Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobLow_mult->setId ( paramLow_mult );
  fKnobLow_mult->setAbsolutePos ( knobx[2], knoby[2] );
  fKnobLow_mult->setRange ( 0.1f, 4.0f );
  fKnobLow_mult->setDefault ( 1.5f );
  fKnobLow_mult->setValue ( 1.5f );
  fKnobLow_mult->setRotationAngle ( 300 );
  fKnobLow_mult->setCallback ( this );

  fKnobHigh_cut = new ImageKnob ( this,
                                  Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobHigh_cut->setId ( paramHigh_cut );
  fKnobHigh_cut->setAbsolutePos ( knobx[0], knoby[1] );
  fKnobHigh_cut->setRange ( 2000.0f, 20000.0f );
  fKnobHigh_cut->setDefault ( 7500.0f );
  fKnobHigh_cut->setValue ( 7500.0f );
  fKnobHigh_cut->setRotationAngle ( 300 );
  fKnobHigh_cut->setCallback ( this );

  fKnobHigh_xover = new ImageKnob ( this,
                                    Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobHigh_xover->setId ( paramHigh_xover );
  fKnobHigh_xover->setAbsolutePos ( knobx[1], knoby[1] );
  fKnobHigh_xover->setRange ( 1000.0f, 20000.0f );
  fKnobHigh_xover->setDefault ( 4500.0f );
  fKnobHigh_xover->setValue ( 4500.0f );
  fKnobHigh_xover->setRotationAngle ( 300 );
  fKnobHigh_xover->setCallback ( this );

  fKnobHigh_mult = new ImageKnob ( this,
                                   Image ( Art::knobData, Art::knobWidth, Art::knobHeight, GL_BGRA ) );
  fKnobHigh_mult->setId ( paramHigh_mult );
  fKnobHigh_mult->setAbsolutePos ( knobx[2], knoby[1] );
  fKnobHigh_mult->setRange ( 0.01f, 2.0f );
  fKnobHigh_mult->setDefault ( 0.4f );
  fKnobHigh_mult->setValue ( 0.4f );
  fKnobHigh_mult->setRotationAngle ( 300 );
  fKnobHigh_mult->setCallback ( this );

  // sliders


  fSliderDry_level = new ImageSlider ( this,
                                       Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderDry_level->setId ( paramDry_level );
  fSliderDry_level->setStartPos ( 781, 31 );
  fSliderDry_level->setEndPos ( 781, 238 );
  fSliderDry_level->setRange ( 0.0f, 100.0f );
  fSliderDry_level->setValue ( 50.0f );
  fSliderDry_level->setInverted ( true );
  fSliderDry_level->setCallback ( this );

  fSliderEarly_level = new ImageSlider ( this,
                                         Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderEarly_level->setId ( paramEarly_level );
  fSliderEarly_level->setStartPos ( 849, 31 );
  fSliderEarly_level->setEndPos ( 849, 238 );
  fSliderEarly_level->setRange ( 0.0f, 100.0f );
  fSliderEarly_level->setValue ( 50.0f );
  fSliderEarly_level->setInverted ( true );
  fSliderEarly_level->setCallback ( this );

  fSliderLate_level = new ImageSlider ( this,
                                        Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderLate_level->setId ( paramLate_level );
  fSliderLate_level->setStartPos ( 918, 31 );
  fSliderLate_level->setEndPos ( 918, 238 );
  fSliderLate_level->setRange ( 0.0f, 100.0f );
  fSliderLate_level->setValue ( 50.0f );
  fSliderLate_level->setInverted ( true );
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
  setParameterValue ( SliderID,value );
}

void DragonflyReverbUI::programLoaded ( uint32_t index )
{
  currentProgram = index;
  const float *preset = presets[index].params;
  fSliderDry_level->setValue ( preset[paramDry_level] );
  fSliderEarly_level->setValue ( preset[paramEarly_level] );
  fSliderLate_level->setValue ( preset[paramLate_level] );
  fKnobSize->setValue ( preset[paramSize] );
  fKnobPredelay->setValue ( preset[paramPredelay] );
  fKnobDiffuse->setValue ( preset[paramDiffuse] );
  fKnobLow_cut->setValue ( preset[paramLow_cut] );
  fKnobLow_xover->setValue ( preset[paramLow_xover] );
  fKnobLow_mult->setValue ( preset[paramLow_mult] );
  fKnobHigh_cut->setValue ( preset[paramHigh_cut] );
  fKnobHigh_xover->setValue ( preset[paramHigh_xover] );
  fKnobHigh_mult->setValue ( preset[paramHigh_mult] );
}


void DragonflyReverbUI::onDisplay()
{
  fImgBackground.draw();
  // repsonse graph here

  // text display
  fNanoText.beginFrame ( this );
  fNanoText.fontFaceId ( fNanoFont );
  fNanoText.fontSize ( 10 );
  fNanoText.textAlign ( NanoVG::ALIGN_LEFT|NanoVG::ALIGN_TOP );
  float r,g,b;
  r =  8.0f / 256;
  g = 19.0f / 256;
  b = 10.0f / 256;
  fNanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';
  
  float xstart = 374.0f, ystart= 195.0f;
  float xoffset = 126.0f, yoffset = 12.0f, extra_offset_x = 8.0f;
  
  // print current program namespace
  std::snprintf (strBuf, 32, "Program : %s", presets[currentProgram].name);
  fNanoText.textBox( xstart, ystart - yoffset, 200.0f, strBuf, nullptr);
  
  // print parameters
  std::snprintf ( strBuf, 32, "Pre-Delay: %4.2f ms", fKnobPredelay->getValue() );
  fNanoText.textBox ( xstart             , ystart , 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Hi Cut   : %i Hz", int ( fKnobHigh_cut->getValue() ) );
  fNanoText.textBox ( xstart             , ystart + yoffset ,130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Low Cut  : %4.2f Hz", fKnobLow_cut->getValue() );
  fNanoText.textBox ( xstart             , ystart + yoffset*2 ,130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Dry      : %i %%", int ( fSliderDry_level->getValue() ) );
  fNanoText.textBox ( xstart             , ystart + yoffset*3, 130.0f, strBuf, nullptr );

  std::snprintf ( strBuf, 32, "Size     : %4.2f m", fKnobSize->getValue() );
  fNanoText.textBox ( xstart + xoffset   , ystart , 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Hi Cross : %i Hz", int ( fKnobHigh_xover->getValue() ) );
  fNanoText.textBox ( xstart + xoffset   , ystart + yoffset, 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Low Cross: %4.2f Hz", fKnobLow_xover->getValue() );
  fNanoText.textBox ( xstart + xoffset   , ystart + yoffset*2, 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Early    : %i %%", int ( fSliderEarly_level->getValue() ) );
  fNanoText.textBox ( xstart + xoffset*1 , ystart + yoffset*3, 130.0f, strBuf, nullptr );

  std::snprintf ( strBuf, 32, "Diffuse : %i %%", int ( fKnobDiffuse->getValue() ) );
  fNanoText.textBox ( xstart + extra_offset_x + xoffset*2 , ystart , 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Hi Mult : %4.2f X", fKnobHigh_mult->getValue() );
  fNanoText.textBox ( xstart + extra_offset_x + xoffset*2 , ystart + yoffset, 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Low Mult: %4.2f X", fKnobLow_mult->getValue() );
  fNanoText.textBox ( xstart + extra_offset_x + xoffset*2 , ystart + yoffset*2, 130.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "Late    : %i %%", int ( fSliderLate_level->getValue() ) );
  fNanoText.textBox ( xstart + extra_offset_x + xoffset*2 , ystart + yoffset*3, 130.0f, strBuf, nullptr );
  fNanoText.endFrame();

}



/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO


