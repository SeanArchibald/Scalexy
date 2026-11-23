#ifndef MULTIDIMENSIONALPAD_H
#define MULTIDIMENSIONALPAD_H

#include <JuceHeader.h>
#include <array>

struct MultiDimensionalPad : public juce::Component
                            , public juce::Slider::Listener
{
    MultiDimensionalPad();
    ~MultiDimensionalPad();
    
    enum Thumb {
        XY_PAD_THUMB = 0,
        NO_THUMB_SELECTED = 1
    };
    
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void paint(Graphics& g) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    
    juce::Label& getScaleLabel (int index)   { return scaleLabels[(size_t) index]; }
    juce::Label& getMappingLabel (int index) { return mappingLabels[(size_t) index]; }
    
    juce::Point<float> thumbPos;
    float thumbRadius;
    uint32_t thumbDragged;
    
    juce::Slider xSlider;
    juce::Slider ySlider;
    
    juce::Rectangle<float> xyPad;
    
private:
    std::array<juce::Label, 4> scaleLabels;
    std::array<juce::Label, 4> mappingLabels;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiDimensionalPad)
};

#endif // MULTIDIMENSIONALPAD_H
