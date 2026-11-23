#ifndef SCALEBOX_H
#define SCALEBOX_H

#include <JuceHeader.h>

struct IconButton : public juce::Button
{
    IconButton (const juce::String& buttonName, const juce::String& buttonText = {})
        : juce::Button (buttonName), text (buttonText) {}

    void setIcon (std::unique_ptr<juce::Drawable> newIcon);
    void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override;

private:
    juce::String text;
    std::unique_ptr<juce::Drawable> icon;
};

struct ScaleBox : public juce::Component
{
    ScaleBox();
    ~ScaleBox();
    
    void resized() override;
    
    IconButton openSclButton { "Load SCL", "Load SCL" };
    IconButton openKbmButton { "Load KBM", "Load KBM" };
    IconButton randomButton { "Random SCL", "Random" };
    IconButton openEditorButton { "Editor", "Editor" };
    IconButton resetButton { "Reset", "Reset" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleBox)
};

#endif // SCALEBOX_H
