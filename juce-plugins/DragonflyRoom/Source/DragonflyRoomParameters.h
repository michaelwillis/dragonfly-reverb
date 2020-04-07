#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class DragonflyRoomParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String dryLevelID, dryLevelName, dryLevelLabel;
    static const float dryLevelMin, dryLevelMax, dryLevelDefault, dryLevelStep;
    static const String earlyLevelID, earlyLevelName, earlyLevelLabel;
    static const float earlyLevelMin, earlyLevelMax, earlyLevelDefault, earlyLevelStep;
    static const String earlySendID, earlySendName, earlySendLabel;
    static const float earlySendMin, earlySendMax, earlySendDefault, earlySendStep;
    static const String lateLevelID, lateLevelName, lateLevelLabel;
    static const float lateLevelMin, lateLevelMax, lateLevelDefault, lateLevelStep;
    static const String sizeID, sizeName, sizeLabel;
    static const float sizeMin, sizeMax, sizeDefault, sizeStep;
    static const String widthID, widthName, widthLabel;
    static const float widthMin, widthMax, widthDefault, widthStep;
    static const String predelayID, predelayName, predelayLabel;
    static const float predelayMin, predelayMax, predelayDefault, predelayStep;
    static const String decayID, decayName, decayLabel;
    static const float decayMin, decayMax, decayDefault, decayStep;
    static const String diffuseID, diffuseName, diffuseLabel;
    static const float diffuseMin, diffuseMax, diffuseDefault, diffuseStep;
    static const String spinID, spinName, spinLabel;
    static const float spinMin, spinMax, spinDefault, spinStep;
    static const String wanderID, wanderName, wanderLabel;
    static const float wanderMin, wanderMax, wanderDefault, wanderStep;
    static const String highCutID, highCutName, highCutLabel;
    static const float highCutMin, highCutMax, highCutDefault, highCutStep;
    static const String earlyDampID, earlyDampName, earlyDampLabel;
    static const float earlyDampMin, earlyDampMax, earlyDampDefault, earlyDampStep;
    static const String lateDampID, lateDampName, lateDampLabel;
    static const float lateDampMin, lateDampMax, lateDampDefault, lateDampStep;
    static const String lowBoostID, lowBoostName, lowBoostLabel;
    static const float lowBoostMin, lowBoostMax, lowBoostDefault, lowBoostStep;
    static const String boostFreqID, boostFreqName, boostFreqLabel;
    static const float boostFreqMin, boostFreqMax, boostFreqDefault, boostFreqStep;
    static const String lowCutID, lowCutName, lowCutLabel;
    static const float lowCutMin, lowCutMax, lowCutDefault, lowCutStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DragonflyRoomParameters(AudioProcessorValueTreeState& vts,
                            AudioProcessorValueTreeState::Listener* processor);
    ~DragonflyRoomParameters();

    void detachControls();
    void attachControls(
        Slider& dryLevelKnob,
        Slider& earlyLevelKnob,
        Slider& earlySendKnob,
        Slider& lateLevelKnob,
        Slider& sizeKnob,
        Slider& widthKnob,
        Slider& predelayKnob,
        Slider& decayKnob,
        Slider& diffuseKnob,
        Slider& spinKnob,
        Slider& wanderKnob,
        Slider& highCutKnob,
        Slider& earlyDampKnob,
        Slider& lateDampKnob,
        Slider& lowBoostKnob,
        Slider& boostFreqKnob,
        Slider& lowCutKnob );

    // working parameter values
    float dryLevel;
    float earlyLevel;
    float earlySend;
    float lateLevel;
    float size;
    float width;
    float predelay;
    float decay;
    float diffuse;
    float spin;
    float wander;
    float highCut;
    float earlyDamp;
    float lateDamp;
    float lowBoost;
    float boostFreq;
    float lowCut;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> earlyLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> earlySendAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lateLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> predelayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> diffuseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> spinAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wanderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highCutAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> earlyDampAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lateDampAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowBoostAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> boostFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;

    // Listener objects link parameters to working variables
    FloatListener dryLevelListener;
    FloatListener earlyLevelListener;
    FloatListener earlySendListener;
    FloatListener lateLevelListener;
    FloatListener sizeListener;
    FloatListener widthListener;
    FloatListener predelayListener;
    FloatListener decayListener;
    FloatListener diffuseListener;
    FloatListener spinListener;
    FloatListener wanderListener;
    FloatListener highCutListener;
    FloatListener earlyDampListener;
    FloatListener lateDampListener;
    FloatListener lowBoostListener;
    FloatListener boostFreqListener;
    FloatListener lowCutListener;
};
