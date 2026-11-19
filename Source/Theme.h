#pragma once

#include <JuceHeader.h>

namespace ScaleSpaceColours
{
    inline const juce::Colour background  { 0xff111111 };
    inline const juce::Colour foreground  { 0xffffffff };
    inline const juce::Colour accent      { 0xff00aaaa };
    inline const juce::Colour controlFill { 0xff3d3d3d };
}

namespace ScaleSpaceLayout
{
    // Uniform spacing used between the main UI elements and the window edge.
    inline constexpr int gap = 8;
}
