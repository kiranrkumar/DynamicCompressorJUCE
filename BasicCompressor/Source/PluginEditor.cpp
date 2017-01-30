/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <ctime>


//==============================================================================
BasicCompressorAudioProcessorEditor::BasicCompressorAudioProcessorEditor (BasicCompressorAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 570);
    
    //Initialize slider properties with custom function
    initSlider(&slInGain, Slider::LinearVertical, "Input Gain", Slider::TextBoxAbove, false, " db", -12.0, 12.0, 0.1, 0.0);
    initSlider(&slRatio, Slider::RotaryHorizontalVerticalDrag, "Ratio", Slider::TextBoxAbove, false, ":1", 1.0, 40.0, 1.0, 1.0);
    initSlider(&slThreshold, Slider::LinearVertical, "Threshold", Slider::TextBoxAbove, false, " db", -60.0, 0.0, 0.15, 0.0);
    initSlider(&slAtkTime, Slider::RotaryHorizontalVerticalDrag, "Attack", Slider::TextBoxAbove, false, " ms", 0.0, 250.0, 0.5, 0.0);
    initSlider(&slRelTime, Slider::RotaryHorizontalVerticalDrag, "Release", Slider::TextBoxAbove, false, " ms", 0.0, 250.0, 0.5, 0.0);
    initSlider(&slOutGain, Slider::LinearVertical, "Gain", Slider::TextBoxAbove, false, " db", -20.0, 12.0, 0.1, 0.0);
    initSlider(&slLevel, Slider::RotaryHorizontalVerticalDrag, "Level", Slider::TextBoxAbove, true, "", 0.0, 2.0, 0.01, 0.0, false);
    
    //Initialize buttons
    initButton(&btnPeak, "Peak", DETECTION_GROUP);
    initButton(&btnRMS, "RMS", DETECTION_GROUP);
    
    
    //Trigger initial slider values
    sliderValueChanged(&slInGain);
    sliderValueChanged(&slRatio);
    sliderValueChanged(&slThreshold);
    sliderValueChanged(&slOutGain);
    sliderValueChanged(&slAtkTime);
    sliderValueChanged(&slRelTime);
    
    //trigger button
    btnPeak.triggerClick();
    
    startTimer(10); //begin timer with a 10ms callback
    
}

BasicCompressorAudioProcessorEditor::~BasicCompressorAudioProcessorEditor()
{
}

//==============================================================================
void BasicCompressorAudioProcessorEditor::paint (Graphics& g)
{
//    g.fillAll (Colours::black);
    g.fillAll(Colour::Colour(45, 45, 45));
    
    g.setColour (Colours::white);
//    g.setFont (12.0f);
//    std::string displayTime = "Last Updated: " + std::string(__DATE__) + " at " + std::string(__TIME__);
//    g.drawFittedText(displayTime, 0, 1, getWidth(), 50, Justification::centred, 1);
    //g.drawFittedText("Slope", 20, 30, 70, 50, Justification::left, 1);
    g.setFont(15.0f);
    g.drawFittedText("Input Gain", 10, 30, 90, 50, Justification::left, 1);
    g.drawFittedText("Ratio", 120, 30, 70, 50, Justification::left, 1);
    g.drawFittedText("Detection Mode", 230, 30, 100, 50, Justification::left, 1);
    g.drawFittedText("Threshold", 375, 30, 70, 50, Justification::left, 1);
    g.drawFittedText("Output Gain", 485, 30, 70, 50, Justification::left, 1);
    
    g.drawFittedText("Attack", 115, 165, 60, 30, Justification::left, 1);
    g.drawFittedText("Release", 190, 165, 60, 30, Justification::left, 1);
    
    g.drawFittedText("Output RMS", 250, 300, 100, 20, Justification::centred, 1);
    
}

void BasicCompressorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    slInGain.setBounds(10, 80, 90, 200);
    slRatio.setBounds(120, 80, 80, 80);
    slThreshold.setBounds(370, 80, 90, 200);
    slOutGain.setBounds(480, 80, 90, 200);
    slAtkTime.setBounds(115, 200, 70, 70);
    slRelTime.setBounds(190, 200, 70, 70);
    btnPeak.setBounds(230, 80, 100, 20);
    btnRMS.setBounds(230, 110, 100, 20);
    slLevel.setBounds(200, 340, 200, 200);
}

void BasicCompressorAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    String slName = slider->getName();
    
    if (slName == "Input Gain")
    {
        processor.curInGain = slider->getValue();
    }
    else if (slName == "Ratio")
    {
        processor.curRatio = slider->getValue();
    }
    else if (slName == "Threshold")
    {
        processor.curThresh = slider->getValue();
    }
    else if (slName == "Gain")
    {
        processor.curOutGain = slider->getValue();
    }
    else if (slName == "Attack")
    {
        processor.curAtkTime = slider->getValue() / 1000.0f;
    }
    else if (slName == "Release")
    {
        processor.curRelTime = slider->getValue() / 1000.0f;
    }
}

void BasicCompressorAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button->getName() == "Peak")
    {
        processor.detMode = PEAK;
    }
    else if (button->getName() == "RMS")
    {
        processor.detMode = RMS;
    }
    
}

void BasicCompressorAudioProcessorEditor::timerCallback()
{
    slLevel.setValue(processor.curSampleVal);
}

//initialize a slider with many parameters
void BasicCompressorAudioProcessorEditor::initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
                Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
                juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener)
{
    //use the parameters given
    slider->setSliderStyle(newStyle);
    slider->setName(newName);
    slider->setTextBoxStyle(newTxtBoxPos, txtIsReadOnly, slider->getTextBoxWidth(), slider->getTextBoxHeight());
    slider->setTextValueSuffix(newSuffix);
    slider->setRange(newMin, newMax, newInc);
    slider->setValue(newValue);
    
    //set up apperance
    slider->setColour(Slider::thumbColourId, Colours::deepskyblue);
    slider->setColour(Slider::textBoxHighlightColourId, Colours::red);
    
    //for rotary sliders only
    if  ((newStyle == Slider::RotaryHorizontalVerticalDrag) || (newStyle == Slider::RotaryHorizontalDrag) || (newStyle = Slider::RotaryVerticalDrag))
    {
        slider->setColour(Slider::rotarySliderFillColourId, Colours::deepskyblue);
        slider->setColour(Slider::rotarySliderOutlineColourId, Colours::deepskyblue);
    }
    
    addAndMakeVisible(slider);
    if (addListener == true)
    {
        slider->addListener(this);
    }
    
}

void BasicCompressorAudioProcessorEditor::initButton(Button* btn, String btnName, int buttonGroup)
{
    addAndMakeVisible(btn);
    btn->addListener(this);
    btn->setButtonText(btnName);
    btn->setName(btnName);
    btn->setColour(ToggleButton::textColourId, Colours::white);
    btn->setColour(ToggleButton::tickColourId, Colours::deepskyblue);
    btn->setRadioGroupId(buttonGroup);
}
