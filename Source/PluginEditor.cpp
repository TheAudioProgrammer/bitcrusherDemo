/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    getLookAndFeel().setColour(Slider::trackColourId, Colours::orange);
    
    getLookAndFeel().setColour(Slider::thumbColourId, Colours::yellowgreen);
    
    
    noise.setName("Noise Amount");
    noise.setRange(0, 100); // %
    noise.setValue(processor.getParameters()[0]->getValue());
    noise.setSliderStyle(Slider::LinearVertical);
    noise.setColour(Slider::textBoxTextColourId, Colours::white);
    noise.setTextBoxStyle(Slider::TextBoxBelow, false, 200, 20);
    noise.setValue(0);
    noise.addListener(this);
    addAndMakeVisible(&noise);
    
    bitRedux.setName("Bits");
    bitRedux.setRange(2, 32);
    bitRedux.setValue(processor.getParameters()[0]->getValue());
    bitRedux.setSliderStyle(Slider::LinearVertical);
    bitRedux.setColour(Slider::textBoxTextColourId, Colours::white);
    bitRedux.setTextBoxStyle(Slider::TextBoxBelow, false, 200, 20);
    bitRedux.setValue(32);
    bitRedux.addListener(this);
    addAndMakeVisible(&bitRedux);
    
    rateRedux.setName("Rate");
    rateRedux.setRange(1, 50); // division rate (rate / x)
    rateRedux.setValue(processor.getParameters()[0]->getValue());
    rateRedux.setSliderStyle(Slider::LinearVertical);
    rateRedux.setColour(Slider::textBoxTextColourId, Colours::white);
    rateRedux.setTextBoxStyle(Slider::TextBoxBelow, false, 200, 20);
    rateRedux.setValue(1);
    rateRedux.addListener(this);
    addAndMakeVisible(&rateRedux);
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

void NewProjectAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    
    if ( slider == &noise)
    {
        processor.getParameters()[0]->setValue(slider->getValue());
    }
    else if ( slider == &rateRedux)
    {
        processor.getParameters()[1]->setValue(slider->getValue());
        
    }
    else if ( slider == &bitRedux)
    {
        processor.getParameters()[2]->setValue(slider->getValue());
        
    }

    
}


//==============================================================================
void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::midnightblue.withMultipliedBrightness(.4));
 
    g.setColour(Colours::beige);
    
    int labelW = 100;
    g.drawText("Noise", noise.getX() + noise.getWidth()/2 - labelW/2, 10, labelW, 20, Justification::centred);
    
    g.drawText("Bit", bitRedux.getX() + bitRedux.getWidth()/2 - labelW/2, 10, labelW, 20, Justification::centred);
    
    g.drawText("Rate", rateRedux.getX() + rateRedux.getWidth()/2 - labelW/2, 10, labelW, 20, Justification::centred);
    
    
}

void NewProjectAudioProcessorEditor::resized()
{
    
    int margin = 10;
    int w = 60;
    int y = 50;
    
    noise.setBounds(getWidth()/4 - w/2, y, w, getHeight() - y - margin);
    
    bitRedux.setBounds(2*getWidth()/4 - w/2, y, w, getHeight() - y - margin);
    
    rateRedux.setBounds(3*getWidth()/4 - w/2, y, w, getHeight() - y - margin);
    
    
}
