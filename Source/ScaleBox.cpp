#include "ScaleBox.h"
#include "Theme.h"

void IconTextButton::setIcon (std::unique_ptr<juce::Drawable> newIcon)
{
    icon = std::move (newIcon);
    repaint();
}

void IconTextButton::paintButton (juce::Graphics& g,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown)
{
    auto& lf = getLookAndFeel();

    lf.drawButtonBackground (g, *this,
                             findColour (juce::TextButton::buttonColourId),
                             shouldDrawButtonAsHighlighted,
                             shouldDrawButtonAsDown);

    const auto font = lf.getTextButtonFont (*this, getHeight());
    const auto text = getButtonText();

    constexpr float iconSize = 16.0f;
    constexpr float iconGap = 5.0f;

    const float textWidth = juce::GlyphArrangement::getStringWidth (font, text);
    const float contentWidth = (icon != nullptr ? iconSize + iconGap : 0.0f) + textWidth;
    const float startX = ((float) getWidth() - contentWidth) * 0.5f;
    const float centreY = (float) getHeight() * 0.5f;
    const float alpha = isEnabled() ? 1.0f : 0.5f;

    if (icon != nullptr)
    {
        icon->setTransformToFit ({ startX, centreY - iconSize * 0.5f, iconSize, iconSize },
                                 juce::RectanglePlacement::centred);
        icon->draw (g, alpha);
    }

    g.setColour (findColour (getToggleState() ? juce::TextButton::textColourOnId
                                              : juce::TextButton::textColourOffId)
                     .withMultipliedAlpha (alpha));
    g.setFont (font);
    g.drawText (text,
                juce::Rectangle<float> (startX + (icon != nullptr ? iconSize + iconGap : 0.0f),
                                        0.0f, textWidth + 2.0f, (float) getHeight()),
                juce::Justification::centredLeft, false);
}

ScaleBox::ScaleBox()
{
    addAndMakeVisible(openSclButton);
    addAndMakeVisible(openKbmButton);
    addAndMakeVisible(randomButton);
    addAndMakeVisible(openEditorButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(sclNameLabel);
    addAndMakeVisible(kbmNameLabel);
    openSclButton.setColour (juce::TextButton::buttonColourId, ScaleSpaceColours::controlFill);
    openSclButton.setColour (juce::TextButton::textColourOffId, ScaleSpaceColours::foreground);
    openSclButton.setTooltip ("Load SCL");
    openKbmButton.setColour (juce::TextButton::buttonColourId, ScaleSpaceColours::controlFill);
    openKbmButton.setColour (juce::TextButton::textColourOffId, ScaleSpaceColours::foreground);
    openKbmButton.setTooltip ("Load KBM");

    auto createFileOpenIcon = []
    {
        auto icon = juce::Drawable::createFromImageData (BinaryData::FileOpenIcon_svg,
                                                        BinaryData::FileOpenIcon_svgSize);
        icon->replaceColour (juce::Colours::black, ScaleSpaceColours::foreground);
        return icon;
    };

    openSclButton.setIcon (createFileOpenIcon());
    openKbmButton.setIcon (createFileOpenIcon());

    auto casinoIcon = juce::Drawable::createFromImageData (BinaryData::CasinoIcon_svg,
                                                          BinaryData::CasinoIcon_svgSize);
    casinoIcon->replaceColour (juce::Colours::black, ScaleSpaceColours::foreground);
    randomButton.setImages (casinoIcon.get());
    randomButton.setTooltip ("Random SCL");

    auto deleteIcon = juce::Drawable::createFromImageData (BinaryData::DeleteIcon_svg,
                                                          BinaryData::DeleteIcon_svgSize);
    deleteIcon->replaceColour (juce::Colours::black, ScaleSpaceColours::foreground);
    resetButton.setImages (deleteIcon.get());
    resetButton.setTooltip ("Reset");

    auto tuneIcon = juce::Drawable::createFromImageData (BinaryData::TuneIcon_svg,
                                                        BinaryData::TuneIcon_svgSize);
    tuneIcon->replaceColour (juce::Colours::black, ScaleSpaceColours::foreground);
    openEditorButton.setImages (tuneIcon.get());
    openEditorButton.setTooltip ("Editor");
    sclNameLabel.setFont (juce::FontOptions().withHeight(16).withStyle ("Bold"));
    sclNameLabel.setColour (juce::Label::textColourId, ScaleSpaceColours::foreground);
    sclNameLabel.setMinimumHorizontalScale(1.0f);
    kbmNameLabel.setFont (juce::FontOptions().withHeight(16));
    kbmNameLabel.setColour (juce::Label::textColourId, ScaleSpaceColours::foreground);
    kbmNameLabel.setMinimumHorizontalScale(1.0f);
}

ScaleBox::~ScaleBox()
{
}

void ScaleBox::setRightAlignLabels (bool shouldRightAlign)
{
    if (rightAlignLabels == shouldRightAlign)
        return;

    rightAlignLabels = shouldRightAlign;
    resized();
}

void ScaleBox::setButtonsAtTop (bool shouldBeAtTop)
{
    if (buttonsAtTop == shouldBeAtTop)
        return;

    buttonsAtTop = shouldBeAtTop;
    resized();
}

void ScaleBox::paint(Graphics& g)
{
    g.setColour(ScaleSpaceColours::accent);
    g.drawRect(getLocalBounds());
}

void ScaleBox::resized()
{
    constexpr int margin = 10;
    constexpr int buttonGap = 6;
    constexpr int buttonHeight = 30;
    constexpr int iconButtonWidth = 30;
    constexpr int labelHeight = 18;

    // Scale and mapping names sit nearest the outer edge: the top for the
    // upper boxes, or the bottom for the lower boxes.
    const auto labelJustification = rightAlignLabels ? juce::Justification::centredRight
                                                     : juce::Justification::centredLeft;
    sclNameLabel.setJustificationType (labelJustification);
    kbmNameLabel.setJustificationType (labelJustification);

    int scaleLabelY = 8;
    int mappingLabelY = 8 + labelHeight;

    if (buttonsAtTop)
    {
        mappingLabelY = getHeight() - 8 - labelHeight;
        scaleLabelY = mappingLabelY - labelHeight;
    }

    sclNameLabel.setBounds (margin, scaleLabelY, getWidth() - margin * 2, labelHeight);
    kbmNameLabel.setBounds (margin, mappingLabelY, getWidth() - margin * 2, labelHeight);

    // Load buttons plus the Random, Tune and Delete icons in one row.
    const int rowY = buttonsAtTop ? 8 : getHeight() - 8 - buttonHeight;
    const int textButtonWidth = (getWidth() - margin * 2 - buttonGap * 4
                                 - iconButtonWidth * 3) / 2;

    int x = margin;
    openSclButton.setBounds (x, rowY, textButtonWidth, buttonHeight);
    x += textButtonWidth + buttonGap;
    openKbmButton.setBounds (x, rowY, textButtonWidth, buttonHeight);
    x += textButtonWidth + buttonGap;
    randomButton.setBounds (x, rowY, iconButtonWidth, buttonHeight);
    x += iconButtonWidth + buttonGap;
    openEditorButton.setBounds (x, rowY, iconButtonWidth, buttonHeight);
    x += iconButtonWidth + buttonGap;
    resetButton.setBounds (x, rowY, iconButtonWidth, buttonHeight);
}
