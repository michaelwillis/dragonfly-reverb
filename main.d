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

enum : int
{
    paramInputGain,
    paramClip,
    paramOutputGain,
    paramMix,
    paramMode,
}


final class DragonflyReverbClient : dplug.client.Client
{
public:
nothrow:
@nogc:

    this()
    {
        // These must have the same order as the enum
        effects1 =
        [
            mallocNew!NoEffect(),
            mallocNew!EarlyEffect(),
            mallocNew!HallEffect(),
            mallocNew!PlateEffect(),
            mallocNew!TankEffect(),
            mallocNew!RoomEffect()
        ];
            
        effects2 = [
            mallocNew!NoEffect(),
            mallocNew!EarlyEffect(),
            mallocNew!HallEffect(),
            mallocNew!PlateEffect(),
            mallocNew!TankEffect(),
            mallocNew!RoomEffect()
        ];
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
        params ~= mallocNew!LinearFloatParameter(paramInputGain, "input gain", "dB", -12.0f, 12.0f, 0.0f) ;
        params ~= mallocNew!LinearFloatParameter(paramClip, "clip", "%", 0.0f, 100.0f, 0.0f) ;
        params ~= mallocNew!LinearFloatParameter(paramOutputGain, "output gain", "db", -12.0f, 12.0f, 0.0f) ;
        params ~= mallocNew!LinearFloatParameter(paramMix, "mix", "%", 0.0f, 100.0f, 100.0f) ;
        params ~= mallocNew!BoolParameter(paramMode, "mode", false);
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
        
        /// Read parameter values
        /// Convert decibel values to floating point
        immutable float input1Send = pow(10, effect1dBSend / 20);
        // immutable float outputGain = pow(10, readParam!float(paramOutputGain) /20);

        immutable float mix = readParam!float(paramMix) / 100.0f;

        // immutable bool hardClip = readParam!bool(paramMode);

        for (int f = 0; f < frames; ++f)
        {
            effect1InL[f] = inputs[0][f];
            effect1InR[f] = inputs[1][f];
        }

        effects1[effect1Selected].processAudio(effect1InL, effect1InR, effect1OutL, effect1OutR, frames);

        for (int f = 0; f < frames; ++f)
        {
            effect2InL[f] = inputs[0][f] + effect1OutL[f] * input1Send;
            effect2InR[f] = inputs[1][f] + effect1OutR[f] * input1Send;
        }

        effects2[effect2Selected].processAudio(effect2InL, effect2InR, effect2OutL, effect2OutR, frames);

        for (int f = 0; f < frames; ++f) {
            outputs[0][f] = ((effect1OutL[f] + effect2OutL[f]) * mix) + (inputs[0][f] * (1.0 - mix));
            outputs[1][f] = ((effect1OutR[f] + effect2OutR[f]) * mix) + (inputs[1][f] * (1.0 - mix));            
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

    // TODO - Maybe NoEffect shouldn't apply send
    float effect1dBSend = -12.0;
    
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

