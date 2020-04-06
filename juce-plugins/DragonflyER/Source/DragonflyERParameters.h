#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class DragonflyERParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String dryLevelID, dryLevelName, dryLevelLabel;
    static const float dryLevelMin, dryLevelMax, dryLevelDefault, dryLevelStep;
    static const String wetLevelID, wetLevelName, wetLevelLabel;
    static const float wetLevelMin, wetLevelMax, wetLevelDefault, wetLevelStep;
    static const String progIndexID, progIndexName, progIndexLabel;
    static const int progIndexEnumCount, progIndexDefault;
    static const String sizeID, sizeName, sizeLabel;
    static const float sizeMin, sizeMax, sizeDefault, sizeStep;
    static const String widthID, widthName, widthLabel;
    static const float widthMin, widthMax, widthDefault, widthStep;
    static const String lowCutID, lowCutName, lowCutLabel;
    static const float lowCutMin, lowCutMax, lowCutDefault, lowCutStep;
    static const String highCutID, highCutName, highCutLabel;
    static const float highCutMin, highCutMax, highCutDefault, highCutStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DragonflyERParameters(AudioProcessorValueTreeState& vts,
                          AudioProcessorValueTreeState::Listener* processor);
    ~DragonflyERParameters();

    static void populateProgramsComboBox(ComboBox&);

    void detachControls();
    void attachControls(
        Slider& dryLevelKnob,
        Slider& wetLevelKnob,
        ComboBox& progIndexCombo,
        Slider& sizeKnob,
        Slider& widthKnob,
        Slider& lowCutKnob,
        Slider& highCutKnob );

    // working parameter values
    float dryLevel;
    float wetLevel;
    enum Program { abruptEcho, backstagePass, concertVenue, damagedGoods, elevatorPitch, floorThirteen, garageBand, homeStudio };
    Program progIndex;
    float size;
    float width;
    float lowCut;
    float highCut;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> progIndexAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highCutAttachment;

    // Listener objects link parameters to working variables
    FloatListener dryLevelListener;
    FloatListener wetLevelListener;
    EnumListener<Program> progIndexListener;
    FloatListener sizeListener;
    FloatListener widthListener;
    FloatListener lowCutListener;
    FloatListener highCutListener;
};
