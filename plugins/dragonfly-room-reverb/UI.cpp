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

namespace Art = DragonflyReverbArtwork;
using DGL::Color;

static const int knobx[]  = {185, 260, 680, 755, 830};
static const int knoby[]  = {15,  130, 245};

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : DragonflyReverbAbstractUI ( Art::backgroundWidth, Art::backgroundHeight, PARAMS, Art::knobData, Art::knobWidth, Art::knobHeight),
    fImgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGRA ),
    fImgTabOff ( Art::tab_offData, Art::tab_offWidth, Art::tab_offHeight, GL_BGR ),
    fImgTabOn ( Art::tab_onData, Art::tab_onWidth,Art::tab_onHeight, GL_BGR ),
    fImgQuestion ( Art::questionData, Art::questionWidth, Art::questionHeight, GL_BGRA )
{
  currentBank = DEFAULT_BANK;
  for (int b = 0; b < NUM_BANKS; b++)
  {
    currentPreset[b] = DEFAULT_PRESET;
  }

  displayAbout = false;

  fKnobSize        = createLabelledKnob(&params[paramSize],        "%2.0f m",  knobx[0], knoby[1]);
  fKnobWidth       = createLabelledKnob(&params[paramWidth],       "%3.0f%%",  knobx[1], knoby[1]);
  fKnobPredelay    = createLabelledKnob(&params[paramPredelay],    "%2.0f ms", knobx[0], knoby[2]);
  fKnobDecay       = createLabelledKnob(&params[paramDecay],       "%2.1f s",  knobx[1], knoby[2]);

  fKnobDiffuse     = createLabelledKnob(&params[paramDiffuse],     "%2.0f%%",  knobx[2], knoby[0]);
  fKnobSpin        = createLabelledKnob(&params[paramSpin],        "%2.2f Hz", knobx[3], knoby[0]);
  fKnobWander      = createLabelledKnob(&params[paramWander],      "%2.0f%%",  knobx[4], knoby[0]);

  fKnobInHighCut   = createLabelledKnob(&params[paramInHighCut],   "%5.0f Hz", knobx[2], knoby[1]);
  fKnobEarlyDamp   = createLabelledKnob(&params[paramEarlyDamp],   "%5.0f Hz", knobx[3], knoby[1]);
  fKnobLateDamp    = createLabelledKnob(&params[paramLateDamp],    "%5.0f Hz", knobx[4], knoby[1]);

  fKnobInLowCut    = createLabelledKnob(&params[paramInLowCut],    "%4.0f Hz", knobx[2], knoby[2]);
  fKnobBoost       = createLabelledKnob(&params[paramBoost],       "%1.0f%%",  knobx[3], knoby[2]);
  fKnobBoostLPF    = createLabelledKnob(&params[paramBoostLPF],    "%4.0f Hz", knobx[4], knoby[2]);

  fSliderDry_level = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderDry_level->setId ( paramDry );
  fSliderDry_level->setStartPos ( 17, 157 );
  fSliderDry_level->setEndPos ( 17, 317 );
  fSliderDry_level->setRange ( 0.0f, 100.0f );
  fSliderDry_level->setInverted ( true );
  fSliderDry_level->setCallback ( this );

  fSliderEarly_level = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderEarly_level->setId ( paramEarly );
  fSliderEarly_level->setStartPos ( 57, 157 );
  fSliderEarly_level->setEndPos ( 57, 317 );
  fSliderEarly_level->setRange ( 0.0f, 100.0f );
  fSliderEarly_level->setInverted ( true );
  fSliderEarly_level->setCallback ( this );

  fSliderEarlySend = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderEarlySend->setId ( paramEarlySend );
  fSliderEarlySend->setStartPos ( 97, 157 );
  fSliderEarlySend->setEndPos ( 97, 317 );
  fSliderEarlySend->setRange ( 0.0f, 100.0f );
  fSliderEarlySend->setInverted ( true );
  fSliderEarlySend->setCallback ( this );

  fSliderLate_level = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderLate_level->setId ( paramLate );
  fSliderLate_level->setStartPos ( 137, 157 );
  fSliderLate_level->setEndPos ( 137, 317 );
  fSliderLate_level->setRange ( 0.0f, 100.0f );
  fSliderLate_level->setInverted ( true );
  fSliderLate_level->setCallback ( this );

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

  for ( int i = 0; i < NUM_BANKS; ++i)
  {
    rectBanks[i].setPos ( 350, 5 + (i * 21) );
    rectBanks[i].setSize ( 95, 24 );
  }

  for ( int i = 0; i < PRESETS_PER_BANK; ++i)
  {
    rectPresets[i].setPos( 460, 5 + (i * 21) );
    rectPresets[i].setSize( 150, 21 );
  }

  rectAbout.setPos  ( 635, 130 );
  rectAbout.setSize ( 20,  20  );

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
    case paramDry:           fSliderDry_level->setValue ( value ); break;
    case paramEarly:       fSliderEarly_level->setValue ( value ); break;
    case paramEarlySend:     fSliderEarlySend->setValue ( value ); break;
    case paramLate:         fSliderLate_level->setValue ( value ); break;

    case paramSize:                 fKnobSize->setValue ( value ); break;
    case paramWidth:               fKnobWidth->setValue ( value ); break;
    case paramPredelay:         fKnobPredelay->setValue ( value ); break;
    case paramDecay:               fKnobDecay->setValue ( value ); break;

    case paramDiffuse:           fKnobDiffuse->setValue ( value ); break;
    case paramSpin:                 fKnobSpin->setValue ( value ); break;
    case paramWander:             fKnobWander->setValue ( value ); break;

    case paramInHighCut:       fKnobInHighCut->setValue ( value ); break;
    case paramEarlyDamp:       fKnobEarlyDamp->setValue ( value ); break;
    case paramLateDamp:         fKnobLateDamp->setValue ( value ); break;

    case paramInLowCut:         fKnobInLowCut->setValue ( value ); break;
    case paramBoost:               fKnobBoost->setValue ( value ); break;
    case paramBoostLPF:         fKnobBoostLPF->setValue ( value ); break;
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
          currentBank = b;
          currentPreset[currentBank] = p;
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
        if (rectPresets[row].contains ( ev.pos ))
        {
          currentPreset[currentBank] = row;
          presetClicked = true;
        }
      }

      if (presetClicked)
      {
        setState("preset", banks[currentBank].presets[currentPreset[currentBank]].name);
        updatePresetDefaults();

        const float *preset = banks[currentBank].presets[currentPreset[currentBank]].params;

        fKnobSize->setValue ( preset[paramSize] );
        fKnobWidth->setValue ( preset[paramWidth] );
        fKnobPredelay->setValue ( preset[paramPredelay] );
        fKnobDecay->setValue ( preset[paramDecay] );

        fKnobDiffuse->setValue ( preset[paramDiffuse] );
        fKnobSpin->setValue ( preset[paramSpin] );
        fKnobWander->setValue ( preset[paramWander] );

	fKnobInHighCut->setValue ( preset[paramInHighCut] );
	fKnobEarlyDamp->setValue ( preset[paramEarlyDamp] );
	fKnobLateDamp->setValue ( preset[paramLateDamp] );

	fKnobInLowCut->setValue ( preset[paramInLowCut] );
	fKnobBoost->setValue ( preset[paramBoost] );
	fKnobBoostLPF->setValue ( preset[paramBoostLPF] );

        for ( uint32_t i = 0; i < paramCount; i++ ) {
	  // Don't set sliders
	  if (i != paramDry && i != paramEarly && i != paramEarlySend && i != paramLate) {
            setParameterValue ( i, preset[i] );
            spectrogram->setParameterValue(i, preset[i]);
	  }
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
  nanoText.beginFrame ( this );
  nanoText.fontSize ( 16 );
  nanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  nanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';

  std::snprintf ( strBuf, 32, "%i%%", int ( fSliderDry_level->getValue() ) );
  nanoText.textBox ( 17 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( fSliderEarly_level->getValue() ) );
  nanoText.textBox ( 57 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( fSliderEarlySend->getValue() ) );
  nanoText.textBox ( 97 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( fSliderLate_level->getValue() ) );
  nanoText.textBox (137 - 2, 330, 35.0f, strBuf, nullptr );

  // print labels;
  nanoText.fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  nanoText.fontSize ( 16 );
  nanoText.textBox (  10, 130, 40, "Dry\nLevel",   nullptr );
  nanoText.textBox (  50, 130, 40, "Early\nLevel", nullptr );
  nanoText.textBox (  90, 130, 40, "Early\nSend",  nullptr );
  nanoText.textBox ( 130, 130, 40, "Late\nLevel",  nullptr );

  nanoText.endFrame();

  //draw faders
  r =  98.0f/255.f;
  g = 130.0f/255.f;
  b = 161.0f/255.f;
  glColor4f ( r, g, b, 1.0f );
  uint dry = ( float ( fSliderDry_level->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint early = ( float ( fSliderEarly_level->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint early_send = ( float ( fSliderEarlySend->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint late = ( float ( fSliderLate_level->getValue() ) / 100.0 ) * 160.0 + 1.0f;

  rectSliders[0].setHeight ( dry );
  rectSliders[0].setY ( 118 + 200 - dry );

  rectSliders[1].setHeight ( early );
  rectSliders[1].setY ( 118 + 200 - early );

  rectSliders[2].setHeight ( early_send );
  rectSliders[2].setY ( 118 + 200 - early_send );

  rectSliders[3].setHeight ( late );
  rectSliders[3].setY ( 118 + 200 - late );

  if ( dry > 1 )
    rectSliders[0].draw();
  if ( early > 1 )
    rectSliders[1].draw();
  if ( early_send > 1 )
    rectSliders[2].draw();
  if ( late > 1 )
    rectSliders[3].draw();

  glColor4f ( 1.0f,1.0f,1.0f,1.0f );

  nanoText.beginFrame ( this );
  nanoText.fontSize ( 18 );
  nanoText.textAlign ( NanoVG::ALIGN_RIGHT | NanoVG::ALIGN_TOP );

  Color bright = Color ( 0.90f, 0.95f, 1.00f );
  Color dim    = Color ( 0.65f, 0.65f, 0.65f );

  for (int row = 0; row < NUM_BANKS; row ++)
  {
    DGL::Rectangle<int> bank = rectBanks[row];
    if (currentBank == row) {
      fImgTabOn.drawAt ( bank.getX(), bank.getY() );
      nanoText.fillColor ( bright );
    } else {
      fImgTabOff.drawAt ( bank.getX(), bank.getY() );
      nanoText.fillColor ( dim );
    }

    nanoText.textBox ( bank.getX() - 3, bank.getY() + 2, bank.getWidth(), banks[row].name, nullptr );
  }


  nanoText.textAlign ( NanoVG::ALIGN_LEFT | NanoVG::ALIGN_TOP );

  for (int row = 0; row < PRESETS_PER_BANK; row ++)
  {
    DGL::Rectangle<int> presetRect = rectPresets[row];
    nanoText.fillColor( row == currentPreset[currentBank] ? bright : dim );
    nanoText.textBox ( presetRect.getX() + 3, presetRect.getY() + 2, presetRect.getWidth(), banks[currentBank].presets[row].name, nullptr );
  }

  nanoText.endFrame();

  if (displayAbout) {
    spectrogram->hide();
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
      "Dragonfly Room Reverb is a free audio effect\n"
      "based on Freeverb3 ProG.\n\n"
      "Version: %d.%d.%d%s  License: GPL 3+\n\n"
      "• Michael Willis - Plugin Development\n"
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
      glColor4f ( 1.0f,1.0f,1.0f,1.0f );
      fImgQuestion.drawAt ( rectAbout.getX(), rectAbout.getY() );
  }

}

void DragonflyReverbUI::uiIdle() {
  spectrogram->uiIdle();
}

void DragonflyReverbUI::updatePresetDefaults() {
  const float *preset = banks[currentBank].presets[currentPreset[currentBank]].params;

  fKnobSize->setDefault ( preset[paramSize] );
  fKnobWidth->setDefault ( preset[paramWidth] );
  fKnobPredelay->setDefault ( preset[paramPredelay] );
  fKnobDecay->setDefault ( preset[paramDecay] );

  fKnobDiffuse->setDefault ( preset[paramDiffuse] );
  fKnobSpin->setDefault ( preset[paramSpin] );
  fKnobWander->setDefault ( preset[paramWander] );

  fKnobInHighCut->setDefault ( preset[paramInHighCut] );
  fKnobEarlyDamp->setDefault ( preset[paramEarlyDamp] );
  fKnobLateDamp->setDefault ( preset[paramLateDamp] );

  fKnobInLowCut->setDefault ( preset[paramInLowCut] );
  fKnobBoost->setDefault ( preset[paramBoost] );
  fKnobBoostLPF->setDefault ( preset[paramBoostLPF] );
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
