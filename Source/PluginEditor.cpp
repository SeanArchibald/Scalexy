/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Theme.h"

//==============================================================================
static juce::PropertiesFile::Options createSettingsOptions()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "Scale-Space";
    options.folderName = "Scale-Space";
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = juce::PropertiesFile::storeAsXML;
    
    return options;
}

ScaleSpaceAudioProcessorEditor::ScaleSpaceAudioProcessorEditor (ScaleSpaceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), settingsFile (createSettingsOptions())
{
    int restoredWidth = designWidth;
    int restoredHeight = designHeight;
    
    restoredWidth  = settingsFile.getIntValue ("editorWidth", designWidth);
    restoredHeight = settingsFile.getIntValue ("editorHeight", designHeight);
    
    restoredWidth  = juce::jlimit (minWidth,  maxWidth,  restoredWidth);
    restoredHeight = juce::jlimit (minHeight, maxHeight, restoredHeight);
    
    // Restore the window size the user last chose (or use the design size).
    setSize (restoredWidth, restoredHeight);
    
    // The whole UI is placed inside contentComponent, which is always laid out
    // at the design size and then scaled to fit the editor window.
    addAndMakeVisible (contentComponent);
    
    // Slider attachments
    xAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "x", multiPad.xSlider);
    yAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "y", multiPad.ySlider);
    smoothAmtSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "smoothAmount", smoothAmtSlider);
    smoothToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.getApvts(), "smooth", smoothToggle);
    
    contentComponent.addAndMakeVisible(&multiPad);

    const auto mainUITextColor = ScaleSpaceColours::foreground;
    const auto controlFillColor = ScaleSpaceColours::controlFill;
    
    contentComponent.addAndMakeVisible (&pluginTitleLabel);
    pluginTitleLabel.setFont (juce::FontOptions(brunoAceSC).withHeight(42));
    pluginTitleLabel.setText ("ScaleSpace", juce::dontSendNotification);
    pluginTitleLabel.setColour (juce::Label::textColourId, mainUITextColor);
    pluginTitleLabel.setMinimumHorizontalScale(1.0f);
    
    // Scale Box 1
    contentComponent.addAndMakeVisible (&scaleBox1);
    scaleBox1.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox1.scaleLabel.setText ("SCALE 1", juce::dontSendNotification);
    scaleBox1.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_1, SCL, "Open SCL File for Scale 1", "*.scl"); };
    scaleBox1.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_1, KBM, "Open KBM File for Scale 1", "*.kbm"); };
    scaleBox1.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_1); };

    // Scale Box 2
    contentComponent.addAndMakeVisible (&scaleBox2);
    scaleBox2.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox2.scaleLabel.setText ("SCALE 2", juce::dontSendNotification);
    scaleBox2.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_2, SCL, "Open SCL File for Scale 2", "*.scl"); };
    scaleBox2.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_2, KBM, "Open KBM File for Scale 2", "*.kbm"); };
    scaleBox2.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_2); };
    
    // Scale Box 3
    contentComponent.addAndMakeVisible (&scaleBox3);
    scaleBox3.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox3.scaleLabel.setText ("SCALE 3", juce::dontSendNotification);
    scaleBox3.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_3, SCL, "Open SCL File for Scale 3", "*.scl"); };
    scaleBox3.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_3, KBM, "Open KBM File for Scale 3", "*.kbm"); };
    scaleBox3.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_3); };
    
    // Scale Box 4
    contentComponent.addAndMakeVisible (&scaleBox4);
    scaleBox4.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox4.scaleLabel.setText ("SCALE 4", juce::dontSendNotification);
    scaleBox4.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_4, SCL, "Open SCL File for Scale 4", "*.scl"); };
    scaleBox4.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_4, KBM, "Open KBM File for Scale 4", "*.kbm"); };
    scaleBox4.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_4); };
    
    // Export buttons
    contentComponent.addAndMakeVisible(&exportSclButton);
    exportSclButton.setColour (juce::TextButton::buttonColourId, controlFillColor);
    exportSclButton.setColour (juce::TextButton::textColourOffId, mainUITextColor);
    exportSclButton.setButtonText ("Export SCL");
    exportSclButton.onClick = [this] { exportButtonClicked(SCL, "Save SCL File for current tuning", "*.scl"); };
    
    contentComponent.addAndMakeVisible(&exportKbmButton);
    exportKbmButton.setColour (juce::TextButton::buttonColourId, controlFillColor);
    exportKbmButton.setColour (juce::TextButton::textColourOffId, mainUITextColor);
    exportKbmButton.setButtonText ("Export KBM");
    exportKbmButton.onClick = [this] { exportButtonClicked(KBM, "Save KBM File for current tuning", "*.kbm"); };
    
    contentComponent.addAndMakeVisible(&smoothToggle);
    smoothToggle.setColour (juce::ToggleButton::textColourId, mainUITextColor);
    smoothToggle.setColour (juce::ToggleButton::tickColourId, mainUITextColor);
    smoothToggle.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
    smoothToggle.setButtonText ("Smooth");
    
    contentComponent.addAndMakeVisible (&smoothAmtSlider);
    smoothAmtSlider.setColour(juce::Slider::thumbColourId , ScaleSpaceColours::accent);
    smoothAmtSlider.setColour(juce::Slider::trackColourId, controlFillColor);
    smoothAmtSlider.setColour(juce::Slider::backgroundColourId, controlFillColor);
    smoothAmtSlider.setColour(juce::Slider::textBoxTextColourId, mainUITextColor);
    smoothAmtSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    smoothAmtSlider.setColour(juce::Slider::textBoxOutlineColourId, mainUITextColor);
    smoothAmtSlider.setRange (1.0f, 10.0f);
    
    // Setting these colours doesn't work yet
    //tooltipWindow->setColour(juce::TooltipWindow::backgroundColourId, juce::Colours::white);
    //tooltipWindow->setColour(juce::TooltipWindow::textColourId, juce::Colours::black);
    //tooltipWindow->setColour(juce::TooltipWindow::outlineColourId, juce::Colours::black);
    
    // Set SCL & KBM Name label texts
    updateAllNameLabels();
    
    tuningEditor = std::make_unique<TuningEditor>(audioProcessor, TUNING_1);
    
    // Allow the host and the user to resize the editor. The bottom-right corner
    // resizer is added by JUCE, and the fixed aspect ratio keeps the UI proportional.
    setResizeLimits (minWidth, minHeight, maxWidth, maxHeight);
    if (auto* constrainer = getConstrainer())
        constrainer->setFixedAspectRatio ((double) designWidth / (double) designHeight);
    setResizable (true, true);
    
    initialUIUpdate = true;
    startTimerHz(10);
}

ScaleSpaceAudioProcessorEditor::~ScaleSpaceAudioProcessorEditor()
{
    editorWindow.deleteAndZero();
}

//==============================================================================
void ScaleSpaceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(ScaleSpaceColours::background);
    g.fillAll();
}

void ScaleSpaceAudioProcessorEditor::resized()
{
    // Scale the fixed-size interface to fill the editor while preserving its aspect ratio.
    const float scale = juce::jmin ((float) getWidth()  / (float) designWidth,
                                    (float) getHeight() / (float) designHeight);
    
    const int scaledWidth  = juce::roundToInt (designWidth  * scale);
    const int scaledHeight = juce::roundToInt (designHeight * scale);
    
    contentComponent.setTransform (juce::AffineTransform::scale (scale));
    contentComponent.setBounds ((getWidth()  - scaledWidth)  / 2,
                                (getHeight() - scaledHeight) / 2,
                                designWidth, designHeight);
    
    layoutContent();
    
    // PropertiesFile saves itself automatically shortly after a change.
    settingsFile.setValue ("editorWidth",  getWidth());
    settingsFile.setValue ("editorHeight", getHeight());
}

void ScaleSpaceAudioProcessorEditor::layoutContent()
{
    pluginTitleLabel.setBounds (20, 20, designWidth - 20, 30);
    
    uint32_t scaleBoxOriginX = 65;
    uint32_t scaleBoxOriginY = 80;
    uint32_t scaleBoxXOffset = 880;
    uint32_t scaleBoxYOffset = 310;
    scaleBox1.setBounds (scaleBoxOriginX, scaleBoxOriginY, 300, 170);
    scaleBox2.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY, 300, 170);
    scaleBox3.setBounds (scaleBoxOriginX, scaleBoxOriginY + scaleBoxYOffset, 300, 170);
    scaleBox4.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY + scaleBoxYOffset, 300, 170);
    
    smoothToggle.setBounds (designWidth * 0.390f, 590, 80, 30);
    smoothAmtSlider.setBounds (designWidth * 0.442, 590, 215, 30);
    
    exportSclButton.setBounds (designWidth * 0.415f, 650, 100, 30);
    exportKbmButton.setBounds (designWidth * 0.505f, 650, 100, 30);
    
    multiPadSize = designHeight / 1.55f;
    
    float xOrigin = (designWidth - multiPadSize) / 2.0f;
    float yOrigin = 100.0f;
    multiPad.setBounds(xOrigin, yOrigin, multiPadSize, multiPadSize);
  
}

void ScaleSpaceAudioProcessorEditor::scaleOpenButtonClicked(const uint32_t tuningNumber, const uint32_t tunType, const char* chooserText, const char* fileExt)
{
	if (tuningNumber >= TUNING_COUNT)
		return;
	
	if (tunType >= FILETYPE_COUNT)
		return;
	
	chooser = std::make_unique<juce::FileChooser> (chooserText,
		juce::File(audioProcessor.getStringState(kStateLastOpenedLoadPath)),
		fileExt);
	auto chooserFlags = juce::FileBrowserComponent::openMode
						| juce::FileBrowserComponent::canSelectFiles;
	chooser->launchAsync (chooserFlags, [this, tuningNumber, tunType, fileExt] (const juce::FileChooser& fc)
		{
			auto file = fc.getResult();
			// This is true if a file was chosen, false if the dialog was cancelled
			if (file != juce::File {})
			{
				if (file.getFileExtension() == juce::String(fileExt).trimCharactersAtStart("*"))
				{
					if (!file.existsAsFile())
						return;
					auto fileText = file.loadFileAsString();
					
					// Should be true if the new scale was applied successfully
					if (applyNewTuningFromEditor(tuningNumber, fileText, tunType))
					{
						// Update Scale Name stringState
						audioProcessor.setNameStateForTuning(tuningNumber, file.getFileName(), tunType);
						audioProcessor.setStringState( kStateLastOpenedLoadPath, file.getParentDirectory().getFullPathName() );
					}
				}
				else
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Couldn't apply file", "Wrong file type!");
				}
				// Update UI.
				updateAllNameLabels();
				repaint();
			}
		});
}

void ScaleSpaceAudioProcessorEditor::exportButtonClicked(const uint32_t tunType, const char* chooserText, const char* fileExt)
{
    if (!editorWindow)
    {
        chooser = std::make_unique<juce::FileChooser> (chooserText,
            juce::File(audioProcessor.getStringState(kStateFileSavePath)),
            fileExt);
        auto chooserFlags = juce::FileBrowserComponent::saveMode
                            | juce::FileBrowserComponent::canSelectFiles
                            | juce::FileBrowserComponent::warnAboutOverwriting;
        chooser->launchAsync (chooserFlags, [this, tunType] (const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                // This is true if a file was chosen, false if the dialog was cancelled
                if (file != juce::File {})
                {
                    if (tunType == SCL)
                        audioProcessor.exportCurrentSclTuning(file.getFullPathName());
                    else if (tunType == KBM)
                        audioProcessor.exportCurrentKbmTuning(file.getFullPathName());
                }
            });
    }
    else
    {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Editor window is open", "Please close the Tuning Editor window before exporting!");
	}
}

void ScaleSpaceAudioProcessorEditor::updateAllNameLabels()
{
    updateNameLabel (scaleBox1.sclNameLabel, kStateNameSCL1);
    updateNameLabel (scaleBox1.kbmNameLabel, kStateNameKBM1);
    updateNameLabel (scaleBox2.sclNameLabel, kStateNameSCL2);
    updateNameLabel (scaleBox2.kbmNameLabel, kStateNameKBM2);
    updateNameLabel (scaleBox3.sclNameLabel, kStateNameSCL3);
    updateNameLabel (scaleBox3.kbmNameLabel, kStateNameKBM3);
    updateNameLabel (scaleBox4.sclNameLabel, kStateNameSCL4);
    updateNameLabel (scaleBox4.kbmNameLabel, kStateNameKBM4);
}

void ScaleSpaceAudioProcessorEditor::updateNameLabel(juce::Label &label, const uint32_t nameIndex)
{
    if (label.getFont().getHeight() <= 0.0f)
        return;
    
    label.setText (audioProcessor.getStringState(nameIndex), juce::dontSendNotification);
    // Rough approximation of whether to set a tooltip, i.e. try to estimate if the name fits in the scale box
    if ((audioProcessor.getStringState(nameIndex).length() / 2.2f) > (label.getWidth() / label.getFont().getHeight()))
        label.setTooltip (audioProcessor.getStringState(nameIndex));
    else
        label.setTooltip ("");
}

bool ScaleSpaceAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray &files)
{
    if (files.size() !=1)
        return false;
    
    for (auto i = files.begin(); i != files.end(); ++i)
    {
        if (i->endsWith(".scl") || i->endsWith(".kbm"))
        {
            return true;
        }
    }
    return false;
}

void ScaleSpaceAudioProcessorEditor::filesDropped (const juce::StringArray &files, int x, int y)
{
    if (files.size() !=1)
        return;
    
    // Convert from editor coordinates into the scaled content component's
    // coordinate space so the scale box hit-testing below still works.
    const auto contentPoint = contentComponent.getLocalPoint (this, juce::Point<int> (x, y));
    x = contentPoint.x;
    y = contentPoint.y;
        
    for (auto i = files.begin(); i != files.end(); ++i)
    {
        if (i->endsWith(".scl") || i->endsWith(".kbm"))
        {
            uint32_t scaleToApply = TUNING_COUNT;
            
            if ( (x > scaleBox1.getX()) && (x < scaleBox1.getRight()) && (y > scaleBox1.getY()) && (y < scaleBox1.getBottom()) )
                scaleToApply = TUNING_1;
            else if ( (x > scaleBox2.getX()) && (x < scaleBox2.getRight()) && (y > scaleBox2.getY()) && (y < scaleBox2.getBottom()) )
                scaleToApply = TUNING_2;
            else if ( (x > scaleBox3.getX()) && (x < scaleBox3.getRight()) && (y > scaleBox3.getY()) && (y < scaleBox3.getBottom()) )
                scaleToApply = TUNING_3;
            else if ( (x > scaleBox4.getX()) && (x < scaleBox4.getRight()) && (y > scaleBox4.getY()) && (y < scaleBox4.getBottom()) )
                scaleToApply = TUNING_4;
                
            if (scaleToApply != TUNING_COUNT)
            {
                if (audioProcessor.applyDroppedFile(scaleToApply, i->toStdString()))
                {
                    if (tuningEditor && audioProcessor.currentEditedTuning == scaleToApply)
                        tuningEditor->setupForTuning(scaleToApply);
                }
            }
        }
    }
    // Update UI.
    updateAllNameLabels();
    repaint();
}

// applyNewTuningFromEditor is for files opened from, or dropped onto, the main editor window.
bool ScaleSpaceAudioProcessorEditor::applyNewTuningFromEditor(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType)
{
    bool success = false;
    
    if (audioProcessor.editedTuningValid(tuningNumber, tunData, tunType))
    {
        // The tuning validity has already been checked but the following try - catches
        // have kept kept just in case...
        if (tunType == SCL)
        {
            try
            {
                auto k = audioProcessor.getTuning(tuningNumber).keyboardMapping;
                auto s = Tunings::parseSCLData(tunData.toStdString());
                audioProcessor.getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
                
                audioProcessor.setStringState( audioProcessor.getFileIndexFromTuningAndFileType(tuningNumber, tunType), tunData);
                
                if ( (tuningEditor) && (editorWindow) && (audioProcessor.currentEditedTuning == tuningNumber) )
                    tuningEditor->setupForTuning(tuningNumber);
                
                success = true;
            }
            catch (const std::exception& e)
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying SCL. Scale reset.", e.what());
                // Reset stringState information for this tuning's SCL and KBM
                audioProcessor.resetTuningStringStatesToStandard(tuningNumber);
                success = false;
            }
        }
        else if (tunType == KBM)
        {
            try
            {
                auto s = audioProcessor.getTuning(tuningNumber).scale;
                auto k = Tunings::parseKBMData(tunData.toStdString());
                audioProcessor.getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
                
                audioProcessor.setStringState( audioProcessor.getFileIndexFromTuningAndFileType(tuningNumber, tunType), tunData);
                
                if ( (tuningEditor) && (editorWindow) && (audioProcessor.currentEditedTuning == tuningNumber) )
                    tuningEditor->setupForTuning(tuningNumber);
                    
                success = true;
            }
            catch (const std::exception& e)
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying KBM. Scale reset.", e.what());
                // Reset stringState information for this tuning's SCL and KBM
                audioProcessor.resetTuningStringStatesToStandard(tuningNumber);
                success = false;
            }
        }
    }
    return success;
}

void ScaleSpaceAudioProcessorEditor::openEditorButtonClicked(const uint32_t tuningNumber)
{
    if (!editorWindow)
    {
        openTuningEditor(tuningNumber, getApproximateScaleFactorForComponent(this));
        
        // TODO Don't use this timer to update the UI while the tuning editor is open
        startTimerHz(10);
    }
    else if ( audioProcessor.currentEditedTuning == tuningNumber )
    {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Editor window is already open", "The Tuning Editor window is already open for this scale.");
	}
	else
    {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Editor window is already open", "The Tuning Editor window is already open for another scale. Please close the current tuning editor window and try again!");
	}
}

void ScaleSpaceAudioProcessorEditor::openTuningEditor(const uint32_t tuningNumber, const float desktopScaleFactor)
{
    if (!editorWindow && tuningEditor)
    {
        audioProcessor.currentEditedTuning = tuningNumber;
        tuningEditor->setupForTuning(tuningNumber);
        tuningEditor->setDesktopScaleFactor(desktopScaleFactor);
        tuningEditor->startTimerHz(50);
        editorWindow = new ExternalTuningEditorWindow("Edit Tuning for Scale " + juce::String(tuningNumber + 1), Colours::grey, DocumentWindow::closeButton);
        editorWindow->setUsingNativeTitleBar(false);
        editorWindow->setResizable(false, true);
        editorWindow->setContentNonOwned(tuningEditor.get(), true);
        editorWindow->centreWithSize(1000 * desktopScaleFactor, 600 * desktopScaleFactor);
        editorWindow->setVisible(true);
    }
}

// TODO Don't use this timer to update the UI while the tuning editor is open
void ScaleSpaceAudioProcessorEditor::timerCallback()
{
    if (initialUIUpdate)
    {
        updateAllNameLabels();
        repaint();
        initialUIUpdate = false;
        stopTimer();
    }
    else if (editorWindow)
    {
        if (audioProcessor.paintFlag)
        {
            updateAllNameLabels();
            repaint();
            audioProcessor.paintFlag = false;
        }
    }
    else
    {
        tuningEditor->stopTimer();
        stopTimer();
    }
}
