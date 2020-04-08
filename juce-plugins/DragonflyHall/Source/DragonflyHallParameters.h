#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class DragonflyHallParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String dryLevelID, dryLevelName, dryLevelLabel;
    static const float dryLevelMin, dryLevelMax, dryLevelDefault, dryLevelStep;
    static const String earlyLevelID, earlyLevelName, earlyLevelLabel;
    static const float earlyLevelMin, earlyLevelMax, earlyLevelDefault, earlyLevelStep;
    static const String lateLevelID, lateLevelName, lateLevelLabel;
    static const float lateLevelMin, lateLevelMax, lateLevelDefault, lateLevelStep;
    static const String sizeID, sizeName, sizeLabel;
    static const float sizeMin, sizeMax, sizeDefault, sizeStep;
    static const String widthID, widthName, widthLabel;
    static const float widthMin, widthMax, widthDefault, widthStep;
    static const String predelayID, predelayName, predelayLabel;
    static const float predelayMin, predelayMax, predelayDefault, predelayStep;
    static const String diffuseID, diffuseName, diffuseLabel;
    static const float diffuseMin, diffuseMax, diffuseDefault, diffuseStep;
    static const String lowCutID, lowCutName, lowCutLabel;
    static const float lowCutMin, lowCutMax, lowCutDefault, lowCutStep;
    static const String lowCrossID, lowCrossName, lowCrossLabel;
    static const float lowCrossMin, lowCrossMax, lowCrossDefault, lowCrossStep;
    static const String lowMultID, lowMultName, lowMultLabel;
    static const float lowMultMin, lowMultMax, lowMultDefault, lowMultStep;
    static const String highCutID, highCutName, highCutLabel;
    static const float highCutMin, highCutMax, highCutDefault, highCutStep;
    static const String highCrossID, highCrossName, highCrossLabel;
    static const float highCrossMin, highCrossMax, highCrossDefault, highCrossStep;
    static const String highMultID, highMultName, highMultLabel;
    static const float highMultMin, highMultMax, highMultDefault, highMultStep;
    static const String spinID, spinName, spinLabel;
    static const float spinMin, spinMax, spinDefault, spinStep;
    static const String wanderID, wanderName, wanderLabel;
    static const float wanderMin, wanderMax, wanderDefault, wanderStep;
    static const String decayID, decayName, decayLabel;
    static const float decayMin, decayMax, decayDefault, decayStep;
    static const String earlySendID, earlySendName, earlySendLabel;
    static const float earlySendMin, earlySendMax, earlySendDefault, earlySendStep;
    static const String modulationID, modulationName, modulationLabel;
    static const float modulationMin, modulationMax, modulationDefault, modulationStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DragonflyHallParameters(AudioProcessorValueTreeState& vts,
                            AudioProcessorValueTreeState::Listener* processor);
    ~DragonflyHallParameters();

    void detachControls();
    void attachControls(
        Slider& dryLevelKnob,
        Slider& earlyLevelKnob,
        Slider& lateLevelKnob,
        Slider& sizeKnob,
        Slider& widthKnob,
        Slider& predelayKnob,
        Slider& diffuseKnob,
        Slider& lowCutKnob,
        Slider& lowCrossKnob,
        Slider& lowMultKnob,
        Slider& highCutKnob,
        Slider& highCrossKnob,
        Slider& highMultKnob,
        Slider& spinKnob,
        Slider& wanderKnob,
        Slider& decayKnob,
        Slider& earlySendKnob,
        Slider& modulationKnob);

    // working parameter values
    float dryLevel;
    float earlyLevel;
    float lateLevel;
    float size;
    float width;
    float predelay;
    float diffuse;
    float lowCut;
    float lowCross;
    float lowMult;
    float highCut;
    float highCross;
    float highMult;
    float spin;
    float wander;
    float decay;
    float earlySend;
    float modulation;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> earlyLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lateLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> predelayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> diffuseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowCrossAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowMultAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highCutAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highCrossAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highMultAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> spinAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wanderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> earlySendAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> modulationAttachment;

    // Listener objects link parameters to working variables
    FloatListener dryLevelListener;
    FloatListener earlyLevelListener;
    FloatListener lateLevelListener;
    FloatListener sizeListener;
    FloatListener widthListener;
    FloatListener predelayListener;
    FloatListener diffuseListener;
    FloatListener lowCutListener;
    FloatListener lowCrossListener;
    FloatListener lowMultListener;
    FloatListener highCutListener;
    FloatListener highCrossListener;
    FloatListener highMultListener;
    FloatListener spinListener;
    FloatListener wanderListener;
    FloatListener decayListener;
    FloatListener earlySendListener;
    FloatListener modulationListener;
};
