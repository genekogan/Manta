#include "MantaAbletonControllerUI.h"


MantaAbletonControllerUI::MantaAbletonControllerUI()
{
    guiGroups = new ofxUITabBar();
    guiMidi = new ofxUICanvas("Options");
    guiMapper = new ofxUICanvas("Map parameter");
    guiView = new ofxUICanvas("View");
    guiPresets = new ofxUICanvas("Presets");

    guiGroups->setPosition(410, 5);
    guiMidi->setPosition(800, 175);
    guiMapper->setPosition(800, 5);
    
    guiGroups->setVisible(false);
    guiMidi->setVisible(false);
    guiMapper->setVisible(false);
    guiView->setVisible(false);
    
    guiPresets->setPosition(410, 5);
    setupUIPresets();
}

void MantaAbletonControllerUI::abletonLoaded()
{
    setupUI();
}

void MantaAbletonControllerUI::setupUIPresets()
{
    guiPresets->clearWidgets();
    guiPresets->removeWidgets();
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
    ofAddListener(guiPresets->newGUIEvent, this, &MantaAbletonControllerUI::guiPresetsEvent);
}

void MantaAbletonControllerUI::setupUI()
{
    ofAddListener(eventPadClick, this, &MantaAbletonControllerUI::eventMantaPadClick);
    ofAddListener(eventSliderClick, this, &MantaAbletonControllerUI::eventMantaSliderClick);
    ofAddListener(eventButtonClick, this, &MantaAbletonControllerUI::eventMantaButtonClick);
    ofAddListener(eventStatClick, this, &MantaAbletonControllerUI::eventMantaStatClick);
    
    selectedElement.type = NONE;

    guiGroups->setVisible(true);
    guiMidi->setVisible(true);
    guiMapper->setVisible(true);
    guiView->setVisible(true);
    
    groupDeviceLU.clear();
    groupTrackLU.clear();

    for (int i=0; i<innerGuis.size(); i++)
    {
        innerGuis[i]->clearWidgets();
        innerGuis[i]->removeWidgets();
    }
    innerGuis.clear();
    guiGroups->clearWidgets();
    guiGroups->removeWidgets();
    guiMidi->clearWidgets();
    guiMidi->removeWidgets();
    guiView->clearWidgets();
    guiView->removeWidgets();
    guiMapper->clearWidgets();
    guiMapper->removeWidgets();
    
    // Global parameters
    vector<string> items;
    items.push_back("tempo");
    items.push_back("time");
    items.push_back("volume");
    items.push_back("pan");
    items.push_back("crossfade");
    ofxUICanvas* guiGlobal = new ofxUICanvas();
    guiGlobal->setName("Global parameters");
    guiGlobal->addLabel("Global parameters");
    guiGlobal->addSpacer();
    ofxUIDropDownList *ddg = guiGlobal->addDropDownList("Parameters", items);
    ddg->open();
    ddg->setAutoClose(false);
    guiGlobal->autoSizeToFitWidgets();
    ofAddListener(guiGlobal->newGUIEvent, this, &MantaAbletonControllerUI::guiSelectGlobalEvent);
    guiGroups->addLabel("Global");
    guiGroups->addCanvas(guiGlobal);
    
    // Track parameters
    guiGroups->addLabel("Tracks");
    map<int, ofxAbletonLiveTrack*>::iterator it = live->getTracks().begin();
    for (; it != live->getTracks().end(); ++it)
    {
        vector<string> items;
        items.push_back("volume");
        items.push_back("pan");
        items.push_back("send");

        ofxUICanvas* gui = new ofxUICanvas();
        gui->setName(it->second->getName());
        gui->addLabel(it->second->getName());
        gui->addSpacer();
        ofxUIDropDownList *dd = gui->addDropDownList("Parameters", items);
        dd->open();
        dd->setAutoClose(false);
        gui->autoSizeToFitWidgets();
        ofAddListener(gui->newGUIEvent, this, &MantaAbletonControllerUI::guiSelectTrackEvent);
        guiGroups->addCanvas(gui);
        innerGuis.push_back(gui);
        
        groupTrackLU[gui] = it->first;
    }
    guiGroups->addSpacer();
    
    // Track device parameters
    it = live->getTracks().begin();
    for (; it != live->getTracks().end(); ++it)
    {
        guiGroups->addLabel(it->second->getName());
        map<int, ofxAbletonLiveDevice*>::iterator itd = it->second->getDevices().begin();
        for (; itd != it->second->getDevices().end(); ++itd)
        {
            vector<string> itemsP;
            map<int, ofxAbletonLiveParameter*>::iterator itp = itd->second->getParameters().begin();
            for (; itp != itd->second->getParameters().end(); ++itp) {
                itemsP.push_back(itp->second->getName());
                parameterLU[make_pair(itd->second, itp->second->getName())] = itp->second;
            }
            
            int countWithSameName = 0;
            map<ofxUICanvas*, ofxAbletonLiveDevice*>::iterator itg = groupDeviceLU.begin();
            for (; itg != groupDeviceLU.end(); ++itg) {
                countWithSameName += (itg->first->getName() == itd->second->getName());
            }

            ofxUICanvas* gui = new ofxUICanvas();
            gui->setName(countWithSameName == 0 ? itd->second->getName() : itd->second->getName() + "("+ofToString(countWithSameName)+")");
            gui->addLabel(itd->second->getName());
            gui->addSpacer();
            ofxUIDropDownList *dd = gui->addDropDownList("Parameters", itemsP);
            dd->open();
            dd->setAutoClose(false);
            gui->autoSizeToFitWidgets();
            
            guiParameterGroups.push_back(gui);
            guiGroups->addCanvas(gui);
            innerGuis.push_back(gui);
            groupDeviceLU[gui] = itd->second;
            groupTrackLU[gui] = it->first;
            ofAddListener(gui->newGUIEvent, this, &MantaAbletonControllerUI::guiSelectEvent);
        }
    
        guiGroups->addSpacer();
    }
    
    // Midi selection gui
    guiMidi->addLabel("Options");
    guiMidi->addSpacer();
    guiMidi->addLabelButton("Refresh Ableton", false);
    guiMidi->addLabelToggle("Pad freeze enabled", &toFreezePads);
    guiMidi->addSpacer();
    guiMidi->addLabel("Midi selection");
    guiMidi->addSpacer();
    guiMidi->addButton("clear", false);
    guiMidiChannel = guiMidi->addIntSlider("Midi channel", 1, 16, 1);
    guiMidiMap = guiMidi->addLabelButton("map selection to midi", false);
    guiMidiMap->setLabelText("map selection to ch. 1");
    guiMidi->addSpacer();
    
    vector<string> modes;
    modes.push_back("Major / Ionian");
    modes.push_back("Dorian");
    modes.push_back("Phrygian");
    modes.push_back("Lydian");
    modes.push_back("Mixolydian");
    modes.push_back("Aeolian");
    modes.push_back("Locrian");
    modes.push_back("Natural Minor");
    modes.push_back("Harmonic Minor");
    modes.push_back("Melodic Minor");
    guiMidi->addTextArea("Key_", "C");
    guiMidi->addIntSlider("Key", 0, 11, getKey());
    guiMidi->addIntSlider("Octave", 0, 12, getOctave());
    guiMidi->addDropDownList("Mode", modes)->open();
    guiMidi->autoSizeToFitWidgets();
    ofAddListener(guiMidi->newGUIEvent, this, &MantaAbletonControllerUI::guiMidiEvent);
    resetKeyString();
    
    // Mapping summary gui
    guiView->setName("Summary");
    guiView->addLabel("Summary");
    guiView->addSpacer();
    guiView->autoSizeToFitWidgets();
    ofAddListener(guiView->newGUIEvent, this, &MantaAbletonControllerUI::guiViewEvent);
    
    // Presets gui
    setupUIPresets();
    guiGroups->addLabel("View");
    guiGroups->addCanvas(guiView);
    guiGroups->addCanvas(guiPresets);
    innerGuis.push_back(guiView);
    innerGuis.push_back(guiPresets);

    // Parameter mapping gui
    guiMantaElement = guiMapper->addTextArea("Manta", "manta_element");
    guiSelectedParameter = guiMapper->addTextArea("Parameter", "my_param");
    guiMapper->addLabelButton("Map", false);
    guiRange = guiMapper->addRangeSlider("Range", 0, 1, 0, 1);
    guiToggle = guiMapper->addLabelToggle("Toggle Mode", false);
    guiDelete = guiMapper->addButton("Delete", false);
    guiDelete->setVisible(false);
    guiMapper->autoSizeToFitWidgets();
    ofAddListener(guiMapper->newGUIEvent, this, &MantaAbletonControllerUI::guiMapEvent);
    
    toRedrawStats = true;
}

void MantaAbletonControllerUI::eventMantaPadClick(int & e)
{
    guiMantaElement->setTextString("Pad ("+ofToString(floor(e / 8))+", "+ofToString(e % 8)+")");
    selectedElement.index = e;
    selectedElement.type = PAD;
}

void MantaAbletonControllerUI::eventMantaSliderClick(int & e)
{
    guiMantaElement->setTextString("Slider "+ofToString(e));
    selectedElement.index = e;
    selectedElement.type = SLIDER;
}

void MantaAbletonControllerUI::eventMantaButtonClick(int & e)
{
    guiMantaElement->setTextString("Button "+ofToString(e));
    selectedElement.index = e;
    selectedElement.type = BUTTON;
}

void MantaAbletonControllerUI::eventMantaStatClick(int & e)
{
    guiMantaElement->setTextString(getStatsInfo(e).name);
    selectedElement.index = e;
    selectedElement.type = STAT;
}

void MantaAbletonControllerUI::guiMidiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "clear") {
        clearMidiMapping();
    }
    else if (e.getName() == "Mode") return;
    else if (e.getName() == "Major / Ionian") setMode(0);
    else if (e.getName() == "Dorian") setMode(1);
    else if (e.getName() == "Phrygian") setMode(2);
    else if (e.getName() == "Lydian") setMode(3);
    else if (e.getName() == "Mixolydian") setMode(4);
    else if (e.getName() == "Aeolian") setMode(5);
    else if (e.getName() == "Locrian") setMode(6);
    else if (e.getName() == "Natural Minor") setMode(7);
    else if (e.getName() == "Harmonic Minor") setMode(8);
    else if (e.getName() == "Melodic Minor") setMode(9);
    else if (e.getName() == "Key") {
        setKey(((ofxUIIntSlider *) guiMidi->getWidget("Key"))->getValue());
    }
    else if (e.getName() == "Octave") {
        setOctave(((ofxUIIntSlider *) guiMidi->getWidget("Octave"))->getValue());
    }
    else if (e.getName() == "Refresh Ableton") {
        live->refresh();
    }
    else if (e.getName() == "Pad freeze enabled") {
        setPadFreezingEnabled(toFreezePads);
    }
    else if (e.getName() == "Midi channel") {
        guiMidiMap->setLabelText("map selection to ch. "+ofToString(guiMidiChannel->getValue()));
    }
    else if (e.getName() == "map selection to midi") {
        mapSelectionToMidiNotes(guiMidiChannel->getValue());
    }
    resetKeyString();
}

void MantaAbletonControllerUI::resetKeyString()
{
    string keys[12]  = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    string modes[10] = {"Major", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolian", "Locrian", "Natural Minor", "Harmonic Minor", "Melodic Minor"};
    string keyString = keys[getKey()]+ofToString(getOctave())+" "+modes[getMode()];
    ((ofxUITextArea *) guiMidi->getWidget("Key_"))->setTextString(keyString);
}

void MantaAbletonControllerUI::guiMapEvent(ofxUIEventArgs &e)
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
        bool toggle = guiToggle->getValue();
        map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
        map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
        map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
        map<int, MantaParameterMapping*>::iterator itsm = statMap.begin();

        if (selectedElement.type == PAD && padMap.count(selectedElement.index)) {
            if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+padMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+padMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 2 && guiSelectedParameter->getTextString() == padMap[selectedElement.index]->parameter.getName())) {
                padMap[selectedElement.index]->setRange(low, high, toggle);
            }
        }
        else if (selectedElement.type == SLIDER && sliderMap.count(selectedElement.index)) {
            if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+sliderMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+sliderMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 2 && guiSelectedParameter->getTextString() == sliderMap[selectedElement.index]->parameter.getName())) {
                sliderMap[selectedElement.index]->setRange(low, high, toggle);
            }
        }
        else if (selectedElement.type == BUTTON && buttonMap.count(selectedElement.index)) {
            if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+buttonMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+buttonMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 2 && guiSelectedParameter->getTextString() == buttonMap[selectedElement.index]->parameter.getName())) {
                buttonMap[selectedElement.index]->setRange(low, high, toggle);
            }
        }
        else if (selectedElement.type == STAT && statMap.count(selectedElement.index)) {
            if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+statMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+statMap[selectedElement.index]->parameter.getName()) ||
                (selectMode == 2 && guiSelectedParameter->getTextString() == statMap[selectedElement.index]->parameter.getName())) {
                statMap[selectedElement.index]->setRange(low, high, toggle);
            }
        }
    }
    else if (e.getName() == "Toggle Mode")
    {
        bool toggle = guiToggle->getValue();
        map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
        map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
        for (; itp != padMap.end(); ++itp) {
            if (itp->second->parameter.getName() == guiSelectedParameter->getTextString() &&
                itp->second->deviceName == selectedDevice->getName()) {
                itp->second->toggle = toggle;
                updatePadColor(floor(selectedElement.index / 8), selectedElement.index % 8);
                return;
            }
        }
        for (; itb != buttonMap.end(); ++itb) {
            if (itb->second->parameter.getName() == guiSelectedParameter->getTextString() &&
                itb->second->deviceName == selectedDevice->getName()) {
                itb->second->toggle = toggle;
                return;
            }
        }
    }
    else if (e.getName() == "Delete")
    {
        if (e.getButton()->getValue())  return;
        removeSelectedMapping();
    }
}

void MantaAbletonControllerUI::guiViewEvent(ofxUIEventArgs &e)
{
    if (e.getButton()->getValue())  return;
    
    selectedViewMappingToggle = e.getToggle();
    selectMode = buttonMappingLU[e.getButton()]->selectMode;
    selectedDevice = buttonMappingLU[e.getButton()]->selectedDevice;
    selectedTrack = buttonMappingLU[e.getButton()]->selectedTrack;
    selectedElement = buttonMappingLU[e.getButton()]->mantaElement;

    if (selectMode == 0) {
        guiSelectedParameter->setTextString("Global "+buttonMappingLU[e.getButton()]->parameterName);
    }
    else if (selectMode == 1) {
        guiSelectedParameter->setTextString("T"+ofToString(selectedTrack)+" "+buttonMappingLU[e.getButton()]->parameterName);
    }
    else if (selectMode == 2) {
        guiSelectedParameter->setTextString(buttonMappingLU[e.getButton()]->parameterName);
    }
    
    
    
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
    else if (selectedElement.type == STAT) {
        guiMantaElement->setTextString(getStatsInfo(selectedElement.index).name);
    }
    
    checkSelectedPair();
}

void MantaAbletonControllerUI::guiSelectEvent(ofxUIEventArgs &e)
{
    if (((ofxUILabelToggle*) e.getParent())->getValue() == 0) return;
    selectMode = 2;
    selectedParameterToggle = e.getToggle();
    selectedDevice = groupDeviceLU[e.getCanvasParent()];
    selectedTrack = groupTrackLU[e.getCanvasParent()];
    guiSelectedParameter->setTextString(e.getName());
    checkSelectedPair();
}

void MantaAbletonControllerUI::guiSelectTrackEvent(ofxUIEventArgs &e)
{
    if (((ofxUILabelToggle*) e.getParent())->getValue() == 0) return;
    selectMode = 1;
    selectedParameterToggle = e.getToggle();
    selectedTrack = groupTrackLU[e.getCanvasParent()];
    guiSelectedParameter->setTextString("T"+ofToString(selectedTrack)+" "+e.getName());
    checkSelectedPair();
}

void MantaAbletonControllerUI::guiSelectGlobalEvent(ofxUIEventArgs &e)
{
    if (((ofxUILabelToggle*) e.getParent())->getValue() == 0) return;
    selectMode = 0;
    selectedParameterToggle = e.getToggle();
    guiSelectedParameter->setTextString("Global "+e.getName());
    checkSelectedPair();
}

void MantaAbletonControllerUI::guiPresetsEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "Save preset")
    {
        if (e.getButton()->getValue())  return;
        string presetName = ofSystemTextBoxDialog("Preset name");
        ofFileDialogResult alsFileResult = ofSystemLoadDialog("Find Ableton ALS file");
        if (presetName != "" && alsFileResult.bSuccess)
        {
            savePreset(presetName, alsFileResult.filePath);
            ((ofxUIDropDownList *) guiPresets->getWidget("Load Preset"))->addToggle(presetName);
            guiPresets->autoSizeToFitWidgets();
        }
    }
    else if (e.getParentName() == "Load Preset")
    {
        loadPreset(e.getName());
    }
}

void MantaAbletonControllerUI::loadPresetData()
{
    MantaAbletonController::loadPresetData();
    setupUI();

    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
    map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
    map<int, MantaParameterMapping*>::iterator itsm = statMap.begin();
    for (; itp != padMap.end(); ++itp)
    {
        string newEntryString;
        int selectMode_, selectTrack_;
        selectTrack_ = itp->second->track;
        ofxAbletonLiveDevice *selectedDevice_ = NULL;
        if (selectTrack_ == -1) {
            selectMode_ = 0;
            selectedDevice_ = NULL;
            newEntryString = "P("+ofToString(floor(itp->first / 8))+","+ofToString(itp->first % 8)+") => Global "+itp->second->parameter.getName();
        }
        else if (itp->second->deviceName == "Track "+ofToString(selectTrack_)) {
            selectMode_ = 1;
            selectedDevice_ = NULL;
            newEntryString = "P("+ofToString(floor(itp->first / 8))+","+ofToString(itp->first % 8)+") => T"+ofToString(selectTrack_)+" "+itp->second->parameter.getName();
        }
        else {
            selectMode_ = 2;
            selectedDevice_ = live->getTrack(selectTrack_)->getDevice(itp->second->deviceName);
            newEntryString = "P("+ofToString(floor(itp->first / 8))+","+ofToString(itp->first % 8)+") => "+itp->second->deviceName+":"+itp->second->parameter.getName();
        }
        selectedElement.index = itp->first;
        selectedElement.type = PAD;
        ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
        buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode_, selectTrack_, selectedDevice_, selectedElement, itp->second->parameter.getName());
        guiView->autoSizeToFitWidgets();
    }
    for (; its != sliderMap.end(); ++its)
    {
        string newEntryString;
        int selectMode_, selectTrack_;
        selectTrack_ = its->second->track;
        ofxAbletonLiveDevice *selectedDevice_ = NULL;
        if (selectTrack_ == -1) {
            selectMode_ = 0;
            selectedDevice_ = NULL;
            newEntryString = "S("+ofToString(its->first)+") => Global "+its->second->parameter.getName();
        }
        else if (its->second->deviceName == "Track "+ofToString(selectTrack_)) {
            selectMode_ = 1;
            selectedDevice_ = NULL;
            newEntryString = "S("+ofToString(its->first)+") => T"+ofToString(selectTrack_)+" "+its->second->parameter.getName();
        }
        else {
            selectMode_ = 2;
            selectedDevice_ = live->getTrack(selectTrack_)->getDevice(its->second->deviceName);
            newEntryString = "S("+ofToString(its->first)+") => "+its->second->deviceName+":"+its->second->parameter.getName();
        }
        selectedElement.index = its->first;
        selectedElement.type = SLIDER;
        ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
        buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode_, selectTrack_, selectedDevice_, selectedElement, its->second->parameter.getName());
        guiView->autoSizeToFitWidgets();
    }
    for (; itb != buttonMap.end(); ++itb)
    {
        string newEntryString;
        int selectMode_, selectTrack_;
        selectTrack_ = itb->second->track;
        ofxAbletonLiveDevice *selectedDevice_ = NULL;
        if (selectTrack_ == -1) {
            selectMode_ = 0;
            selectedDevice_ = NULL;
            newEntryString = "B("+ofToString(itb->first)+") => Global "+itb->second->parameter.getName();
        }
        else if (itb->second->deviceName == "Track "+ofToString(selectTrack_)) {
            selectMode_ = 1;
            selectedDevice_ = NULL;
            newEntryString = "B("+ofToString(itb->first)+") => T"+ofToString(selectTrack_)+" "+itb->second->parameter.getName();
        }
        else {
            selectMode_ = 2;
            selectedDevice_ = live->getTrack(selectTrack_)->getDevice(itb->second->deviceName);
            newEntryString = "B("+ofToString(itb->first)+") => "+itb->second->deviceName+":"+itb->second->parameter.getName();
        }
        selectedElement.index = itb->first;
        selectedElement.type = BUTTON;
        ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
        buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode_, selectTrack_, selectedDevice_, selectedElement, itb->second->parameter.getName());
        guiView->autoSizeToFitWidgets();
    }
    for (; itsm != statMap.end(); ++itsm)
    {
        string newEntryString;
        int selectMode_, selectTrack_;
        selectTrack_ = itsm->second->track;
        ofxAbletonLiveDevice *selectedDevice_ = NULL;
        if (selectTrack_ == -1) {
            selectMode_ = 0;
            selectedDevice_ = NULL;
            newEntryString = getStatsInfo(selectedElement.index).name+") => Global "+itsm->second->parameter.getName();
        }
        else if (itsm->second->deviceName == "Track "+ofToString(selectTrack_)) {
            selectMode_ = 1;
            selectedDevice_ = NULL;
            newEntryString = getStatsInfo(selectedElement.index).name+") => T"+ofToString(selectTrack_)+" "+itsm->second->parameter.getName();
        }
        else {
            selectMode_ = 2;
            selectedDevice_ = live->getTrack(selectTrack_)->getDevice(itsm->second->deviceName);
            newEntryString = getStatsInfo(selectedElement.index).name+" => "+itsm->second->deviceName+":"+itsm->second->parameter.getName();
        }
        selectedElement.index = itsm->first;
        selectedElement.type = STAT;
        ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
        buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode_, selectTrack_, selectedDevice_, selectedElement, itsm->second->parameter.getName());
        guiView->autoSizeToFitWidgets();
    }
    ((ofxUIIntSlider *) guiMidi->getWidget("Key"))->setValue(getKey());
    ((ofxUIIntSlider *) guiMidi->getWidget("Octave"))->setValue(getOctave());
    ((ofxUIDropDownList *) guiMidi->getWidget("Mode"))->getToggles()[getMode()]->setValue(true);
    resetKeyString();
    guiView->autoSizeToFitWidgets();
}

void MantaAbletonControllerUI::checkSelectedPair()
{
    ofPoint range(0, 1);
    if (selectMode == 0)
    {
        if (guiSelectedParameter->getTextString() == "Global tempo") {
            range.set(live->getTempo().getMin(), live->getTempo().getMax());
        }
        else if (guiSelectedParameter->getTextString() == "Global time") {
            range.set(live->getTime().getMin(), live->getTime().getMax());
        }
        else if (guiSelectedParameter->getTextString() == "Global volume") {
            range.set(live->getVolume().getMin(), live->getVolume().getMax());
        }
        else if (guiSelectedParameter->getTextString() == "Global pan") {
            range.set(live->getPan().getMin(), live->getPan().getMax());
        }
        else if (guiSelectedParameter->getTextString() == "Global crossfade") {
            range.set(live->getCrossFader().getMin(), live->getCrossFader().getMax());
        }
    }
    else if (selectMode == 1)
    {
        if (guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" volume") {
            range.set(live->getTrack(selectedTrack)->getVolume().getMin(), live->getTrack(selectedTrack)->getVolume().getMax());
        }
        else if (guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" pan") {
            range.set(live->getTrack(selectedTrack)->getPan().getMin(), live->getTrack(selectedTrack)->getPan().getMax());
        }
        else if (guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" send") {
            range.set(live->getTrack(selectedTrack)->getSend(0)->send.getMin(), live->getTrack(selectedTrack)->getSend(0)->send.getMax());
        }
    }
    else if (selectMode == 2)
    {
        ofxAbletonLiveParameter *p = parameterLU[make_pair(selectedDevice, guiSelectedParameter->getTextString())];
        range.set(p->getMin(), p->getMax());
    }

    guiMapper->newGUIEvent.disable();
    guiRange->setMin(range.x);
    guiRange->setMax(range.y);
    guiRange->setValueLow(range.x);
    guiRange->setValueHigh(range.y);
    guiToggle->setValue(false);
    
    if (selectedElement.type == PAD && padMap.count(selectedElement.index)) {
        if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+padMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+padMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 2 && guiSelectedParameter->getTextString() == padMap[selectedElement.index]->parameter.getName())) {
            guiRange->setValueLow(padMap[selectedElement.index]->min);
            guiRange->setValueHigh(padMap[selectedElement.index]->max);
            guiToggle->setValue(padMap[selectedElement.index]->toggle);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            guiMapper->newGUIEvent.enable();

        }
    }
    else if (selectedElement.type == SLIDER && sliderMap.count(selectedElement.index)) {
        if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+sliderMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+sliderMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 2 && guiSelectedParameter->getTextString() == sliderMap[selectedElement.index]->parameter.getName())) {
            guiRange->setValueLow(sliderMap[selectedElement.index]->min);
            guiRange->setValueHigh(sliderMap[selectedElement.index]->max);
            guiToggle->setValue(sliderMap[selectedElement.index]->toggle);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
        }
    }
    else if (selectedElement.type == BUTTON && buttonMap.count(selectedElement.index)) {
        if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+buttonMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+buttonMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 2 && guiSelectedParameter->getTextString() == buttonMap[selectedElement.index]->parameter.getName())) {
            guiRange->setValueLow(buttonMap[selectedElement.index]->min);
            guiRange->setValueHigh(buttonMap[selectedElement.index]->max);
            guiToggle->setValue(buttonMap[selectedElement.index]->toggle);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
        }
    }
    else if (selectedElement.type == STAT && statMap.count(selectedElement.index)) {
        if ((selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+statMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" "+statMap[selectedElement.index]->parameter.getName()) ||
            (selectMode == 2 && guiSelectedParameter->getTextString() == statMap[selectedElement.index]->parameter.getName())) {
            guiRange->setValueLow(statMap[selectedElement.index]->min);
            guiRange->setValueHigh(statMap[selectedElement.index]->max);
            guiToggle->setValue(statMap[selectedElement.index]->toggle);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
        }
    }

    guiMapper->newGUIEvent.enable();
    guiDelete->setVisible(false);
    guiMapper->autoSizeToFitWidgets();
}

void MantaAbletonControllerUI::addSelectedMapping()
{
    string newEntryString;
    if (selectedElement.type == NONE) {
        return;
    }
    
    ofParameter<float> *selectedParameter;
    if (selectMode == 0)
    {
        if (guiSelectedParameter->getTextString() == "Global tempo") {
            selectedParameter = &live->getTempo();
        }
        else if (guiSelectedParameter->getTextString() == "Global time") {
            selectedParameter = &live->getTime();
        }
        else if (guiSelectedParameter->getTextString() == "Global volume") {
            selectedParameter = &live->getVolume();
        }
        else if (guiSelectedParameter->getTextString() == "Global pan") {
            selectedParameter = &live->getPan();
        }
        else if (guiSelectedParameter->getTextString() == "Global crossfade") {
            selectedParameter = &live->getCrossFader();
        }
    }
    else if (selectMode == 1)
    {
        if (guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" volume") {
            selectedParameter = &live->getTrack(selectedTrack)->getVolume();
        }
        else if (guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" pan") {
            selectedParameter = &live->getTrack(selectedTrack)->getPan();
        }
        else if (guiSelectedParameter->getTextString() == "T"+ofToString(selectedTrack)+" send") {
            selectedParameter = &live->getTrack(selectedTrack)->getSend(0)->send;
        }
    }
    else if (selectMode == 2 && selectedDevice != NULL)
    {
        selectedParameter = selectedDevice->getParameter(guiSelectedParameter->getTextString())->getParameter();
    }
    

    if (selectedElement.type == PAD)
    {
        int row = floor(selectedElement.index / 8);
        int col = selectedElement.index % 8;

        if (selectMode == 0)
        {
            mapPadToParameter(row, col, -1, "Global", *selectedParameter, guiToggle->getValue());
            newEntryString = "P("+ofToString(row)+","+ofToString(col)+") => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 1)
        {
            mapPadToParameter(row, col, selectedTrack, "Track "+ofToString(selectedTrack), *selectedParameter, guiToggle->getValue());
            newEntryString = "P("+ofToString(row)+","+ofToString(col)+") => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 2 && selectedDevice != NULL)
        {
            mapPadToParameter(row, col, selectedTrack, selectedDevice->getName(), *selectedParameter, guiToggle->getValue());
            newEntryString = "P("+ofToString(row)+","+ofToString(col)+") => "+selectedDevice->getName()+":"+guiSelectedParameter->getTextString();
        }
        
        padMap[selectedElement.index]->min = guiRange->getValueLow();
        padMap[selectedElement.index]->max = guiRange->getValueHigh();
    }
    else if (selectedElement.type == SLIDER)
    {
        if (selectMode == 0)
        {
            mapSliderToParameter(selectedElement.index, -1, "Global", *selectedParameter);
            newEntryString = "S("+ofToString(selectedElement.index)+") => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 1)
        {
            mapSliderToParameter(selectedElement.index, selectedTrack, "Track "+ofToString(selectedTrack), *selectedParameter);
            newEntryString = "S("+ofToString(selectedElement.index)+") => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 2 && selectedDevice != NULL)
        {
            mapSliderToParameter(selectedElement.index, selectedTrack, selectedDevice->getName(), *selectedParameter);
            newEntryString = "S("+ofToString(selectedElement.index)+") => "+selectedDevice->getName()+":"+guiSelectedParameter->getTextString();
        }

        sliderMap[selectedElement.index]->min = guiRange->getValueLow();
        sliderMap[selectedElement.index]->max = guiRange->getValueHigh();
    }
    else if (selectedElement.type == BUTTON)
    {
        if (selectMode == 0)
        {
            mapButtonToParameter(selectedElement.index, -1, "Global", *selectedParameter, guiToggle->getValue());
            newEntryString = "B("+ofToString(selectedElement.index)+") => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 1)
        {
            mapButtonToParameter(selectedElement.index, selectedTrack, "Track "+ofToString(selectedTrack), *selectedParameter, guiToggle->getValue());
            newEntryString = "B("+ofToString(selectedElement.index)+") => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 2 && selectedDevice != NULL)
        {
            mapButtonToParameter(selectedElement.index, selectedTrack, selectedDevice->getName(), *selectedParameter, guiToggle->getValue());
            newEntryString = "B("+ofToString(selectedElement.index)+") => "+selectedDevice->getName()+":"+guiSelectedParameter->getTextString();
        }
        
        buttonMap[selectedElement.index]->min = guiRange->getValueLow();
        buttonMap[selectedElement.index]->max = guiRange->getValueHigh();
    }
    else if (selectedElement.type == STAT && selectedDevice != NULL)
    {
        if (selectMode == 0)
        {
            mapStatToParameter(selectedElement.index, -1, "Global", *selectedParameter);
            newEntryString = getStatsInfo(selectedElement.index).name+" => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 1)
        {
            mapStatToParameter(selectedElement.index, -1, "Track "+ofToString(selectedTrack), *selectedParameter);
            newEntryString = getStatsInfo(selectedElement.index).name+" => "+guiSelectedParameter->getTextString();
        }
        else if (selectMode == 2 && selectedDevice != NULL)
        {
            mapStatToParameter(selectedElement.index, selectedTrack, selectedDevice->getName(), *selectedParameter);
            newEntryString = getStatsInfo(selectedElement.index).name+" => "+selectedDevice->getName()+":"+guiSelectedParameter->getTextString();
        }
        
        statMap[selectedElement.index]->min = guiRange->getValueLow();
        statMap[selectedElement.index]->max = guiRange->getValueHigh();
    }
    
    ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
    buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode, selectedTrack, selectedDevice, selectedElement, selectedParameter->getName());
    guiDelete->setVisible(true);
    guiMapper->autoSizeToFitWidgets();
    guiView->autoSizeToFitWidgets();
    selectedParameterToggle->setColorBack(ofColor(190, 5, 5));
}

void MantaAbletonControllerUI::removeSelectedMapping()
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
    else if (selectedElement.type == STAT) {
        removeStatMapping(selectedElement.index);
    }
    
    guiDelete->setVisible(false);
    guiView->removeWidget(selectedViewMappingToggle);
    guiView->autoSizeToFitWidgets();
    guiView->stateChange();
    selectedParameterToggle->setColorBack(OFX_UI_COLOR_BACK);
}

MantaAbletonControllerUI::~MantaAbletonControllerUI()
{
    ofRemoveListener(eventPadClick, this, &MantaAbletonControllerUI::eventMantaPadClick);
    ofRemoveListener(eventSliderClick, this, &MantaAbletonControllerUI::eventMantaSliderClick);
    ofRemoveListener(eventButtonClick, this, &MantaAbletonControllerUI::eventMantaButtonClick);
    ofRemoveListener(eventStatClick, this, &MantaAbletonControllerUI::eventMantaStatClick);
    
    parameterLU.clear();
    groupDeviceLU.clear();
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