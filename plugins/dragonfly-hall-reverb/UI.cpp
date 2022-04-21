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
#include "DragonflyVersion.h"
#include "DSP.hpp"
#include "UI.hpp"
#include "Artwork.hpp"
#include "DistrhoPluginInfo.h"
#include <array>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>

START_NAMESPACE_DISTRHO

namespace Art = Artwork;
using DGL::Color;

static const int knobx[]  = {185, 260, 605, 680, 755, 830};
static const int knoby[]  = {15,  130, 245};

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : DragonflyReverbAbstractUI ( Art::backgroundWidth, Art::backgroundHeight, PARAMS, Art::knobData, Art::knobWidth, Art::knobHeight, Art::questionData, Art::questionWidth, Art::questionHeight),
    imgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, kImageFormatBGRA ),
    imgTabOff ( Art::tab_offData, Art::tab_offWidth, Art::tab_offHeight, kImageFormatBGRA ),
    imgTabOn ( Art::tab_onData, Art::tab_onWidth,Art::tab_onHeight, kImageFormatBGRA )
{
  knobSize       = createLabelledKnob(&params[paramSize],       "%3.0f m",  knobx[0], knoby[1]);
  knobWidth      = createLabelledKnob(&params[paramWidth],      "%3.0f%%",  knobx[1], knoby[1]);
  knobPredelay   = createLabelledKnob(&params[paramPredelay],   "%2.0f ms", knobx[0], knoby[2]);
  knobDecay      = createLabelledKnob(&params[paramDecay],      "%2.1f s",  knobx[1], knoby[2]);

  knobDiffuse    = createLabelledKnob(&params[paramDiffuse],    "%2.0f%%",  knobx[2], knoby[0]);
  knobModulation = createLabelledKnob(&params[paramModulation], "%2.0f%%",  knobx[3], knoby[0]);
  knobSpin       = createLabelledKnob(&params[paramSpin],       "%2.2f Hz", knobx[4], knoby[0]);
  knobWander     = createLabelledKnob(&params[paramWander],     "%2.1f ms", knobx[5], knoby[0]);


  knobHighCut    = createLabelledKnob(&params[paramHighCut],    "%5.0f Hz", knobx[3], knoby[1]);
  knobHighXover  = createLabelledKnob(&params[paramHighXover],  "%5.0f Hz", knobx[4], knoby[1]);
  knobHighMult   = createLabelledKnob(&params[paramHighMult],   "%2.1f X",  knobx[5], knoby[1]);

  knobLowCut     = createLabelledKnob(&params[paramLowCut],     "%4.0f Hz", knobx[3], knoby[2]);
  knobLowXover   = createLabelledKnob(&params[paramLowXover],   "%4.0f Hz", knobx[4], knoby[2]);
  knobLowMult    = createLabelledKnob(&params[paramLowMult],    "%2.1f X",  knobx[5], knoby[2]);

  sliderDryLevel = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA ) );
  sliderDryLevel->setId ( paramDry );
  sliderDryLevel->setStartPos ( 17, 157 );
  sliderDryLevel->setEndPos ( 17, 317 );
  sliderDryLevel->setRange ( 0.0f, 100.0f );
  sliderDryLevel->setInverted ( true );
  sliderDryLevel->setCallback ( this );

  sliderEarlyLevel = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA ) );
  sliderEarlyLevel->setId ( paramEarly );
  sliderEarlyLevel->setStartPos ( 57, 157 );
  sliderEarlyLevel->setEndPos ( 57, 317 );
  sliderEarlyLevel->setRange ( 0.0f, 100.0f );
  sliderEarlyLevel->setInverted ( true );
  sliderEarlyLevel->setCallback ( this );

  sliderEarlySend = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA ) );
  sliderEarlySend->setId ( paramEarlySend );
  sliderEarlySend->setStartPos ( 97, 157 );
  sliderEarlySend->setEndPos ( 97, 317 );
  sliderEarlySend->setRange ( 0.0f, 100.0f );
  sliderEarlySend->setInverted ( true );
  sliderEarlySend->setCallback ( this );

  sliderLateLevel = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA ) );
  sliderLateLevel->setId ( paramLate );
  sliderLateLevel->setStartPos ( 137, 157 );
  sliderLateLevel->setEndPos ( 137, 317 );
  sliderLateLevel->setRange ( 0.0f, 100.0f );
  sliderLateLevel->setInverted ( true );
  sliderLateLevel->setCallback ( this );

  rectSliders[0].setPos  ( 17,  157 );
  rectSliders[0].setSize ( 26,  160 );
  rectSliders[1].setPos  ( 57,  157 );
  rectSliders[1].setSize ( 26,  160 );
  rectSliders[2].setPos  ( 97,  157 );
  rectSliders[2].setSize ( 26,  160 );
  rectSliders[3].setPos  ( 137, 157 );
  rectSliders[3].setSize ( 26,  160 );

  rectDisplay.setPos  ( 355, 126 );
  rectDisplay.setSize ( 305, 207 );

  bankSelection = new Selection(this, this, 100, &nanoText, NUM_BANKS);
  bankSelection->setAbsolutePos(320, 5);
  bankSelection->setSelectedImage(&imgTabOn);
  bankSelection->setUnselectedImage(&imgTabOff);
  bankSelection->setTextAlign(NanoVG::ALIGN_RIGHT);
  for ( int i = 0; i < NUM_BANKS; ++i) { bankSelection->setOptionName(i, banks[i].name); }

  presetSelection = new Selection(this, this, 165, &nanoText, PRESETS_PER_BANK);
  presetSelection->setAbsolutePos(420, 5);

  updateBank(DEFAULT_BANK);
  for (int b = 0; b < NUM_BANKS; b++)
  {
    currentProgram[b] = DEFAULT_PRESET;
  }

  aboutButton->setAbsolutePos ( 635, 130 );

  AbstractDSP *dsp = new DragonflyReverbDSP(SPECTROGRAM_SAMPLE_RATE);
  spectrogram = new Spectrogram(this, &nanoText, &rectDisplay, dsp);
  spectrogram->setAbsolutePos (355, 126);
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
    case paramDry:            sliderDryLevel->setValue ( value ); break;
    case paramEarly:        sliderEarlyLevel->setValue ( value ); break;
    case paramEarlySend:     sliderEarlySend->setValue ( value ); break;
    case paramLate:          sliderLateLevel->setValue ( value ); break;

    case paramSize:                 knobSize->setValue ( value ); break;
    case paramWidth:               knobWidth->setValue ( value ); break;
    case paramPredelay:         knobPredelay->setValue ( value ); break;
    case paramDiffuse:           knobDiffuse->setValue ( value ); break;
    case paramLowCut:             knobLowCut->setValue ( value ); break;
    case paramLowXover:         knobLowXover->setValue ( value ); break;
    case paramLowMult:           knobLowMult->setValue ( value ); break;
    case paramHighCut:           knobHighCut->setValue ( value ); break;
    case paramHighXover:       knobHighXover->setValue ( value ); break;
    case paramHighMult:         knobHighMult->setValue ( value ); break;
    case paramDecay:               knobDecay->setValue ( value ); break;
    case paramSpin:                 knobSpin->setValue ( value ); break;
    case paramWander:             knobWander->setValue ( value ); break;
    case paramModulation:     knobModulation->setValue ( value ); break;
  }

  if (index != paramDry) {
    spectrogram->setParameterValue(index, value);
  }

}

void DragonflyReverbUI::stateChanged(const char* key, const char* value)
{
  if (std::strcmp(key, "preset") == 0) {
    for (int b = 0; b < NUM_BANKS; b++) {
      for (int p = 0; p < PRESETS_PER_BANK; p++) {
        if (std::strcmp(value, banks[b].presets[p].name) == 0) {
          updateBank(b);
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

void DragonflyReverbUI::selectionClicked(Selection* selection, int selectedOption) {

  if (selection == bankSelection) {
    updateBank(selectedOption);
  }
  else if (selection == presetSelection) {
    currentProgram[currentBank] = selectedOption;
    presetSelection->setSelectedOption(selectedOption);
  }

  setState("preset", banks[currentBank].presets[currentProgram[currentBank]].name);
  updatePresetDefaults();

  const float *preset = banks[currentBank].presets[currentProgram[currentBank]].params;

  knobSize->setValue ( preset[paramSize] );
  knobWidth->setValue ( preset[paramWidth] );
  knobPredelay->setValue ( preset[paramPredelay] );
  knobDecay->setValue ( preset[paramDecay] );
  knobDiffuse->setValue ( preset[paramDiffuse] );
  knobLowCut->setValue ( preset[paramLowCut] );
  knobLowXover->setValue ( preset[paramLowXover] );
  knobLowMult->setValue ( preset[paramLowMult] );
  knobHighCut->setValue ( preset[paramHighCut] );
  knobHighXover->setValue ( preset[paramHighXover] );
  knobHighMult->setValue ( preset[paramHighMult] );
  knobSpin->setValue ( preset[paramSpin] );
  knobWander->setValue ( preset[paramWander] );
  knobModulation->setValue ( preset[paramModulation] );

  for ( uint32_t i = 0; i < paramCount; i++ ) {
    // Don't set sliders
    if (i != paramDry   &&
        i != paramEarly &&
        i != paramEarlySend   &&
        i != paramLate) {
            setParameterValue ( i, preset[i] );
            spectrogram->setParameterValue(i, preset[i]);
    }
  }

  repaint();
}

void DragonflyReverbUI::onDisplay()
{
  const GraphicsContext& context(getGraphicsContext());

  imgBackground.draw(context);

  float r,g,b;
  r = 230.0f / 256;
  g = 230.0f / 256;
  b = 230.0f / 256;

  // print parameters
  nanoText.beginFrame ( this );
  nanoText.fontSize ( 15 );
  nanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  nanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';

  std::snprintf ( strBuf, 32, "%i%%", int ( sliderDryLevel->getValue() ) );
  nanoText.textBox ( 17 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( sliderEarlyLevel->getValue() ) );
  nanoText.textBox ( 57 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( sliderEarlySend->getValue() ) );
  nanoText.textBox ( 97 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( sliderLateLevel->getValue() ) );
  nanoText.textBox (137 - 2, 330, 35.0f, strBuf, nullptr );

  // print labels;
  nanoText.fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  nanoText.fontSize ( 14 );
  nanoText.textBox (  10, 130, 40, "Dry\nLevel",   nullptr );
  nanoText.textBox (  50, 130, 40, "Early\nLevel", nullptr );
  nanoText.textBox (  90, 130, 40, "Early\nSend",  nullptr );
  nanoText.textBox ( 130, 130, 40, "Late\nLevel",  nullptr );

  nanoText.endFrame();

  //draw faders
  r = 82.0f/255.f;
  g = 148.0f/255.f;
  b = 88.0f/255.f;
  glColor4f ( r, g, b, 1.0f );
  uint dry = ( float ( sliderDryLevel->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint early = ( float ( sliderEarlyLevel->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint early_send = ( float ( sliderEarlySend->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint late = ( float ( sliderLateLevel->getValue() ) / 100.0 ) * 160.0 + 1.0f;

  rectSliders[0].setHeight ( dry );
  rectSliders[0].setY ( 118 + 200 - dry );

  rectSliders[1].setHeight ( early );
  rectSliders[1].setY ( 118 + 200 - early );

  rectSliders[2].setHeight ( early_send );
  rectSliders[2].setY ( 118 + 200 - early_send );

  rectSliders[3].setHeight ( late );
  rectSliders[3].setY ( 118 + 200 - late );

  if ( dry > 1 )
    rectSliders[0].draw(context);
  if ( early > 1 )
    rectSliders[1].draw(context);
  if ( early_send > 1 )
    rectSliders[2].draw(context);
  if ( late > 1 )
    rectSliders[3].draw(context);

  glColor4f ( 1.0f,1.0f,1.0f,1.0f );

  if (displayAbout) {
    spectrogram->hide();
    nanoText.beginFrame ( this );
    nanoText.fontSize ( 15 );
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
      "Dragonfly Hall Reverb is a free audio effect\n"
      "based on Freeverb3 Hibiki.\n\n"
      "Version: %d.%d.%d%s  License: GPL 3+\n\n"
      "• Michael Willis - Plugin Development\n"
      "• Rob van den Berg - Plugin Development\n"
      "• James Peters - Quality Assurance\n"
      "• Chris Share - Technical Writer\n"
      "• Teru Kamogashira - Freeverb3\n"
      "• \"falkTX\" Coelho - Distrho Plugin Framework",
      MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, VERSION_SUFFIX
    );

    nanoText.textBox ( x, y, w, textBuffer, nullptr );
    nanoText.endFrame();
  }
  else
  {
      spectrogram->show();
  }

}

void DragonflyReverbUI::uiIdle() {
  spectrogram->uiIdle();
}

void DragonflyReverbUI::updateBank(int newBank) {
  currentBank = newBank;
  bankSelection->setSelectedOption(newBank);  
  for ( int p = 0; p < NUM_BANKS; ++p) {
    presetSelection->setOptionName(p, banks[currentBank].presets[p].name);
  }
}

void DragonflyReverbUI::updatePresetDefaults() {
  const float *preset = banks[currentBank].presets[currentProgram[currentBank]].params;

  knobSize->setDefault ( preset[paramSize] );
  knobWidth->setDefault ( preset[paramWidth] );
  knobPredelay->setDefault ( preset[paramPredelay] );
  knobDecay->setDefault ( preset[paramDecay] );
  knobDiffuse->setDefault ( preset[paramDiffuse] );
  knobLowCut->setDefault ( preset[paramLowCut] );
  knobLowXover->setDefault ( preset[paramLowXover] );
  knobLowMult->setDefault ( preset[paramLowMult] );
  knobHighCut->setDefault ( preset[paramHighCut] );
  knobHighXover->setDefault ( preset[paramHighXover] );
  knobHighMult->setDefault ( preset[paramHighMult] );
  knobSpin->setDefault ( preset[paramSpin] );
  knobWander->setDefault ( preset[paramWander] );
  knobModulation->setDefault ( preset[paramModulation] );
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
