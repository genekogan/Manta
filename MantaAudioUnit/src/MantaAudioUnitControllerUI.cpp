#include "MantaAudioUnitControllerUI.h"

void MantaAudioUnitControllerUI::setupSelectionUI(AudioUnitInstrument * synth)
{
    this->synth = synth;
    //registerAudioUnit(synth);

    ofAddListener(eventPadClick, this, &MantaAudioUnitControllerUI::eventMantaPadClick);
    ofAddListener(eventSliderClick, this, &MantaAudioUnitControllerUI::eventMantaSliderClick);
    ofAddListener(eventButtonClick, this, &MantaAudioUnitControllerUI::eventMantaButtonClick);
    
    guiGroups = new ofxUITabBar();
    guiGroups->setPosition(410, 5);
    guiGroups->addLabel("Groups");
    
    map<string, vector<AudioUnitParameterInfo> >::iterator it = synth->parameterGroups.begin();
    for (; it != synth->parameterGroups.end(); ++it)
    {
        vector<string> itemsP;
        for (auto p : synth->parameterGroups[it->first]) {
            itemsP.push_back(p.name);
        }
        ofxUICanvas* gui = new ofxUICanvas();
        gui->setName(it->first);
        gui->addLabel(it->first);;
        gui->addSpacer();
        ofxUIDropDownList *dd = gui->addDropDownList("Parameters", itemsP);
        dd->open();
        dd->setAutoClose(false);
        gui->autoSizeToFitWidgets();
        guiGroups->addCanvas(gui);
        ofAddListener(gui->newGUIEvent, this, &MantaAudioUnitControllerUI::guiEvent);
    }
    
    guiGroups->addSpacer();
    guiGroups->addLabel("MIDI");
    
    ofxUICanvas* guiMidi = new ofxUICanvas();
    guiMidi->setName("Midi selection");
    guiMidi->addLabel("Midi selection");
    guiMidi->addSpacer();
    guiMidi->addButton("clear", false);
    guiMidi->addButton("map to selection", false);
    guiMidi->autoSizeToFitWidgets();
    guiGroups->addCanvas(guiMidi);
    //ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    
    guiGroups->addSpacer();
    guiGroups->addLabel("View");
    
    ofxUICanvas* guiView = new ofxUICanvas();
    guiView->setName("Summary");
    guiView->addSpacer();
    guiView->autoSizeToFitWidgets();
    guiGroups->addCanvas(guiView);
    
    ofxUICanvas* guiPresets = new ofxUICanvas();
    guiPresets->setName("Presets");
    guiPresets->addLabel("Presets");
    guiPresets->addSpacer();
    guiPresets->addButton("Save preset", false);
    guiPresets->addSpacer();
    
    vector<string> presets;
    presets.push_back("Preset 1");
    presets.push_back("Preset 2");
    presets.push_back("Preset 3");
    ofxUIDropDownList *dd = guiPresets->addDropDownList("Parameters", presets);
    dd->open();
    dd->setAutoClose(false);
    
    guiPresets->autoSizeToFitWidgets();
    guiGroups->addCanvas(guiPresets);
    
    
    
    guiMapper = new ofxUICanvas("Map parameter");
    guiMapper->setPosition(800, 5);
    guiMantaElement = guiMapper->addTextArea("Manta", "manta_element");
    guiSelectedParameter = guiMapper->addTextArea("Parameter", "my_param");
    guiMapper->addLabelButton("Map", false);
    guiRange = guiMapper->addRangeSlider("Range", 0, 1, 0, 1);
    guiMapper->autoSizeToFitWidgets();
    ofAddListener(guiMapper->newGUIEvent, this, &MantaAudioUnitControllerUI::guiMapEvent);
}

void MantaAudioUnitControllerUI::eventMantaPadClick(int & e)
{
    guiMantaElement->setTextString("Pad ("+ofToString(floor(e / 8))+", "+ofToString(e % 8)+")");
    selectedElement.index = e;
    selectedElement.type = PAD;
    setMantaParameterPair();
}

void MantaAudioUnitControllerUI::eventMantaSliderClick(int & e)
{
    guiMantaElement->setTextString("Slider "+ofToString(e));
    selectedElement.index = e;
    selectedElement.type = SLIDER;
    setMantaParameterPair();
}

void MantaAudioUnitControllerUI::eventMantaButtonClick(int & e)
{
    guiMantaElement->setTextString("Button "+ofToString(e));
    selectedElement.index = e;
    selectedElement.type = BUTTON;
    setMantaParameterPair();
}

void MantaAudioUnitControllerUI::guiMapEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "Map")
    {
        if (selectedElement.type == PAD)
        {
            int row = floor(selectedElement.index / 8);
            int col = selectedElement.index % 8;
            mapPadToParameter(row, col, *synth, guiSelectedParameter->getTextString());
        }
        else if (selectedElement.type == SLIDER)
        {
            mapSliderToParameter(selectedElement.index, *synth, guiSelectedParameter->getTextString());
        }
        else if (selectedElement.type == BUTTON)
        {
            mapButtonToParameter(selectedElement.index, *synth, guiSelectedParameter->getTextString());
        }
    }
}

void MantaAudioUnitControllerUI::setMantaParameterPair()
{
    if      (selectedElement.type == PAD) {
        
    }
    else if (selectedElement.type == SLIDER) {
    
    }
    else if (selectedElement.type == BUTTON) {
    
    }
}

void MantaAudioUnitControllerUI::guiEvent(ofxUIEventArgs &e)
{
    guiSelectedParameter->setTextString(e.getName());
}

MantaAudioUnitControllerUI::~MantaAudioUnitControllerUI()
{
    ofRemoveListener(eventPadClick, this, &MantaAudioUnitControllerUI::eventMantaPadClick);
    ofRemoveListener(eventSliderClick, this, &MantaAudioUnitControllerUI::eventMantaSliderClick);
    ofRemoveListener(eventButtonClick, this, &MantaAudioUnitControllerUI::eventMantaButtonClick);

    // clear gui elements....
    // should the other be virtual?
}