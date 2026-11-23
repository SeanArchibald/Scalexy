/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Theme.h"

#include <cmath>

//==============================================================================
static juce::PropertiesFile::Options createSettingsOptions()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "Scalexy";
    options.folderName = "Scalexy";
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = juce::PropertiesFile::storeAsXML;
    
    return options;
}

//==============================================================================
void OutlinedTextButton::paintButton (juce::Graphics& g,
                                      bool shouldDrawButtonAsHighlighted,
                                      bool shouldDrawButtonAsDown)
{
    constexpr float cornerRadius = 5.0f;
    const auto bounds = getLocalBounds().toFloat();
    g.setColour (ScaleSpaceColours::background);
    g.fillRoundedRectangle (bounds, cornerRadius);

    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
    {
        g.setColour (ScaleSpaceColours::accent.withAlpha (shouldDrawButtonAsDown ? 0.18f : 0.08f));
        g.fillRoundedRectangle (bounds, cornerRadius);
    }

    g.setColour (ScaleSpaceColours::buttonOutline);
    g.drawRoundedRectangle (bounds.reduced (0.5f), cornerRadius, 1.0f);

    const auto font = getLookAndFeel().getTextButtonFont (*this, getHeight());
    g.setColour (ScaleSpaceColours::foreground);
    g.setFont (font);
    g.drawText (getButtonText(), getLocalBounds(), juce::Justification::centred, false);
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
    
    // If the stored size has a different aspect ratio it was saved by an older
    // layout, so fall back to the current design size instead of letterboxing.
    const double designAspect   = (double) designWidth  / (double) designHeight;
    const double restoredAspect = (double) restoredWidth / (double) restoredHeight;
    if (std::abs (restoredAspect - designAspect) > 0.01)
    {
        restoredWidth  = designWidth;
        restoredHeight = designHeight;
    }
    
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
    
    // Scale Box 1
    contentComponent.addAndMakeVisible (&scaleBox1);
    scaleBox1.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_1, SCL, "Open SCL File for Scale 1", "*.scl"); };
    scaleBox1.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_1, KBM, "Open KBM File for Scale 1", "*.kbm"); };
    scaleBox1.randomButton.onClick = [this] { randomiseSclButtonClicked(TUNING_1); };
    scaleBox1.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_1); };
    scaleBox1.resetButton.onClick = [this] { resetButtonClicked(TUNING_1); };

    // Scale Box 2
    contentComponent.addAndMakeVisible (&scaleBox2);
    scaleBox2.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_2, SCL, "Open SCL File for Scale 2", "*.scl"); };
    scaleBox2.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_2, KBM, "Open KBM File for Scale 2", "*.kbm"); };
    scaleBox2.randomButton.onClick = [this] { randomiseSclButtonClicked(TUNING_2); };
    scaleBox2.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_2); };
    scaleBox2.resetButton.onClick = [this] { resetButtonClicked(TUNING_2); };
    
    // Scale Box 3
    contentComponent.addAndMakeVisible (&scaleBox3);
    scaleBox3.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_3, SCL, "Open SCL File for Scale 3", "*.scl"); };
    scaleBox3.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_3, KBM, "Open KBM File for Scale 3", "*.kbm"); };
    scaleBox3.randomButton.onClick = [this] { randomiseSclButtonClicked(TUNING_3); };
    scaleBox3.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_3); };
    scaleBox3.resetButton.onClick = [this] { resetButtonClicked(TUNING_3); };
    
    // Scale Box 4
    contentComponent.addAndMakeVisible (&scaleBox4);
    scaleBox4.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_4, SCL, "Open SCL File for Scale 4", "*.scl"); };
    scaleBox4.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_4, KBM, "Open KBM File for Scale 4", "*.kbm"); };
    scaleBox4.randomButton.onClick = [this] { randomiseSclButtonClicked(TUNING_4); };
    scaleBox4.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_4); };
    scaleBox4.resetButton.onClick = [this] { resetButtonClicked(TUNING_4); };
    
    // Settings button (bottom-left)
    contentComponent.addAndMakeVisible(&settingsButton);
    auto settingsIcon = juce::Drawable::createFromImageData (BinaryData::SettingsIcon_svg,
                                                            BinaryData::SettingsIcon_svgSize);
    settingsIcon->replaceColour (juce::Colours::black, ScaleSpaceColours::foreground);
    settingsButton.setIcon (std::move (settingsIcon));
    settingsButton.onClick = [this] { settingsButtonClicked(); };
    
    // Export buttons
    contentComponent.addAndMakeVisible(&exportSclButton);
    exportSclButton.onClick = [this] { exportButtonClicked(SCL, "Save SCL File for current tuning", "*.scl"); };
    
    contentComponent.addAndMakeVisible(&exportKbmButton);
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
    constexpr int gap = ScaleSpaceLayout::gap;
    constexpr int padX = gap + scaleBoxWidth + gap;
    constexpr int rightColumnX = padX + padSize + gap;
    
    // The XY pad sits between the left and right columns of scale boxes.
    multiPad.setBounds (padX, gap, padSize, padSize);
    
    // Scale 1 sits in the top-left corner and Scale 2 mirrors it on the
    // top-right. Each operation block is vertically centred on its adjacent
    // XY pad quadrant.
    const int topBoxY = gap + padSize / 4 - scaleBoxHeight / 2;
    scaleBox1.setBounds (gap, topBoxY, scaleBoxWidth, scaleBoxHeight);
    scaleBox2.setBounds (rightColumnX, topBoxY, scaleBoxWidth, scaleBoxHeight);
    
    // Scale 3 and Scale 4 mirror each other on the bottom row.
    const int bottomBoxY = gap + (padSize * 3) / 4 - scaleBoxHeight / 2;
    scaleBox3.setBounds (gap, bottomBoxY, scaleBoxWidth, scaleBoxHeight);
    scaleBox4.setBounds (rightColumnX, bottomBoxY, scaleBoxWidth, scaleBoxHeight);
    
    // A single status-bar row below everything. The export buttons and smooth
    // controls are grouped together and centred in the window.
    const int statusBarY = multiPad.getBottom() + gap;
    
    constexpr int exportButtonWidth = 100;
    constexpr int smoothToggleWidth = 80;
    constexpr int smoothSliderWidth = 215;
    constexpr int statusGroupWidth = exportButtonWidth * 2 + smoothToggleWidth
                                     + smoothSliderWidth + gap * 3;
    int statusX = (designWidth - statusGroupWidth) / 2;
    
    exportSclButton.setBounds (statusX, statusBarY, exportButtonWidth, statusBarHeight);
    statusX += exportButtonWidth + gap;
    exportKbmButton.setBounds (statusX, statusBarY, exportButtonWidth, statusBarHeight);
    statusX += exportButtonWidth + gap;
    smoothToggle.setBounds (statusX, statusBarY, smoothToggleWidth, statusBarHeight);
    statusX += smoothToggleWidth + gap;
    smoothAmtSlider.setBounds (statusX, statusBarY, smoothSliderWidth, statusBarHeight);
    
    settingsButton.setBounds (gap, statusBarY, statusBarHeight, statusBarHeight);
}

void ScaleSpaceAudioProcessorEditor::scaleOpenButtonClicked(const uint32_t tuningNumber, const uint32_t tunType, const char* chooserText, const char* fileExt)
{
	if (tuningNumber >= TUNING_COUNT)
		return;
	
	if (tunType >= FILETYPE_COUNT)
		return;
	
	const juce::String dirSettingKey = (tunType == SCL) ? "sclDir" : "kbmDir";
	juce::String startDir = settingsFile.getValue (dirSettingKey);
	if (startDir.isEmpty())
		startDir = audioProcessor.getStringState (kStateLastOpenedLoadPath);
	
	chooser = std::make_unique<juce::FileChooser> (chooserText,
		juce::File(startDir),
		fileExt);
	auto chooserFlags = juce::FileBrowserComponent::openMode
						| juce::FileBrowserComponent::canSelectFiles;
	chooser->launchAsync (chooserFlags, [this, tuningNumber, tunType, fileExt, dirSettingKey] (const juce::FileChooser& fc)
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
						audioProcessor.setNameStateForTuning(tuningNumber, audioProcessor.getDisplayNameForTuning(fileText, file.getFileName(), tunType), tunType);
						audioProcessor.setStringState( kStateLastOpenedLoadPath, file.getParentDirectory().getFullPathName() );
						settingsFile.setValue (dirSettingKey, file.getParentDirectory().getFullPathName());
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

void ScaleSpaceAudioProcessorEditor::randomiseSclButtonClicked(const uint32_t tuningNumber)
{
    if (tuningNumber >= TUNING_COUNT)
        return;

    const juce::String dirSettingKey = "sclDir";

    if (!settingsFile.containsKey (dirSettingKey))
        return;

    const juce::File sclDir (settingsFile.getValue (dirSettingKey));
    if (!sclDir.isDirectory())
        return;

    const auto sclFiles = sclDir.findChildFiles (juce::File::findFiles, false, "*.scl");
    if (sclFiles.isEmpty())
        return;

    const auto file = sclFiles[juce::Random::getSystemRandom().nextInt (sclFiles.size())];
    const auto fileText = file.loadFileAsString();

    if (applyNewTuningFromEditor (tuningNumber, fileText, SCL))
    {
        audioProcessor.setNameStateForTuning (
            tuningNumber,
            audioProcessor.getDisplayNameForTuning (fileText, file.getFileName(), SCL),
            SCL);
        audioProcessor.setStringState (kStateLastOpenedLoadPath,
                                       file.getParentDirectory().getFullPathName());
    }

    updateAllNameLabels();
    repaint();
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
    updateNameLabel (multiPad.getScaleLabel (0), kStateNameSCL1);
    updateNameLabel (multiPad.getMappingLabel (0), kStateNameKBM1);
    updateNameLabel (multiPad.getScaleLabel (1), kStateNameSCL2);
    updateNameLabel (multiPad.getMappingLabel (1), kStateNameKBM2);
    updateNameLabel (multiPad.getScaleLabel (2), kStateNameSCL3);
    updateNameLabel (multiPad.getMappingLabel (2), kStateNameKBM3);
    updateNameLabel (multiPad.getScaleLabel (3), kStateNameSCL4);
    updateNameLabel (multiPad.getMappingLabel (3), kStateNameKBM4);
}

void ScaleSpaceAudioProcessorEditor::updateNameLabel(juce::Label &label, const uint32_t nameIndex)
{
    if (label.getFont().getHeight() <= 0.0f)
        return;
    
    label.setText (audioProcessor.getStringState(nameIndex), juce::dontSendNotification);
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
                    const auto droppedFile = juce::File(*i);
                    const auto dirSettingKey = droppedFile.getFileExtension() == ".scl" ? "sclDir" : "kbmDir";
                    settingsFile.setValue (dirSettingKey, droppedFile.getParentDirectory().getFullPathName());
                    
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

void ScaleSpaceAudioProcessorEditor::resetButtonClicked(const uint32_t tuningNumber)
{
    if (tuningNumber >= TUNING_COUNT)
        return;

    audioProcessor.resetTuningToStandard(tuningNumber);

    if ( (tuningEditor) && (editorWindow) && (audioProcessor.currentEditedTuning == tuningNumber) )
        tuningEditor->setupForTuning(tuningNumber);

    updateAllNameLabels();
    repaint();
}

void ScaleSpaceAudioProcessorEditor::settingsButtonClicked()
{
    settingsFile.saveIfNeeded();

    const auto settingsFileOnDisk = settingsFile.getFile();
    if (settingsFileOnDisk.existsAsFile())
        settingsFileOnDisk.startAsProcess();
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
