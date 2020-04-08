# Dragonfly Reverb adapted for JUCE

## The original [Dragonfly Reverb](https://michaelwillis.github.io/dragonfly-reverb/) project
[Dragonfly Reverb](https://michaelwillis.github.io/dragonfly-reverb/) is a bundle of free audio effects for Linux, MacOS, and Windows, based on [Freeverb3 by Teru Kamogashira](http://www.nongnu.org/freeverb3/) and developed by Michael Willis and others. See https://michaelwillis.github.io/dragonfly-reverb.

## This repository adds JUCE versions
**This repository** is a fork of [Michael's original dragonfly-reverb repo](https://github.com/michaelwillis/dragonfly-reverb), in which I have added [JUCE](https://github.com/WeAreROLI/JUCE)-based versions of the four reverb effects, to complement the originals which were built with the [DPF DISTRHO Plugin Framework](https://github.com/DISTRHO/DPF).

DPF offers superior Linux support, including the ability to build LADSPA, DSSI, LV2 and Linux-VST formats. As used in the original Dragonfly code, however, DPF has three severe limitations for non-Linux platforms:
1. No native build system for Windows: an elaborate Linux cross-compile environment must be used.
2. Dependency on OpenGL for GUI rendering, which Apple has deprecated for newer versions of MacOS.
3. No ability to build VST3 or AAX formats plug-ins.

The JUCE framework addresses all three issues, and can be used for open-source work under the GPL3. The basic plug-in structure is highly similar between DPF and JUCE, so re-wrapping the Freeverb3 DSP code for JUCE was straightforward. The basic code was generated semi-automatically, using a [Python-based template/script system](https://github.com/getdunne/audio-effects/tree/master/effects-new/Generator%20Scripts) I developed for [another open-source project](https://github.com/getdunne/audio-effects).

The GUIs for the JUCE-based plug-ins are simplified from the elegant originals, but I have used some of the same color schemes and the original logo images, in an attempt to pay respect to the original designs by Michael Willis and Rob van den Berg.

## What you need to build the JUCE-based plug-ins
**[PlugInGuru, Inc.](https://www.pluginguru.com) is not able to provide individual developer support for this, or any other open-source software. Audio plug-in development is *not simple* and is not recommended for novice programmers.**

To build the JUCE-based plug-ins, you will need the JUCE framework, which you can get from ROLI Inc. at https://shop.juce.com/get-juce. The JUCE software includes *Projucer*, an app which reads the *.jucer* files provided here under the *juce-plugins* folder, and produces ready-to-build projects for [Microsoft Visual Studio](https://visualstudio.microsoft.com/) on Windows and [Xcode](https://developer.apple.com/xcode/) on MacOS.

The JUCE framework is dual-licensed. A commercial license is required if you want to distribute a JUCE-based program as closed-source, but the framework can also be used at no charge for open-source works, provided the GPL3 license is used, as is the case here.

If you want to build VST or VST3 plug-ins, you will also need a copy of the appropriate SDK from Steinberg ("VST" is a trademark of Steinberg Media Technologies GmbH). To distribute VST/VST3 plug-ins you must obtain a license from Steinberg. See https://www.steinberg.net/en/company/developers.html.

Should you wish to build AAX versions, you will need a commercial license from [AVID Technology, Inc.](http://developer.avid.com/). AAX is *not an open format*.

## License

All versions of Dragonfly Reverb are distributed under the [GPL 3.0 License](https://www.gnu.org/licenses/gpl-3.0.en.html)
