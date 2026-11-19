#include "MultiDimensionalPad.h"
#include "Theme.h"

MultiDimensionalPad::MultiDimensionalPad()
{
    addAndMakeVisible(xSlider);
    xSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    xSlider.setRange(-1.0f, 1.0f, 0.0001f);
    xSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    xSlider.addListener(this);
    xSlider.setEnabled(false);
    xSlider.setVisible(false);
    
    addAndMakeVisible(ySlider);
    ySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ySlider.setRange(-1.0f, 1.0f, 0.0001f);
    ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    ySlider.addListener(this);
    ySlider.setEnabled(false);
    ySlider.setVisible(false);
    
    thumbRadius = 10.0f;
    thumbDragged = NO_THUMB_SELECTED;
}

MultiDimensionalPad::~MultiDimensionalPad()
{
}

void MultiDimensionalPad::mouseDown(const MouseEvent& event)
{
    if ( (event.position.getX() > xyPad.getX()) && (event.position.getX() < xyPad.getRight()) && (event.position.getY() > xyPad.getY()) && (event.position.getY() < xyPad.getBottom()) )
    {
        auto newPos = event.position;
            
        newPos.x = juce::jlimit<float>(xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius, newPos.x);
        newPos.y = juce::jlimit<float>(xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, newPos.y);
        
        xSlider.setValue(juce::jmap<float>(newPos.x, xyPad.getX() + thumbRadius, xyPad.getRight()  - thumbRadius, xSlider.getMinimum(), xSlider.getMaximum()));
        ySlider.setValue(juce::jmap<float>(newPos.y, xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, ySlider.getMaximum(), ySlider.getMinimum()));

        repaint();
        
        thumbDragged = XY_PAD_THUMB;
    }
    else
    {
        thumbDragged = NO_THUMB_SELECTED;
    }
}

void MultiDimensionalPad::mouseDrag(const MouseEvent& event)
{
    if (thumbDragged == XY_PAD_THUMB)
    {
        auto newPos = event.position;

        newPos.x = juce::jlimit<float>(xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius, newPos.x);
        newPos.y = juce::jlimit<float>(xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, newPos.y);

        xSlider.setValue(juce::jmap<float>(newPos.x, xyPad.getX() + thumbRadius, xyPad.getRight()  - thumbRadius, xSlider.getMinimum(), xSlider.getMaximum()));
        ySlider.setValue(juce::jmap<float>(newPos.y, xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, ySlider.getMaximum(), ySlider.getMinimum()));

        repaint();
    }
}

void MultiDimensionalPad::paint(Graphics& g)
{
    // Fill XY Pad area
    g.setColour(ScaleSpaceColours::controlFill);
    g.fillRoundedRectangle(xyPad, 10);
    
    // Draw XY pad grid
    for (int i = 0; i < 9; i++)
    {
        if (i == 4)
        {
            g.setColour(ScaleSpaceColours::accent.withAlpha(0.53f));
        }
        else
        {
            g.setColour(ScaleSpaceColours::accent.withAlpha(0.16f));
        }

        g.drawLine(xyPad.getX() + thumbRadius, xyPad.getY() + thumbRadius + i * ( (xyPad.getHeight() - (thumbRadius * 2) ) / 8), xyPad.getRight() - thumbRadius, xyPad.getY() + thumbRadius + i * ( (xyPad.getHeight() - (thumbRadius * 2) ) / 8), 2);
        g.drawLine(xyPad.getX() + thumbRadius + i * ( (xyPad.getWidth() - (thumbRadius * 2) ) / 8), xyPad.getY() + thumbRadius, xyPad.getX() + thumbRadius + i * ( (xyPad.getWidth() - (thumbRadius * 2) ) / 8), xyPad.getBottom() - thumbRadius, 2);
    }
    // Draw XY Pad
    g.setColour(ScaleSpaceColours::accent);
    g.drawRoundedRectangle(xyPad, 10, 2);

    // Draw XY pad thumb
    thumbPos = juce::Point<float>(  juce::jmap<float>(xSlider.getValue(), xSlider.getMinimum(), xSlider.getMaximum(), xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius),
                                    juce::jmap<float>(ySlider.getValue(), ySlider.getMaximum(), ySlider.getMinimum(), xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius) );
    g.setColour(ScaleSpaceColours::accent);
    g.fillEllipse(thumbPos.x - thumbRadius, thumbPos.y - thumbRadius, thumbRadius * 2, thumbRadius * 2);
}

void MultiDimensionalPad::sliderValueChanged(Slider* slider)
{
    if (slider == &xSlider)
    {
        thumbPos.x = juce::jmap<float>(xSlider.getValue(), xSlider.getMinimum(), xSlider.getMaximum(), xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius);
    }
    else if (slider == &ySlider)
    {
        thumbPos.y = juce::jmap<float>(ySlider.getValue(), ySlider.getMaximum(), ySlider.getMinimum(), xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius);
    }

    repaint();
}

void MultiDimensionalPad::resized()
{
    // The outline is centred on the pad edge, so inset by half the stroke
    // width to keep the visible edge flush with the component bounds. This
    // makes the layout gaps around the pad match their configured values.
    constexpr float strokeWidth = 2.0f;
    xyPad = getLocalBounds().toFloat().reduced(strokeWidth * 0.5f);
}
