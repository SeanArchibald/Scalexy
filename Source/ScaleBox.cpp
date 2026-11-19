#include "ScaleBox.h"
#include "Theme.h"

ScaleBox::ScaleBox()
{
    addAndMakeVisible(openSclButton);
    addAndMakeVisible(openKbmButton);
    addAndMakeVisible(openEditorButton);
    addAndMakeVisible(scaleLabel);
    addAndMakeVisible(sclNameLabel);
    addAndMakeVisible(kbmNameLabel);
    scaleLabel.setColour (juce::Label::textColourId, ScaleSpaceColours::foreground);
    scaleLabel.setMinimumHorizontalScale(1.0f);
    openSclButton.setColour (juce::TextButton::buttonColourId, ScaleSpaceColours::controlFill);
    openSclButton.setColour (juce::TextButton::textColourOffId, ScaleSpaceColours::foreground);
    openSclButton.setButtonText ("Open SCL File");
    openKbmButton.setColour (juce::TextButton::buttonColourId, ScaleSpaceColours::controlFill);
    openKbmButton.setColour (juce::TextButton::textColourOffId, ScaleSpaceColours::foreground);
    openKbmButton.setButtonText ("Open KBM File");
    openEditorButton.setColour (juce::TextButton::buttonColourId, ScaleSpaceColours::controlFill);
    openEditorButton.setColour (juce::TextButton::textColourOffId, ScaleSpaceColours::foreground);
    openEditorButton.setButtonText ("Edit");
    sclNameLabel.setFont (juce::FontOptions().withHeight(16));
    sclNameLabel.setColour (juce::Label::textColourId, ScaleSpaceColours::foreground);
    sclNameLabel.setMinimumHorizontalScale(1.0f);
    kbmNameLabel.setFont (juce::FontOptions().withHeight(16));
    kbmNameLabel.setColour (juce::Label::textColourId, ScaleSpaceColours::foreground);
    kbmNameLabel.setMinimumHorizontalScale(1.0f);
}

ScaleBox::~ScaleBox()
{
}

void ScaleBox::paint(Graphics& g)
{
    g.setColour(ScaleSpaceColours::accent);
    g.drawRect(getLocalBounds());
}

void ScaleBox::resized()
{
    scaleLabel.setBounds (10, 10, 150, 20);
    openSclButton.setBounds (14, 40, 110, 30);
    openKbmButton.setBounds (14, 75, 110, 30);
    openEditorButton.setBounds (128, 40, 60, 65);
    sclNameLabel.setBounds (10, 115, getWidth() * 0.95f, 20);
    kbmNameLabel.setBounds (10, 140, getWidth() * 0.95f, 20);
}
