module main;

import std.math;
import std.algorithm;
import std.stdio;

import dplug.core,
       dplug.client;

import gui;

import dsp.effects;
import dsp.early;

mixin(pluginEntryPoints!DragonflyReverbClient);

enum : int {
    paramMix,

    paramEffect1Gain,
    paramEffect1Send,
    paramEffect1Algorithm,
    paramEffect1EarlyReflectionPattern,
    paramEffect1Size,
    paramEffect1Width,
    paramEffect1Predelay,
    paramEffect1Decay,
    paramEffect1Damp,
    paramEffect1Diffuse,
    paramEffect1Modulation,
    paramEffect1Spin,
    paramEffect1Wander,
    paramEffect1HighCross,
    paramEffect1HighMult,
    paramEffect1HighCut,
    paramEffect1HighCutHard,
    paramEffect1LowCross,
    paramEffect1LowMult,
    paramEffect1LowCut,
    paramEffect1LowCutHard,

    paramEffect2Gain,
    paramEffect2Algorithm,
    paramEffect2EarlyReflectionPattern,
    paramEffect2Size,
    paramEffect2Width,
    paramEffect2Predelay,
    paramEffect2Decay,
    paramEffect2Damp,
    paramEffect2Diffuse,
    paramEffect2Modulation,
    paramEffect2Spin,
    paramEffect2Wander,
    paramEffect2HighCross,
    paramEffect2HighMult,
    paramEffect2HighCut,
    paramEffect2HighCutHard,
    paramEffect2LowCross,
    paramEffect2LowMult,
    paramEffect2LowCut,
    paramEffect2LowCutHard,
}

final class DragonflyReverbClient : dplug.client.Client
{
public:
nothrow:
@nogc:

    this()
    {

      noEffect = mallocNew!NoEffect();
      earlyEffect1 = mallocNew!EarlyEffect();
      earlyEffect2 = mallocNew!EarlyEffect();
      hallEffect1 = mallocNew!HallEffect();
      hallEffect2 = mallocNew!HallEffect();
      plateEffect1 = mallocNew!PlateEffect();
      plateEffect2 = mallocNew!PlateEffect();
      tankEffect1 = mallocNew!TankEffect();
      tankEffect2 = mallocNew!TankEffect();
      roomEffect1 = mallocNew!RoomEffect();
      roomEffect2 = mallocNew!RoomEffect();

      // These must have the same order as the enum
      effects1 = [noEffect, earlyEffect1, hallEffect1, plateEffect1, tankEffect1, roomEffect1];
      effects2 = [noEffect, earlyEffect2, hallEffect2, plateEffect2, tankEffect2, roomEffect2];
    }

    override PluginInfo buildPluginInfo()
    {
        static immutable PluginInfo pluginInfo = parsePluginInfo(import("plugin.json"));
        return pluginInfo;
    }

    // This is an optional overload, default is zero parameter.
    // Caution when adding parameters: always add the indices
    // in the same order as the parameter enum.
    override Parameter[] buildParameters()
    {
        auto params = makeVec!Parameter();

        params ~= mallocNew!LinearFloatParameter(paramMix, "Mix", "%", 0.0f, 100.0f, 100.0f) ;

        params ~= mallocNew!GainParameter(paramEffect1Gain, "Effect 1 Gain", 10.0f, 0.0f);
        params ~= mallocNew!GainParameter(paramEffect1Send, "Effect 1 Send", 10.0f, 0.0f);
        params ~= mallocNew!EnumParameter(paramEffect1Algorithm, "Effect 1 Algorithm", effectAlgorithmNames, 1);
        params ~= mallocNew!EnumParameter(paramEffect1EarlyReflectionPattern,
            "Effect 1 Early Reflection Pattern", earlyReflectionPatternNames, 2);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Size, "Effect 1 Size", "m", 10.0f, 60.0f, 20.0);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Width, "Effect 1 Width", "%", 0.0f, 100.0f, 100.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Predelay, "Effect 1 Predelay", "ms", 0, 100, 4);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Decay, "Effect 1 Decay Time", "s", 0.1f, 10.0f, 0.5f);
        params ~= mallocNew!LogFloatParameter(paramEffect1Damp, "Effect 1 Damp", "Hz", 20, 20000, 10000);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Diffuse, "Effect 1 Diffuse", "%", 0.0f, 100.0f, 80.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Modulation, "Effect 1 Mod", "%", 0.0f, 100.0f, 20.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Spin, "Effect 1 Spin", "Hz", 0.0f, 10.0f, 1.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect1Wander, "Effect 1 Wander", "%", 0.0f, 100.0f, 20.0f);
        params ~= mallocNew!LogFloatParameter(paramEffect1HighCross, "Effect 1 High Cross", "Hz", 20, 20000, 6000);
        params ~= mallocNew!LinearFloatParameter(paramEffect1HighMult, "Effect 1 High Mult", "x", 0.1f, 2.5f, 0.5f);
        params ~= mallocNew!LogFloatParameter(paramEffect1HighCut, "Effect 1 High Cut", "Hz", 20, 20000, 12000);
        params ~= mallocNew!BoolParameter(paramEffect1HighCutHard, "Effect 1 High Cut Hard", false);
        params ~= mallocNew!LogFloatParameter(paramEffect1LowCross, "Effect 1 Low Cross", "Hz", 20, 20000, 1000);
        params ~= mallocNew!LinearFloatParameter(paramEffect1LowMult, "Effect 1 Low Mult", "x", 0.1f, 2.5f, 1.5f);
        params ~= mallocNew!LogFloatParameter(paramEffect1LowCut, "Effect 1 Low Cut", "Hz", 20, 20000, 400);
        params ~= mallocNew!BoolParameter(paramEffect1LowCutHard, "Effect 1 Low Cut Hard", false);

        params ~= mallocNew!GainParameter(paramEffect2Gain, "Effect 2 Gain", 10.0f, 0.0f);
        params ~= mallocNew!EnumParameter(paramEffect2Algorithm, "Effect 2 Algorithm", effectAlgorithmNames, 2);
        params ~= mallocNew!EnumParameter(paramEffect2EarlyReflectionPattern,
            "Effect 2 Early Reflection Pattern", earlyReflectionPatternNames, 2);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Size, "Effect 2 Size", "m", 10.0f, 60.0f, 20.0);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Width, "Effect 2 Width", "%", 0.0f, 100.0f, 100.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Predelay, "Effect 2 Predelay", "ms", 0, 100, 4);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Decay, "Effect 2 Decay Time", "s", 0.1f, 10.0f, 0.5f);
        params ~= mallocNew!LogFloatParameter(paramEffect2Damp, "Effect 2 Damp", "Hz", 20, 20000, 10000);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Diffuse, "Effect 2 Diffuse", "%", 0.0f, 100.0f, 80.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Modulation, "Effect 2 Mod", "%", 0.0f, 100.0f, 20.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Spin, "Effect 2 Spin", "Hz", 0.0f, 10.0f, 1.0f);
        params ~= mallocNew!LinearFloatParameter(paramEffect2Wander, "Effect 2 Wander", "%", 0.0f, 100.0f, 20.0f);
        params ~= mallocNew!LogFloatParameter(paramEffect2HighCross, "Effect 2 High Cross", "Hz", 20, 20000, 6000);
        params ~= mallocNew!LinearFloatParameter(paramEffect2HighMult, "Effect 2 High Mult", "x", 0.1f, 2.5f, 0.5f);
        params ~= mallocNew!LogFloatParameter(paramEffect2HighCut, "Effect 2 High Cut", "Hz", 20, 20000, 12000);
        params ~= mallocNew!BoolParameter(paramEffect2HighCutHard, "Effect 2 High Cut Hard", false);
        params ~= mallocNew!LogFloatParameter(paramEffect2LowCross, "Effect 2 Low Cross", "Hz", 20, 20000, 1000);
        params ~= mallocNew!LinearFloatParameter(paramEffect2LowMult, "Effect 2 Low Mult", "x", 0.1f, 2.5f, 1.5f);
        params ~= mallocNew!LogFloatParameter(paramEffect2LowCut, "Effect 2 Low Cut", "Hz", 20, 20000, 400);
        params ~= mallocNew!BoolParameter(paramEffect2LowCutHard, "Effect 2 Low Cut Hard", false);

        return params.releaseData();
    }

    override LegalIO[] buildLegalIO()
    {
        auto io = makeVec!LegalIO();
        io ~= LegalIO(2, 2);
        return io.releaseData();
    }

    // This override is also optional. It allows to split audio buffers in order to never
    // exceed some amount of frames at once.
    // This can be useful as a cheap chunking for parameter smoothing.
    // Buffer splitting also allows to allocate statically or on the stack with less worries.
    override int maxFramesInProcess() const //nothrow @nogc
    {
        return 512;
    }

    override void reset(double sampleRate, int maxFrames, int numInputs, int numOutputs) nothrow @nogc
    {
        // Clear here any state and delay buffers you might have.

        assert(maxFrames <= 512); // guaranteed by audio buffer splitting

        for (int effect = 0; effect < effectCount; effect++) {
          effects1[effect].reset(sampleRate, maxFrames);
          effects2[effect].reset(sampleRate, maxFrames);          
        }
    }

    override void processAudio(const(float*)[] inputs, float*[]outputs, int frames,
                               TimeInfo info) nothrow @nogc
    {
        assert(frames <= 512); // guaranteed by audio buffer splitting

        immutable float mix = readParam!float(paramMix) / 100.0f;

        immutable float effect1Gain = pow(10, readParam!float(paramEffect1Gain) / 20); // dB to mult
        immutable float effect1SendToEffect2 = pow(10, readParam!float(paramEffect1Send) / 20); // dB to mult

        // TODO: Effect 1 Algorithm Select

        immutable int effect1EarlyReflectionPattern = readParam!int(paramEffect1EarlyReflectionPattern);
        if (effect1EarlyReflectionPattern != earlyEffect1.getReflectionPattern()) {
          earlyEffect1.setReflectionPattern(effect1EarlyReflectionPattern);
        }

        immutable float effect1Predelay = readParam!float(paramEffect1Predelay);
        earlyEffect1.setPredelaySeconds(effect1Predelay / 1000.0);
        // TODO: Room, Hall, Plate predelay

        immutable float effect1Size = readParam!float(paramEffect1Size);
        earlyEffect1.setSize(effect1Size / 10.0);
        // TODO: Room and Hall Size

        immutable float effect1Width = readParam!float(paramEffect1Width) / 100.0;
        earlyEffect1.setWidth(effect1Width);
        // TODO: Width for Plate, Room and Hall   

        immutable float effect1HighCut = readParam!float(paramEffect1HighCut);
        earlyEffect1.setHighCut(effect1HighCut);
        // TODO: High Cut for Plate, Room and Hall   

        immutable float effect1LowCut = readParam!float(paramEffect1LowCut);
        earlyEffect1.setLowCut(effect1LowCut);
        // TODO: Low Cut for Plate, Room and Hall   

        // immutable bool hardClip = readParam!bool(paramMode);

        for (int f = 0; f < frames; ++f)
        {
            effect1InL[f] = inputs[0][f];
            effect1InR[f] = inputs[1][f];
        }

        effects1[effect1Selected].processAudio(effect1InL, effect1InR, effect1OutL, effect1OutR, frames);

        for (int f = 0; f < frames; ++f)
        {
            effect2InL[f] = inputs[0][f] + effect1OutL[f] * effect1SendToEffect2;
            effect2InR[f] = inputs[1][f] + effect1OutR[f] * effect1SendToEffect2;
        }

        effects2[effect2Selected].processAudio(effect2InL, effect2InR, effect2OutL, effect2OutR, frames);

        for (int f = 0; f < frames; ++f) {
            outputs[0][f] = ((effect1OutL[f] * effect1Gain + effect2OutL[f]) * mix) + (inputs[0][f] * (1.0 - mix));
            outputs[1][f] = ((effect1OutR[f] * effect1Gain + effect2OutR[f]) * mix) + (inputs[1][f] * (1.0 - mix));            
        }

        /// Get access to the GUI
        if (DragonflyReverbGUI gui = cast(DragonflyReverbGUI) graphicsAcquire())
        {
            /// This is where you would update any elements in the gui
            /// such as feeding values to meters.

            graphicsRelease();
        }
    }

    override IGraphics createGraphics()
    {
        return mallocNew!DragonflyReverbGUI(this);
    }

private:
    int effect1Selected = earlyEffect;
    int effect2Selected = 0;

    NoEffect noEffect;
    EarlyEffect earlyEffect1, earlyEffect2;
    HallEffect hallEffect1, hallEffect2;
    PlateEffect plateEffect1, plateEffect2;
    TankEffect tankEffect1, tankEffect2;
    RoomEffect roomEffect1, roomEffect2;

    Effect[effectCount] effects1;
    Effect[effectCount] effects2;

    float[512] effect1InL;
    float[512] effect1InR;
    float[512] effect1OutL;
    float[512] effect1OutR;

    float[512] effect2InL;
    float[512] effect2InR;
    float[512] effect2OutL;
    float[512] effect2OutR;
}

