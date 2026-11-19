/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <bitset>
#include "Tunings.h"
#include "libMTSMaster.h"

enum States {
    kStateFileSCL1 = 0,
    kStateFileSCL2 = 1,
    kStateFileSCL3 = 2,
    kStateFileSCL4 = 3,
    kStateFileKBM1 = 4,
    kStateFileKBM2 = 5,
    kStateFileKBM3 = 6,
    kStateFileKBM4 = 7,
    kStateNameSCL1 = 8,
    kStateNameSCL2 = 9,
    kStateNameSCL3 = 10,
    kStateNameSCL4 = 11,
    kStateNameKBM1 = 12,
    kStateNameKBM2 = 13,
    kStateNameKBM3 = 14,
    kStateNameKBM4 = 15,
    kStateLastOpenedLoadPath = 16,
    kStateFileSavePath = 17,
    kStateEditorMode = 18,
    kStateEditorPolarMode = 19,
    kStateCount    = 20
};

enum ScaleTunings {
    TUNING_1 = 0,
    TUNING_2 = 1,
    TUNING_3 = 2,
    TUNING_4 = 3,
    TUNING_COUNT = 4
};

enum FileType {
    SCL = 0,
    KBM = 1,
    FILETYPE_COUNT = 2
};

//==============================================================================
/**
*/
class ScaleSpaceAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ScaleSpaceAudioProcessor();
    ~ScaleSpaceAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void setStringState(juce::Identifier id, const juce::String & newVal);
    void setStringState(const uint32_t index, const juce::String & newVal);
    void setIntState(const uint32_t index, const int newVal);
    const juce::String getStringState(const uint32_t index);
    const int getIntState(const uint32_t index);

    AudioProcessorValueTreeState& getApvts();
    Tunings::Tuning& getTuning(const uint32_t tuningNumber) noexcept;
    Tunings::Tuning getCopyOfTuning(const uint32_t tuningNumber) noexcept;
    Tunings::Tuning& getTestTuning();
    const char* getStateKey(const uint32_t index) noexcept;
    void setNameStateForTuning(const uint32_t tuningNumber, const juce::String & nameString, const uint32_t tunType);
    bool editedTuningValid(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType);
    bool applyNewTuning(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType, const bool setFileState = true);
    bool applySclKbmPair(const uint32_t tuningNumber, const Tunings::Scale &s, const Tunings::KeyboardMapping &k);
    bool retuneToScale(const uint32_t tuningNumber, const Tunings::Scale &s);
    bool applyDroppedFile(const uint32_t tuningNumber, const std::string & filePath);
    void resetTuningStringStatesToStandard(const uint32_t tuningNumber);
    const uint32_t getComplementaryKbmOrSclState(const uint32_t index)  noexcept;
    const uint32_t getFileIndexFromTuningAndFileType(const uint32_t tuningNumber, const uint32_t tunType) noexcept;
    const uint32_t getNameIndexFromTuningAndFileType(const uint32_t tuningNumber, const uint32_t tunType) noexcept;
    void exportCurrentSclTuning(const juce::String & filePath);
    void exportCurrentKbmTuning(const juce::String & filePath);
    void setNoteOn(int noteNumber);
    void setNoteOff(int noteNumber);
    std::bitset<128>& getCurrentKeysOn();
    
    std::atomic<bool> paintFlag;
    std::atomic<bool> keysOnFlag;
    uint32_t currentEditedTuning;
        
    std::string standardSCLTuning;
    std::string standardKBMMapping;
    
protected:
    bool validSclKbmPair(const Tunings::Scale &s, const Tunings::KeyboardMapping &k);
    
private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout addParameters();
    // stringStates XmlElement holds our SCL & KBM file contents and names.
    // This format is also convenient for saving and restoring the states.
    juce::XmlElement stringStates;
    
    // Tunings for Scales 1 to 4
    Tunings::Tuning tuning1;
    Tunings::Tuning tuning2;
    Tunings::Tuning tuning3;
    Tunings::Tuning tuning4;
    // A separate tuning used for testing if SCL &KBM combinations are valid
    Tunings::Tuning testTuning;
    
    // Frequency arrays
    double frequenciesInHz[128];
    double targetFrequenciesInHz[128];
    
    bool previousSmooth;
    bool registeredAsMaster;
    
    void updateMasterRegistration();
    
    std::bitset<128> currentKeysOn;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleSpaceAudioProcessor)
};
