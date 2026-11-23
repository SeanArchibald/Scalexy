/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TuningEditor.h"
#include "ScaleBox.h"
#include "MultiDimensionalPad.h"
#include "Theme.h"
//==============================================================================
struct OutlinedTextButton : public juce::TextButton
{
    using juce::TextButton::TextButton;
    void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override;
};

/**
*/
class ScaleSpaceAudioProcessorEditor  : public juce::AudioProcessorEditor
                                        , public juce::FileDragAndDropTarget
                                        , juce::Timer
{
public:
    ScaleSpaceAudioProcessorEditor (ScaleSpaceAudioProcessor&);
    ~ScaleSpaceAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    bool isInterestedInFileDrag (const juce::StringArray &files) override;
    void filesDropped (const juce::StringArray &files, int x, int y) override;
    void timerCallback() override;
        
protected:
    void scaleOpenButtonClicked(const uint32_t tuningNumber, const uint32_t tunType, const char* chooserText, const char* fileExt);
    void randomiseSclButtonClicked(const uint32_t tuningNumber);
    void exportButtonClicked(const uint32_t tunType, const char* chooserText, const char* fileExt);
    void updateAllNameLabels();
    void updateNameLabel(juce::Label &label, const uint32_t nameIndex);
    bool applyNewTuningFromEditor(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType);
    void openEditorButtonClicked(const uint32_t tuningNumber);
    void resetButtonClicked(const uint32_t tuningNumber);
    void settingsButtonClicked();
    void openTuningEditor(const uint32_t tuningNumber, const float desktopScaleFactor);

private:
    static constexpr int scaleBoxWidth   = 98;
    static constexpr int scaleBoxHeight  = 168;
    static constexpr int padSize         = 451;
    static constexpr int statusBarHeight = 30;
    
    // The design size wraps tightly around the four scale boxes, the XY pad
    // and a single status-bar row, with a uniform gap between everything.
    static constexpr int designWidth  = ScaleSpaceLayout::gap * 4 + scaleBoxWidth * 2 + padSize;
    static constexpr int designHeight = ScaleSpaceLayout::gap * 3 + padSize + statusBarHeight;
    static constexpr int minWidth = designWidth / 2;
    static constexpr int minHeight = designHeight / 2;
    static constexpr int maxWidth = designWidth * 3;
    static constexpr int maxHeight = designHeight * 3;
    
    void layoutContent();
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ScaleSpaceAudioProcessor& audioProcessor;
    
    juce::PropertiesFile settingsFile;
    
    // All UI lives inside this fixed-size component. Its transform is updated
    // on resize so that the whole interface scales while keeping its aspect ratio.
    juce::Component contentComponent;
    
    std::unique_ptr<TuningEditor> tuningEditor;
    juce::Component::SafePointer<ExternalTuningEditorWindow> editorWindow;
    
    MultiDimensionalPad multiPad;
    
    ScaleBox scaleBox1;
    ScaleBox scaleBox2;
    ScaleBox scaleBox3;
    ScaleBox scaleBox4;

    IconButton settingsButton { "Settings" };

    OutlinedTextButton exportSclButton { "Export SCL" };
    OutlinedTextButton exportKbmButton { "Export KBM" };
    
    juce::ToggleButton smoothToggle;
    juce::Slider smoothAmtSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> yAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> smoothAmtSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> smoothToggleAttachment;
    
    std::unique_ptr<juce::FileChooser> chooser;
    
    bool initialUIUpdate;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleSpaceAudioProcessorEditor)
};
