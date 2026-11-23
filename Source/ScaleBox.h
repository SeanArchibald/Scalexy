#ifndef SCALEBOX_H
#define SCALEBOX_H

#include <JuceHeader.h>

struct IconTextButton : public juce::TextButton
{
    explicit IconTextButton (const juce::String& text) : juce::TextButton (text) {}

    void setIcon (std::unique_ptr<juce::Drawable> newIcon);
    void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override;

private:
    std::unique_ptr<juce::Drawable> icon;
};

struct ScaleBox : public juce::Component
{
    ScaleBox();
    ~ScaleBox();
    
    void paint(Graphics& g) override;
    void resized() override;
    
    IconTextButton openSclButton { "SCL" };
    IconTextButton openKbmButton { "KBM" };
    juce::DrawableButton randomButton { "Random", juce::DrawableButton::ImageFitted };
    juce::DrawableButton openEditorButton { "Editor", juce::DrawableButton::ImageFitted };
    juce::DrawableButton resetButton { "Reset", juce::DrawableButton::ImageFitted };
    juce::Label sclNameLabel;
    juce::Label kbmNameLabel;

    void setRightAlignLabels (bool shouldRightAlign);
    void setButtonsAtTop (bool shouldBeAtTop);

private:
    bool rightAlignLabels = false;
    bool buttonsAtTop = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleBox)
};

#endif // SCALEBOX_H
