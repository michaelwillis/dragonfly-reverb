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

#include "DistrhoUI.hpp"
#include "DragonflyReverbUI.hpp"
#include "DragonflyReverbArtwork.hpp"
#include "DistrhoPluginInfo.h"
#include "NotoSans_Regular.ttf.hpp"
#include <array>
#include <vector>
#include <math.h>
#include <string>

#include <iostream>

START_NAMESPACE_DISTRHO

namespace Art = DragonflyReverbArtwork;
using DGL::Color;

static const int knobx[]  = {145, 615, 700, 790};
static const int knoby[]  = {12,  122, 232};

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : UI ( Art::backgroundWidth, Art::backgroundHeight ),
    fImgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGRA ),
    fImgTabOff ( Art::tab_offData, Art::tab_offWidth, Art::tab_offHeight, GL_BGR ),
    fImgTabOn ( Art::tab_onData, Art::tab_onWidth,Art::tab_onHeight, GL_BGR ),
    fImgQuestion ( Art::questionData, Art::questionWidth, Art::questionHeight, GL_BGR )
{
  currentBank = DEFAULT_BANK;

  for (int b = 0; b < NUM_BANKS; b++)
  {
    currentProgram[b] = DEFAULT_PRESET;
  }

  displayAbout = false;

  // text
  fNanoFont  = fNanoText.createFontFromMemory ( "notosans", font_notosans::notosans_ttf, font_notosans::notosans_ttf_size, false );
  fNanoText.fontFaceId ( fNanoFont );

  fKnobSize      = new LabelledKnob (this, this, &fNanoText, paramSize,      "%3.0f m",  knobx[0], knoby[0]);
  fKnobDecay     = new LabelledKnob (this, this, &fNanoText, paramDecay,     "%2.1f s",  knobx[0], knoby[1]);
  fKnobPredelay  = new LabelledKnob (this, this, &fNanoText, paramPredelay,  "%4.0f ms", knobx[0], knoby[2]);

  fKnobDiffuse   = new LabelledKnob (this, this, &fNanoText, paramDiffuse,   "%2.0f %%", knobx[1], knoby[0]);
  fKnobSpin      = new LabelledKnob (this, this, &fNanoText, paramSpin,      "%2.2f Hz", knobx[2], knoby[0]);
  fKnobWander    = new LabelledKnob (this, this, &fNanoText, paramWander,    "%2.1f ms", knobx[3], knoby[0]);

  fKnobHighCut   = new LabelledKnob (this, this, &fNanoText, paramHighCut,   "%5.0f Hz", knobx[1], knoby[1]);
  fKnobHighXover = new LabelledKnob (this, this, &fNanoText, paramHighXover, "%5.0f Hz", knobx[2], knoby[1]);
  fKnobHighMult  = new LabelledKnob (this, this, &fNanoText, paramHighMult,  "%2.1f X",  knobx[3], knoby[1]);

  fKnobLowCut    = new LabelledKnob (this, this, &fNanoText, paramLowCut,    "%4.0f Hz", knobx[1], knoby[2]);
  fKnobLowXover  = new LabelledKnob (this, this, &fNanoText, paramLowXover,  "%4.0f Hz", knobx[2], knoby[2]);
  fKnobLowMult   = new LabelledKnob (this, this, &fNanoText, paramLowMult,   "%2.1f X",  knobx[3], knoby[2]);

  // sliders
  fSliderDry_level = new ImageSlider ( this,
                                       Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderDry_level->setId ( paramDry_level );
  fSliderDry_level->setStartPos ( 17, 102 );
  fSliderDry_level->setEndPos ( 17, 302 );
  fSliderDry_level->setRange ( 0.0f, 100.0f );
  fSliderDry_level->setInverted ( true );
  fSliderDry_level->setCallback ( this );

  fSliderEarly_level = new ImageSlider ( this,
                                         Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderEarly_level->setId ( paramEarly_level );
  fSliderEarly_level->setStartPos ( 57, 102 );
  fSliderEarly_level->setEndPos ( 57, 302 );
  fSliderEarly_level->setRange ( 0.0f, 100.0f );
  fSliderEarly_level->setInverted ( true );
  fSliderEarly_level->setCallback ( this );

  fSliderLate_level = new ImageSlider ( this,
                                        Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderLate_level->setId ( paramLate_level );
  fSliderLate_level->setStartPos ( 97, 102 );
  fSliderLate_level->setEndPos ( 97, 302 );
  fSliderLate_level->setRange ( 0.0f, 100.0f );
  fSliderLate_level->setInverted ( true );
  fSliderLate_level->setCallback ( this );

  rectSliders[0].setPos ( 17,102 );
  rectSliders[0].setSize ( 26,200 );
  rectSliders[1].setPos ( 57,102 );
  rectSliders[1].setSize ( 26,200 );
  rectSliders[2].setPos ( 97,102 );
  rectSliders[2].setSize ( 26,200 );

  rectDisplay.setPos ( 245, 140 );
  rectDisplay.setSize ( 365, 180 );

  for ( int i = 0; i < NUM_BANKS; ++i)
  {
    rectBanks[i].setPos ( 235, 5 + (i * 24) );
    rectBanks[i].setSize ( 95, 24 );
  }

  for ( int i = 0; i < PRESETS_PER_BANK; ++i)
  {
    rectPrograms[i].setPos( 340, 5 + (i * 24) );
    rectPrograms[i].setSize( 150, 21 );
  }

  rectAbout.setPos ( 115, 50 );
  rectAbout.setSize ( 20, 20 );

  spectrogram = new Spectrogram(this, &fNanoText, &rectDisplay);
  spectrogram->setAbsolutePos (245, 140);
}

/**
   A parameter has changed on the plugin side
   This is called by the host to inform the UI about parameter changes.
 */
void DragonflyReverbUI::parameterChanged ( uint32_t index, float value )
{
  std::cout << "DragonflyReverbUI::parameterChanged(\"" << index << "\", \"" << value << "\")\n";
  displayAbout = false;

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

    case paramLowCut:
      fKnobLowCut->setValue ( value );
      break;

    case paramLowXover:
      fKnobLowXover->setValue ( value );
      break;

    case paramLowMult:
      fKnobLowMult->setValue ( value );
      break;

    case paramHighCut:
      fKnobHighCut->setValue ( value );
      break;

    case paramHighXover:
      fKnobHighXover->setValue ( value );
      break;

    case paramHighMult:
      fKnobHighMult->setValue ( value );
      break;

    case paramDecay:
      fKnobDecay->setValue ( value );
      break;

    case paramSpin:
      fKnobSpin->setValue ( value );
      break;

    case paramWander:
      fKnobWander->setValue ( value );
      break;
    }

    spectrogram->setParameterValue(index, value);
}

void DragonflyReverbUI::stateChanged(const char* key, const char* value)
{
  if (std::strcmp(key, "preset") == 0) {
    for (int b = 0; b < NUM_BANKS; b++) {
      for (int p = 0; p < PRESETS_PER_BANK; p++) {
        if (std::strcmp(value, banks[b].presets[p].name) == 0) {
          currentBank = b;
          currentProgram[currentBank] = p;
        }
      }
    }

    updatePresetDefaults();
  }

  repaint();
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
  spectrogram->setParameterValue ( knob->getId(), value );
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
  spectrogram->setParameterValue ( SliderID, value );
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
      bool presetClicked = false;

      for (int row = 0; row < NUM_BANKS; row++)
      {
        if (rectBanks[row].contains ( ev.pos ))
        {
          currentBank = row;
          presetClicked = true;
        }
      }

      for (int row = 0; row < PRESETS_PER_BANK; row++)
      {
        if (rectPrograms[row].contains ( ev.pos ))
        {
          currentProgram[currentBank] = row;
          presetClicked = true;
        }
      }

      if (presetClicked)
      {
        setState("preset", banks[currentBank].presets[currentProgram[currentBank]].name);
        updatePresetDefaults();

        const float *preset = banks[currentBank].presets[currentProgram[currentBank]].params;

        fSliderDry_level->setValue ( preset[paramDry_level] );
        fSliderEarly_level->setValue ( preset[paramEarly_level] );
        fSliderLate_level->setValue ( preset[paramLate_level] );
        fKnobSize->setValue ( preset[paramSize] );
        fKnobPredelay->setValue ( preset[paramPredelay] );
        fKnobDiffuse->setValue ( preset[paramDiffuse] );
        fKnobLowCut->setValue ( preset[paramLowCut] );
        fKnobLowXover->setValue ( preset[paramLowXover] );
        fKnobLowMult->setValue ( preset[paramLowMult] );
        fKnobHighCut->setValue ( preset[paramHighCut] );
        fKnobHighXover->setValue ( preset[paramHighXover] );
        fKnobHighMult->setValue ( preset[paramHighMult] );
        fKnobDecay->setValue ( preset[paramDecay] );
        fKnobSpin->setValue ( preset[paramSpin] );
        fKnobWander->setValue ( preset[paramWander] );
        for ( uint32_t i = 0; i < paramCount; i++ ) {
          setParameterValue ( i, preset[i] );
          spectrogram->setParameterValue(i, preset[i]);
        }

        repaint();
        return true;
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
  fNanoText.beginFrame ( this );
  fNanoText.fontSize ( 16 );
  fNanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  fNanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';

  std::snprintf ( strBuf, 32, "%i %%", int ( fSliderDry_level->getValue() ) );
  fNanoText.textBox ( 17 - 2  , 314 , 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i %%", int ( fSliderEarly_level->getValue() ) );
  fNanoText.textBox ( 57 - 2, 314, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i %%", int ( fSliderLate_level->getValue() ) );
  fNanoText.textBox ( 97 - 2, 314 , 35.0f, strBuf, nullptr );

  // print labels;
  fNanoText.fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  fNanoText.fontSize ( 18 );
  fNanoText.textBox ( 17 - 2, 90, 30, "Dry",   nullptr );
  fNanoText.textBox ( 57 - 2, 90, 30, "Early", nullptr );
  fNanoText.textBox ( 97 - 2, 90, 30, "Late",  nullptr );

  fNanoText.endFrame();

  //draw faders
  r = 82.0f/255.f;
  g = 148.0f/255.f;
  b = 88.0f/255.f;
  glColor4f ( r, g, b, 1.0f );
  uint dry = ( float ( fSliderDry_level->getValue() ) / 100.0 ) * 200.0 + 1.0f;
  uint early = ( float ( fSliderEarly_level->getValue() ) / 100.0 ) * 200.0 + 1.0f;
  uint late = ( float ( fSliderLate_level->getValue() ) / 100.0 ) * 200.0 + 1.0f;

  rectSliders[0].setHeight ( dry );
  rectSliders[0].setY ( 103 + 200 - dry );

  rectSliders[1].setHeight ( early );
  rectSliders[1].setY ( 103 + 200 - early );

  rectSliders[2].setHeight ( late );
  rectSliders[2].setY ( 103 + 200 - late );

  if ( dry > 1 )
    rectSliders[0].draw();
  if ( early > 1 )
    rectSliders[1].draw();
  if ( late > 1 )
    rectSliders[2].draw();

  glColor4f ( 1.0f,1.0f,1.0f,1.0f );
  fImgQuestion.drawAt ( rectAbout.getX(), rectAbout.getY() );

  fNanoText.beginFrame ( this );
  fNanoText.fontSize ( 18 );
  fNanoText.textAlign ( NanoVG::ALIGN_RIGHT | NanoVG::ALIGN_TOP );

  Color bright = Color ( 0.90f, 0.95f, 1.00f );
  Color dim    = Color ( 0.65f, 0.65f, 0.65f );

  for (int row = 0; row < NUM_BANKS; row ++)
  {
    DGL::Rectangle<int> bank = rectBanks[row];
    if (currentBank == row) {
      fImgTabOn.drawAt ( bank.getX(), bank.getY() );
      fNanoText.fillColor ( bright );
    } else {
      fImgTabOff.drawAt ( bank.getX(), bank.getY() );
      fNanoText.fillColor ( dim );
    }

    fNanoText.textBox ( bank.getX() - 3, bank.getY() + 2, bank.getWidth(), banks[row].name, nullptr );
  }

  fNanoText.textAlign ( NanoVG::ALIGN_LEFT | NanoVG::ALIGN_TOP );

  for (int row = 0; row < PRESETS_PER_BANK; row ++)
  {
    DGL::Rectangle<int> program = rectPrograms[row];
    fNanoText.fillColor( row == currentProgram[currentBank] ? bright : dim );
    fNanoText.textBox ( program.getX() + 3, program.getY() + 2, program.getWidth(), banks[currentBank].presets[row].name, nullptr );
  }

  fNanoText.endFrame();

  if (displayAbout) {
    spectrogram->hide();
    fNanoText.beginFrame ( this );
    fNanoText.fontSize ( 18 );
    fNanoText.textAlign ( NanoVG::ALIGN_LEFT|NanoVG::ALIGN_TOP );

    r = 230.0f / 256;
    g = 230.0f / 256;
    b = 230.0f / 256;
    fNanoText.fillColor ( Color ( r, g, b ) );

    int x = rectDisplay.getX() + 10;
    int y = rectDisplay.getY() + 5;
    int w = rectDisplay.getWidth() - 20;

    char textBuffer[256];

    std::snprintf(textBuffer, 256,
      "Dragonfly is a free hall-style reverb plugin\n\n"
      "Developed by Michael Willis and Rob van den Berg\n\n"
      "Acknowledgments:\n"
      "Teru Kamogashira - Freeverb3\n"
      "Filipe \"falkTX\" Coelho - Distrho Plugin Framework\n\n"
      "Version: %d.%d.%d  License: GPL 3+",
      MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION
    );

    fNanoText.textBox ( x, y, w, textBuffer, nullptr );
    fNanoText.endFrame();
  }
  else
  {
      spectrogram->show();
  }

}

void DragonflyReverbUI::uiIdle() {
  spectrogram->uiIdle();
}

void DragonflyReverbUI::updatePresetDefaults() {
  const float *preset = banks[currentBank].presets[currentProgram[currentBank]].params;

  fKnobSize->setDefault ( preset[paramSize] );
  fKnobPredelay->setDefault ( preset[paramPredelay] );
  fKnobDiffuse->setDefault ( preset[paramDiffuse] );
  fKnobLowCut->setDefault ( preset[paramLowCut] );
  fKnobLowXover->setDefault ( preset[paramLowXover] );
  fKnobLowMult->setDefault ( preset[paramLowMult] );
  fKnobHighCut->setDefault ( preset[paramHighCut] );
  fKnobHighXover->setDefault ( preset[paramHighXover] );
  fKnobHighMult->setDefault ( preset[paramHighMult] );
  fKnobDecay->setDefault ( preset[paramDecay] );
  fKnobSpin->setDefault ( preset[paramSpin] );
  fKnobWander->setDefault ( preset[paramWander] );
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
