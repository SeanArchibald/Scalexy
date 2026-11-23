#pragma once

#include <JuceHeader.h>

namespace ScaleSpaceColours
{
    inline const juce::Colour background  { 0xff333333 };
    inline const juce::Colour foreground  { 0xffffffff };
    inline const juce::Colour accent      { 0xff00dddd };
    inline const juce::Colour controlFill { 0xff000000 };
    inline const juce::Colour buttonOutline { 0xff8e989b };
}

namespace ScaleSpaceLayout
{
    // Uniform spacing used between the main UI elements and the window edge.
    inline constexpr int gap = 8;
}
