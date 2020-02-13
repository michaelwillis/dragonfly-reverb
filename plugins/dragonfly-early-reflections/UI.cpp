/*
 * Dragonfly Reverb, copyright (c) 2019 Michael Willis, Rob van den Berg
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

#include "DistrhoUI.hpp"
#include "DragonflyVersion.h"
#include "DSP.hpp"
#include "UI.hpp"
#include "Artwork.hpp"
#include "DistrhoPluginInfo.h"
#include "NotoSans_Regular.ttf.hpp"
#include <array>
#include <vector>
#include <math.h>
#include <string>


START_NAMESPACE_DISTRHO

namespace Art = Artwork;
using DGL::Color;

static const int knobx[]  = {285, 360};
static const int knoby[]  = {130, 245};

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : DragonflyReverbAbstractUI ( Art::backgroundWidth, Art::backgroundHeight, PARAMS, Art::knobData, Art::knobWidth, Art::knobHeight),
    fImgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGRA ),
    fImgTabOff ( Art::tab_offData, Art::tab_offWidth, Art::tab_offHeight, GL_BGR ),
    fImgTabOn ( Art::tab_onData, Art::tab_onWidth,Art::tab_onHeight, GL_BGR ),
    fImgQuestion ( Art::questionData, Art::questionWidth, Art::questionHeight, GL_BGRA )
{
  currentProgram = DEFAULT_PROGRAM;

  displayAbout = false;

  knobSize        = createLabelledKnob(&params[paramSize],       "%2.0f m", knobx[0], knoby[0]);
  knobWidth       = createLabelledKnob(&params[paramWidth],      "%3.0f%%", knobx[1], knoby[0]);
  knobLowCut      = createLabelledKnob(&params[paramLowCut],    "%4.0f Hz", knobx[0], knoby[1]);
  knobHighCut     = createLabelledKnob(&params[paramHighCut],   "%5.0f Hz", knobx[1], knoby[1]);

  sliderDry = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  sliderDry->setId ( paramDry );
  sliderDry->setStartPos ( 17, 157 );
  sliderDry->setEndPos ( 17, 317 );
  sliderDry->setRange ( 0.0f, 100.0f );
  sliderDry->setInverted ( true );
  sliderDry->setCallback ( this );

  sliderWet = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  sliderWet->setId ( paramWet );
  sliderWet->setStartPos ( 57, 157 );
  sliderWet->setEndPos ( 57, 317 );
  sliderWet->setRange ( 0.0f, 100.0f );
  sliderWet->setInverted ( true );
  sliderWet->setCallback ( this );

  rectSliders[0].setPos  ( 17,  157 );
  rectSliders[0].setSize ( 26,  160 );
  rectSliders[1].setPos  ( 57,  157 );
  rectSliders[1].setSize ( 26,  160 );

  rectDisplay.setPos  ( 110, 126 );
  rectDisplay.setSize ( 305, 207 );

  for ( int i = 0; i < PROGRAM_COUNT; ++i)
  {
    rectPrograms[i].setPos( 120, 160 + (i * 21) );
    rectPrograms[i].setSize( 125, 21 );
  }

  rectAbout.setPos  ( 240, 130 );
  rectAbout.setSize ( 20,  20  );
}

/**
   A parameter has changed on the plugin side
   This is called by the host to inform the UI about parameter changes.
 */
void DragonflyReverbUI::parameterChanged ( uint32_t index, float value )
{
  displayAbout = false;

  switch ( index )
  {
    case paramDry:            sliderDry->setValue ( value ); break;
    case paramWet:            sliderWet->setValue ( value ); break;
    case paramSize:            knobSize->setValue ( value ); break;
    case paramWidth:          knobWidth->setValue ( value ); break;
    case paramLowCut:        knobLowCut->setValue ( value ); break;
    case paramHighCut:      knobHighCut->setValue ( value ); break;
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
  setParameterValue ( knob->getId(),value );
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

bool DragonflyReverbUI::onMouse ( const MouseEvent& ev )
{
  if ( ev.button != 1 )
    return false;
  if ( ev.press )
  {
    if ( displayAbout )
    {
      displayAbout = false;
      repaint();
      return false;
    }
    else
    {
      for (int row = 0; row < PROGRAM_COUNT; row++)
      {
        if (rectPrograms[row].contains ( ev.pos ))
        {
	  setParameterValue ( paramProgram, row );
          currentProgram = row;
	  repaint();
        }
      }

      if ( rectAbout.contains ( ev.pos ) )
      {
        displayAbout = true;
        repaint();
        return true;
      }
    }
  }
  return false;
}

void DragonflyReverbUI::onDisplay()
{
  fImgBackground.draw();

  float r,g,b;
  r = 230.0f / 256;
  g = 230.0f / 256;
  b = 230.0f / 256;

  // print parameters
  nanoText.beginFrame ( this );
  nanoText.fontSize ( 16 );
  nanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  nanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';

  std::snprintf ( strBuf, 32, "%i%%", int ( sliderDry->getValue() ) );
  nanoText.textBox ( 17 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( sliderWet->getValue() ) );
  nanoText.textBox ( 57 - 2, 330, 35.0f, strBuf, nullptr );

  // print labels;
  nanoText.fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  nanoText.fontSize ( 16 );
  nanoText.textBox (  10, 130, 40, "Dry\nLevel",   nullptr );
  nanoText.textBox (  50, 130, 40, "Wet\nLevel",   nullptr );
  nanoText.endFrame();

  //draw faders
  r = 178.0f/255.f;
  g = 148.0f/255.f;
  b =  59.0f/255.f;
  glColor4f ( r, g, b, 1.0f );
  uint dry = ( float ( sliderDry->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint wet = ( float ( sliderWet->getValue() ) / 100.0 ) * 160.0 + 1.0f;

  rectSliders[0].setHeight ( dry );
  rectSliders[0].setY ( 118 + 200 - dry );

  rectSliders[1].setHeight ( wet );
  rectSliders[1].setY ( 118 + 200 - wet );

  if ( dry > 1 )
    rectSliders[0].draw();
  if ( wet > 1 )
    rectSliders[1].draw();

  glColor4f ( 1.0f,1.0f,1.0f,1.0f );

  nanoText.beginFrame ( this );
  nanoText.fontSize ( 18 );
  nanoText.textAlign ( NanoVG::ALIGN_RIGHT | NanoVG::ALIGN_TOP );

  Color bright = Color ( 0.90f, 0.95f, 1.00f );
  Color dim    = Color ( 0.65f, 0.65f, 0.65f );

  if (displayAbout) {
    nanoText.beginFrame ( this );
    nanoText.fontSize ( 18 );
    nanoText.textAlign ( NanoVG::ALIGN_LEFT|NanoVG::ALIGN_TOP );

    r = 230.0f / 256;
    g = 230.0f / 256;
    b = 230.0f / 256;
    nanoText.fillColor ( Color ( r, g, b ) );

    int x = rectDisplay.getX() + 5;
    int y = rectDisplay.getY() + 5;
    int w = rectDisplay.getWidth() - 10;

    char textBuffer[400];

    std::snprintf(textBuffer, 400,
      "This is a free audio effect\n"
      "based on Freeverb3.\n\n"
      "Version: %d.%d.%d%s\nLicense: GPL 3+\n\n"
      "• Michael Willis - Dev\n"
      "• James Peters - QA\n"
      "• Teru Kamogashira - FV3\n"
      "• \"falkTX\" Coelho - DPF",
      MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, VERSION_SUFFIX
    );

    nanoText.textBox ( x, y, w, textBuffer, nullptr );
    nanoText.endFrame();
  }
  else
  {
      glColor4f ( 1.0f,1.0f,1.0f,1.0f );
      fImgQuestion.drawAt ( rectAbout.getX(), rectAbout.getY() );

      nanoText.textAlign ( NanoVG::ALIGN_LEFT | NanoVG::ALIGN_TOP );
      nanoText.fillColor(bright);
      nanoText.textBox ( 123, 130, 200, "Reflection Type", nullptr );
  
      for (int row = 0; row < PROGRAM_COUNT; row ++) {
	DGL::Rectangle<int> rect = rectPrograms[row];
	nanoText.fillColor( row == currentProgram ? bright : dim );
	nanoText.textBox ( rect.getX() + 3, rect.getY() + 2, rect.getWidth(), programs[row].name, nullptr );
      }
      
      nanoText.endFrame();
  }

}

void DragonflyReverbUI::uiIdle() { }

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
