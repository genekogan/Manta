#include "MantaAudioUnitControllerUI.h"


MantaAudioUnitControllerUI::MantaAudioUnitControllerUI() : MantaAudioUnitController()
{
    
}

void MantaAudioUnitControllerUI::setupUI()
{
    ofAddListener(eventPadClick, this, &MantaAudioUnitControllerUI::eventMantaPadClick);
    ofAddListener(eventSliderClick, this, &MantaAudioUnitControllerUI::eventMantaSliderClick);
    ofAddListener(eventButtonClick, this, &MantaAudioUnitControllerUI::eventMantaButtonClick);
    
    selectedElement.type = NONE;
    
    guiGroups = new ofxUITabBar();
    guiGroups->setPosition(410, 5);
    
    map<string, AudioUnitInstrument*>::iterator its = synths.begin();
    for (; its != synths.end(); ++its)
    {
        guiGroups->addLabel(its->second->getName());
        map<string, vector<AudioUnitParameterInfo> >::iterator it = its->second->getParameterGroups().begin();
        for (; it != its->second->getParameterGroups().end(); ++it)
        {
            vector<string> itemsP;
            for (auto p : its->second->getParameterGroups()[it->first]) {
                itemsP.push_back(p.name);
                parameterLU[make_pair(its->second, p.name)] = p;
            }

            int countWithSameName = 0;
            map<ofxUICanvas*, AudioUnitInstrument*>::iterator itg = groupSynthLU.begin();
            for (; itg != groupSynthLU.end(); ++itg) {
                countWithSameName += (itg->first->getName() == it->first);
            }
            
            ofxUICanvas* gui = new ofxUICanvas();
            gui->setName(countWithSameName == 0 ? it->first : it->first + "("+ofToString(countWithSameName)+")");
            gui->addLabel(it->first);
            gui->addSpacer();
            ofxUIDropDownList *dd = gui->addDropDownList("Parameters", itemsP);
            dd->open();
            dd->setAutoClose(false);
            gui->autoSizeToFitWidgets();

            guiParameterGroups.push_back(gui);
            guiGroups->addCanvas(gui);
            groupSynthLU[gui] = its->second;
            ofAddListener(gui->newGUIEvent, this, &MantaAudioUnitControllerUI::guiSelectEvent);
        }
        guiGroups->addSpacer();
    }
    
    // Midi selection gui
    guiMidi = new ofxUICanvas();
    guiMidi->setName("Midi selection");
    guiMidi->addLabel("Midi selection");
    guiMidi->addSpacer();
    guiMidi->addButton("clear", false);
    its = synths.begin();
    for (; its != synths.end(); ++its) {
        ofxUIButton *b = guiMidi->addButton("map selection to "+its->second->getName(), false);
        synthLU[b] = its->second;
    }
    guiMidi->autoSizeToFitWidgets();
    ofAddListener(guiMidi->newGUIEvent, this, &MantaAudioUnitControllerUI::guiMidiEvent);
    
    // Mapping summary gui
    guiView = new ofxUICanvas();
    guiView->setName("Summary");
    guiView->addLabel("Summary");
    guiView->addSpacer();
    guiView->autoSizeToFitWidgets();
    ofAddListener(guiView->newGUIEvent, this, &MantaAudioUnitControllerUI::guiViewEvent);

    // Presets gui
    guiPresets = new ofxUICanvas();
    guiPresets->setName("Presets");
    guiPresets->addLabel("Presets");
    guiPresets->addSpacer();
    guiPresets->addButton("Save preset", false);
    guiPresets->addSpacer();
    
    vector<string> presets;
    ofDirectory dir(ofToDataPath("presets/"));
    dir.allowExt("xml");
    dir.listDir();
    for(int i = 0; i < dir.numFiles(); i++) {
        string presetName = dir.getName(i);
        presets.push_back(presetName.substr(0, presetName.length()-4));
    }
    ofxUIDropDownList *dd = guiPresets->addDropDownList("Load Preset", presets);
    dd->open();
    dd->setAutoClose(false);
    guiPresets->autoSizeToFitWidgets();
    ofAddListener(guiPresets->newGUIEvent, this, &MantaAudioUnitControllerUI::guiPresetsEvent);
    
    guiGroups->addLabel("MIDI");
    guiGroups->addCanvas(guiMidi);
    guiGroups->addSpacer();
    guiGroups->addLabel("View");
    guiGroups->addCanvas(guiView);
    guiGroups->addCanvas(guiPresets);
    
    // Parameter mapping gui
    guiMapper = new ofxUICanvas("Map parameter");
    guiMapper->setPosition(800, 5);
    guiMantaElement = guiMapper->addTextArea("Manta", "manta_element");
    guiSelectedParameter = guiMapper->addTextArea("Parameter", "my_param");
    guiMapper->addLabelButton("Map", false);
    guiRange = guiMapper->addRangeSlider("Range", 0, 1, 0, 1);
    guiDelete = guiMapper->addButton("Delete", false);
    guiDelete->setVisible(false);
    guiMapper->autoSizeToFitWidgets();
    ofAddListener(guiMapper->newGUIEvent, this, &MantaAudioUnitControllerUI::guiMapEvent);
}

void MantaAudioUnitControllerUI::eventMantaPadClick(int & e)
{
    guiMantaElement->setTextString("Pad ("+ofToString(floor(e / 8))+", "+ofToString(e % 8)+")");
    selectedElement.index = e;
    selectedElement.type = PAD;
}

void MantaAudioUnitControllerUI::eventMantaSliderClick(int & e)
{
    guiMantaElement->setTextString("Slider "+ofToString(e));
    selectedElement.index = e;
    selectedElement.type = SLIDER;
}

void MantaAudioUnitControllerUI::eventMantaButtonClick(int & e)
{
    guiMantaElement->setTextString("Button "+ofToString(e));
    selectedElement.index = e;
    selectedElement.type = BUTTON;
}

void MantaAudioUnitControllerUI::guiMidiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "clear") {
        clearMidiMapping();
    }
    else {
        mapSelectionToMidiNotes(*synthLU[e.getButton()]);
    }
}

void MantaAudioUnitControllerUI::guiMapEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "Map")
    {
        if (e.getButton()->getValue())  return;
        addSelectedMapping();
    }
    else if (e.getName() == "Range")
    {
        float low = guiRange->getValueLow();
        float high = guiRange->getValueHigh();
        map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
        map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
        map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
        for (; itp != padMap.end(); ++itp) {
            if (itp->second->parameter.getName() == guiSelectedParameter->getTextString() &&
                itp->second->synthName == selectedAudioUnit->getName()) {
                itp->second->min = low;
                itp->second->max = high;
                return;
            }
        }
        for (; its != sliderMap.end(); ++its) {
            if (its->second->parameter.getName() == guiSelectedParameter->getTextString() &&
                its->second->synthName == selectedAudioUnit->getName()) {
                its->second->min = low;
                its->second->max = high;
                return;
            }
        }
        for (; itb != buttonMap.end(); ++itb) {
            if (itb->second->parameter.getName() == guiSelectedParameter->getTextString() &&
                itb->second->synthName == selectedAudioUnit->getName()) {
                itb->second->min = low;
                itb->second->max = high;
                return;
            }
        }
    }
    else if (e.getName() == "Delete") {
        if (e.getButton()->getValue())  return;
        removeSelectedMapping();
    }
}

void MantaAudioUnitControllerUI::guiViewEvent(ofxUIEventArgs &e)
{
    if (e.getButton()->getValue())  return;
    
    selectedViewMappingToggle = e.getToggle();
    selectedAudioUnit = buttonMappingLU[e.getButton()]->selectedAudioUnit;
    selectedElement = buttonMappingLU[e.getButton()]->mantaElement;
    guiSelectedParameter->setTextString(buttonMappingLU[e.getButton()]->parameterName);
    
    if      (selectedElement.type == NONE) {
        guiMantaElement->setTextString("*** none selected ***");
    }
    else if (selectedElement.type == PAD) {
        guiMantaElement->setTextString("Pad ("+ofToString(floor(selectedElement.index / 8))+", "+ofToString(selectedElement.index % 8)+")");
    }
    else if (selectedElement.type == SLIDER) {
        guiMantaElement->setTextString("Slider "+ofToString(selectedElement.index));
    }
    else if (selectedElement.type == BUTTON) {
        guiMantaElement->setTextString("Button "+ofToString(selectedElement.index));
    }
    
    checkSelectedPair();
}

void MantaAudioUnitControllerUI::guiSelectEvent(ofxUIEventArgs &e)
{
    selectedParameterToggle = e.getToggle();
    selectedAudioUnit = groupSynthLU[e.getCanvasParent()];
    guiSelectedParameter->setTextString(e.getName());
    checkSelectedPair();
}

void MantaAudioUnitControllerUI::guiPresetsEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "Save preset"){
        if (e.getButton()->getValue())  return;
        string presetName = ofSystemTextBoxDialog("Preset name");
        savePreset(presetName);
        ((ofxUIDropDownList *) guiPresets->getWidget("Load Preset"))->addToggle(presetName);
        guiPresets->autoSizeToFitWidgets();
    }
    else if (e.getParentName() == "Load Preset") {
        loadPreset(e.getName());
        map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
        map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
        map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
        for (; itp != padMap.end(); ++itp) {
            string newEntryString = "P("+ofToString(floor(itp->first / 8))+","+ofToString(itp->first % 8)+") => "+itp->second->synthName+":"+itp->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedAudioUnit = synths[itp->second->synthName];
            selectedElement.index = itp->first;
            selectedElement.type = PAD;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectedAudioUnit, selectedElement, itp->second->parameter.getName());
        }
        for (; its != sliderMap.end(); ++its) {
            string newEntryString = "S("+ofToString(its->first)+") => "+its->second->synthName+":"+its->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedAudioUnit = synths[its->second->synthName];
            selectedElement.index = its->first;
            selectedElement.type = SLIDER;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectedAudioUnit, selectedElement, its->second->parameter.getName());
        }
        for (; itb != buttonMap.end(); ++itb) {
            string newEntryString = "B("+ofToString(itb->first)+") => "+itb->second->synthName+":"+itb->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedAudioUnit = synths[itb->second->synthName];
            selectedElement.index = itb->first;
            selectedElement.type = BUTTON;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectedAudioUnit, selectedElement, itb->second->parameter.getName());
        }
        guiView->autoSizeToFitWidgets();
    }
}

void MantaAudioUnitControllerUI::checkSelectedPair()
{
    AudioUnitParameterInfo p = parameterLU[make_pair(selectedAudioUnit, guiSelectedParameter->getTextString())];
    guiRange->setMin(p.minValue);
    guiRange->setMax(p.maxValue);
    guiRange->setValueLow(p.minValue);
    guiRange->setValueHigh(p.maxValue);
    
    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
    map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();

    for (; itp != padMap.end(); ++itp) {
        if (itp->second->parameter.getName() == guiSelectedParameter->getTextString() &&
            itp->second->synthName == selectedAudioUnit->getName()) {
            guiRange->setValueLow(itp->second->min);
            guiRange->setValueHigh(itp->second->max);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }
    for (; its != sliderMap.end(); ++its) {
        if (its->second->parameter.getName() == guiSelectedParameter->getTextString() &&
            its->second->synthName == selectedAudioUnit->getName()) {
            guiRange->setValueLow(its->second->min);
            guiRange->setValueHigh(its->second->max);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }
    for (; itb != buttonMap.end(); ++itb) {
        if (itb->second->parameter.getName() == guiSelectedParameter->getTextString() &&
            itb->second->synthName == selectedAudioUnit->getName()) {
            guiRange->setValueLow(itb->second->min);
            guiRange->setValueHigh(itb->second->max);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }

    guiDelete->setVisible(false);
    guiMapper->autoSizeToFitWidgets();
}

void MantaAudioUnitControllerUI::addSelectedMapping()
{
    string newEntryString;
    if (selectedElement.type == NONE) {
        return;
    }
    else if (selectedElement.type == PAD)
    {
        int row = floor(selectedElement.index / 8);
        int col = selectedElement.index % 8;
        mapPadToParameter(row, col, *selectedAudioUnit, guiSelectedParameter->getTextString());
        padMap[selectedElement.index]->min = guiRange->getValueLow();
        padMap[selectedElement.index]->max = guiRange->getValueHigh();
        newEntryString = "P("+ofToString(row)+","+ofToString(col)+") => "+selectedAudioUnit->getName()+":"+guiSelectedParameter->getTextString();
    }
    else if (selectedElement.type == SLIDER)
    {
        mapSliderToParameter(selectedElement.index, *selectedAudioUnit, guiSelectedParameter->getTextString());
        sliderMap[selectedElement.index]->min = guiRange->getValueLow();
        sliderMap[selectedElement.index]->max = guiRange->getValueHigh();
        newEntryString = "S("+ofToString(selectedElement.index)+") => "+selectedAudioUnit->getName()+":"+guiSelectedParameter->getTextString();
    }
    else if (selectedElement.type == BUTTON)
    {
        mapButtonToParameter(selectedElement.index, *selectedAudioUnit, guiSelectedParameter->getTextString());
        buttonMap[selectedElement.index]->min = guiRange->getValueLow();
        buttonMap[selectedElement.index]->max = guiRange->getValueHigh();
        newEntryString = "B("+ofToString(selectedElement.index)+") => "+selectedAudioUnit->getName()+":"+guiSelectedParameter->getTextString();
    }

    ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
    buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectedAudioUnit, selectedElement, guiSelectedParameter->getTextString());
    guiDelete->setVisible(true);
    guiMapper->autoSizeToFitWidgets();
    guiView->autoSizeToFitWidgets();
    selectedParameterToggle->setColorBack(ofColor(190, 5, 5));
}

void MantaAudioUnitControllerUI::removeSelectedMapping()
{
    if (selectedElement.type == NONE) {
        return;
    }
    else if (selectedElement.type == PAD) {
        removePadMapping(floor(selectedElement.index / 8), selectedElement.index % 8);
    }
    else if (selectedElement.type == SLIDER) {
        removeSliderMapping(selectedElement.index);
    }
    else if (selectedElement.type == BUTTON) {
        removeButtonMapping(selectedElement.index);
    }
    guiDelete->setVisible(false);
    guiView->removeWidget(selectedViewMappingToggle);
    guiView->autoSizeToFitWidgets();
    guiView->stateChange();
    selectedParameterToggle->setColorBack(OFX_UI_COLOR_BACK);
}

MantaAudioUnitControllerUI::~MantaAudioUnitControllerUI()
{
    ofRemoveListener(eventPadClick, this, &MantaAudioUnitControllerUI::eventMantaPadClick);
    ofRemoveListener(eventSliderClick, this, &MantaAudioUnitControllerUI::eventMantaSliderClick);
    ofRemoveListener(eventButtonClick, this, &MantaAudioUnitControllerUI::eventMantaButtonClick);

    parameterLU.clear();
    synthLU.clear();
    groupSynthLU.clear();
    buttonMappingLU.clear();
    
    vector<ofxUICanvas*>::iterator itg = guiParameterGroups.begin();
    while (itg != guiParameterGroups.end()) {
        delete *itg;
        guiParameterGroups.erase(itg);
    }
    guiParameterGroups.clear();
    
    delete guiPresets;
    delete guiMidi;
    delete guiView;
    delete guiMapper;
    delete guiGroups;
}