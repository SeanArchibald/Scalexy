#include "ScaleBox.h"
#include "Theme.h"

namespace
{
std::unique_ptr<juce::Drawable> createForegroundIcon (const void* data, size_t size)
{
    auto icon = juce::Drawable::createFromImageData (data, size);
    icon->replaceColour (juce::Colours::black, ScaleSpaceColours::foreground);
    return icon;
}
}

void IconButton::setIcon (std::unique_ptr<juce::Drawable> newIcon)
{
    icon = std::move (newIcon);
    repaint();
}

void IconButton::paintButton (juce::Graphics& g,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown)
{
    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
    {
        g.setColour (ScaleSpaceColours::accent.withAlpha (shouldDrawButtonAsDown ? 0.18f : 0.08f));
        g.fillRect (getLocalBounds());
    }

    constexpr float iconSize = 24.0f;
    constexpr float iconGap = 6.0f;
    const auto font = juce::Font (juce::FontOptions().withHeight (20.0f));
    const auto bounds = getLocalBounds().toFloat();
    const float startX = bounds.getX();
    const float centreY = bounds.getCentreY();
    const float alpha = isEnabled() ? 1.0f : 0.5f;

    if (icon != nullptr)
    {
        icon->setTransformToFit ({ startX, centreY - iconSize * 0.5f, iconSize, iconSize },
                                 juce::RectanglePlacement::centred);
        icon->draw (g, alpha);
    }

    if (! text.isEmpty())
    {
        g.setColour (ScaleSpaceColours::foreground.withMultipliedAlpha (alpha));
        g.setFont (font);
        g.drawText (text,
                    juce::Rectangle<float> (startX + iconSize + iconGap, 0.0f,
                                            bounds.getRight() - startX - iconSize - iconGap,
                                            bounds.getHeight()),
                    juce::Justification::centredLeft, false);
    }
}

ScaleBox::ScaleBox()
{
    addAndMakeVisible(openSclButton);
    addAndMakeVisible(openKbmButton);
    addAndMakeVisible(randomButton);
    addAndMakeVisible(openEditorButton);
    addAndMakeVisible(resetButton);

    openSclButton.setIcon (createForegroundIcon (BinaryData::FileOpenIcon_svg,
                                                 BinaryData::FileOpenIcon_svgSize));
    openKbmButton.setIcon (createForegroundIcon (BinaryData::FileOpenIcon_svg,
                                                 BinaryData::FileOpenIcon_svgSize));
    randomButton.setIcon (createForegroundIcon (BinaryData::CasinoIcon_svg,
                                                BinaryData::CasinoIcon_svgSize));
    openEditorButton.setIcon (createForegroundIcon (BinaryData::TuneIcon_svg,
                                                    BinaryData::TuneIcon_svgSize));
    resetButton.setIcon (createForegroundIcon (BinaryData::DeleteIcon_svg,
                                               BinaryData::DeleteIcon_svgSize));
}

ScaleBox::~ScaleBox()
{
}

void ScaleBox::resized()
{
    constexpr int rowHeight = 28;
    constexpr int rowGap = 3;
    constexpr int rowCount = 5;

    const int stackHeight = rowCount * rowHeight + (rowCount - 1) * rowGap;
    int y = (getHeight() - stackHeight) / 2;
    const int rowWidth = getWidth();

    auto placeRow = [&] (IconButton& button)
    {
        button.setBounds (0, y, rowWidth, rowHeight);
        y += rowHeight + rowGap;
    };

    placeRow (openSclButton);
    placeRow (openKbmButton);
    placeRow (randomButton);
    placeRow (openEditorButton);
    placeRow (resetButton);
}
