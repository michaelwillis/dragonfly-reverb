cmake_minimum_required(VERSION 3.15)
project(DragonflyReverb VERSION 3.2.6)

add_subdirectory(JUCE)

# Config

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# dependencies

add_library(dgl STATIC IMPORTED)
set_property(TARGET dgl PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../dpf/build/libdgl-opengl.a")

if (APPLE)
set(EXTRA_LIBS "-lz")
set(GL_LIBRARIES "-framework OpenGL")
endif (APPLE)


juce_add_plugin(DragonflyHallReverb
    AU_MAIN_TYPE kAudioUnitType_Effect
    COMPANY_COPYRIGHT "GPL-3.0-or-later"
    COMPANY_NAME "Dragonfly Reverb"
    COMPANY_WEBSITE "https://michaelwillis.github.io/dragonfly-reverb/"
    DESCRIPTION "Hall-style reverb plugin"
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    FORMATS AU
    IS_MIDI_EFFECT FALSE
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_CODE Dfhr
    PLUGIN_MANUFACTURER_CODE Dfrv
    PRODUCT_NAME "Dragonfly Hall Reverb"
    VERSION 3.2.7)

target_sources(DragonflyHallReverb
    PRIVATE
        DragonflyWrapper.cpp)

target_include_directories(DragonflyHallReverb
    PRIVATE
        ../common
        ../dpf/distrho
        ../plugins/dragonfly-hall-reverb)

target_compile_definitions(DragonflyHallReverb
    PUBLIC
        JucePlugin_PreferredChannelConfigurations=2,2
        JUCE_CHECK_MEMORY_LEAKS=0
        JUCE_DISABLE_NATIVE_FILECHOOSERS=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=0
        JUCE_USE_CURL=0
        JUCE_USE_FLAC=0
        JUCE_USE_OGGVORBIS=0
        JUCE_USE_XINERAMA=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_ALSA=0
        JUCE_DIRECTSOUND=0
        JUCE_JACK=0
        JUCE_WASAPI=0
        JUCE_WEB_BROWSER=0)

target_link_libraries(DragonflyHallReverb
    PRIVATE
        juce::juce_audio_utils
        dgl
        ${GL_LIBRARIES}
        ${EXTRA_LIBS}
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
