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
}

void MantaAbletonControllerUI::abletonLoaded()
{
    setupUI();
}

void MantaAbletonControllerUI::setupUI()
{
    ofAddListener(eventPadClick, this, &MantaAbletonControllerUI::eventMantaPadClick);
    ofAddListener(eventSliderClick, this, &MantaAbletonControllerUI::eventMantaSliderClick);
    ofAddListener(eventButtonClick, this, &MantaAbletonControllerUI::eventMantaButtonClick);
    ofAddListener(eventStatClick, this, &MantaAbletonControllerUI::eventMantaStatClick);
    
    selectedElement.type = NONE;

    guiGroups->clearWidgets();
    guiGroups->removeWidgets();
    guiMidi->clearWidgets();
    guiMidi->removeWidgets();
    guiView->clearWidgets();
    guiView->removeWidgets();
    guiPresets->clearWidgets();
    guiPresets->removeWidgets();
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
    
    guiGroups->addLabel("View");
    guiGroups->addCanvas(guiView);
    guiGroups->addCanvas(guiPresets);
    
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
        
        for (; itp != padMap.end(); ++itp) {
            if ((selectMode == 2 && itp->second->deviceName == selectedDevice->getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(itp->second->track)+" "+itp->second->parameter.getName()) ||
                (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+itp->second->parameter.getName())) {
                itp->second->min = low;
                itp->second->max = high;
                itp->second->toggle = toggle;
                return;
            }
        }
        for (; its != sliderMap.end(); ++its) {
            if ((selectMode == 2 && its->second->deviceName == selectedDevice->getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(its->second->track)+" "+its->second->parameter.getName()) ||
                (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+its->second->parameter.getName())) {
                its->second->min = low;
                its->second->max = high;
                return;
            }
        }
        for (; itb != buttonMap.end(); ++itb) {
            if ((selectMode == 2 && itb->second->deviceName == selectedDevice->getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(itb->second->track)+" "+itb->second->parameter.getName()) ||
                (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+itb->second->parameter.getName())) {
                itb->second->min = low;
                itb->second->max = high;
                itb->second->toggle = toggle;
                return;
            }
        }
        for (; itsm != statMap.end(); ++itsm) {
            if ((selectMode == 2 && itsm->second->deviceName == selectedDevice->getName()) ||
                (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(itsm->second->track)+" "+itsm->second->parameter.getName()) ||
                (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+itsm->second->parameter.getName())) {
                itsm->second->min = low;
                itsm->second->max = high;
                return;
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
        savePreset(presetName);
        ((ofxUIDropDownList *) guiPresets->getWidget("Load Preset"))->addToggle(presetName);
        guiPresets->autoSizeToFitWidgets();
    }
    else if (e.getParentName() == "Load Preset")
    {
        loadPreset(e.getName());
        
        // system dialog -> press OK to do the rest
        // 1) scanLiveSet()
        // 2) when ready, do manta mapping as below....
        //
        //
        //
        //
        //
        //
        //
        
        
        
        map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
        map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
        map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
        map<int, MantaParameterMapping*>::iterator itsm = statMap.begin();
        for (; itp != padMap.end(); ++itp)
        {
            string newEntryString = "P("+ofToString(floor(itp->first / 8))+","+ofToString(itp->first % 8)+") => "+itp->second->deviceName+":"+itp->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedDevice = live->getTrack(itp->second->track)->getDevice(itp->second->deviceName);
            selectedElement.index = itp->first;
            selectedElement.type = PAD;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode, selectedTrack, selectedDevice, selectedElement, itp->second->parameter.getName());
        }
        for (; its != sliderMap.end(); ++its)
        {
            string newEntryString = "S("+ofToString(its->first)+") => "+its->second->deviceName+":"+its->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedDevice = live->getTrack(itp->second->track)->getDevice(itp->second->deviceName);
            selectedElement.index = its->first;
            selectedElement.type = SLIDER;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode, selectedTrack, selectedDevice, selectedElement, its->second->parameter.getName());
        }
        for (; itb != buttonMap.end(); ++itb)
        {
            string newEntryString = "B("+ofToString(itb->first)+") => "+itb->second->deviceName+":"+itb->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedDevice = live->getTrack(itp->second->track)->getDevice(itp->second->deviceName);
            selectedElement.index = itb->first;
            selectedElement.type = BUTTON;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode, selectedTrack, selectedDevice, selectedElement, itb->second->parameter.getName());
        }
        for (; itsm != statMap.end(); ++itsm)
        {
            string newEntryString = getStatsInfo(selectedElement.index).name+" => "+itsm->second->deviceName+":"+itsm->second->parameter.getName();
            ofxUIButton *newSummaryEntry = guiView->addButton(newEntryString, false);
            selectedDevice = live->getTrack(itp->second->track)->getDevice(itp->second->deviceName);
            selectedElement.index = itsm->first;
            selectedElement.type = STAT;
            buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode, selectedTrack, selectedDevice, selectedElement, itsm->second->parameter.getName());
        }
        ((ofxUIIntSlider *) guiMidi->getWidget("Key"))->setValue(getKey());
        ((ofxUIIntSlider *) guiMidi->getWidget("Octave"))->setValue(getOctave());
        ((ofxUIDropDownList *) guiMidi->getWidget("Mode"))->getToggles()[getMode()]->setValue(true);
        resetKeyString();
        guiView->autoSizeToFitWidgets();
    }
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
        guiRange->setMin(p->getMin());
        guiRange->setMax(p->getMax());
        guiRange->setValueLow(p->getMin());
        guiRange->setValueHigh(p->getMax());
        guiToggle->setValue(false);
    }
    
    guiRange->setMin(range.x);
    guiRange->setMax(range.y);
    guiRange->setValueLow(range.x);
    guiRange->setValueHigh(range.y);
    guiToggle->setValue(false);
    
    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
    map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
    map<int, MantaParameterMapping*>::iterator itsm = statMap.begin();

    for (; itp != padMap.end(); ++itp) {
        if ((selectMode == 2 && itp->second->deviceName == selectedDevice->getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(itp->second->track)+" "+itp->second->parameter.getName()) ||
            (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+itp->second->parameter.getName())) {
            guiRange->setValueLow(itp->second->min);
            guiRange->setValueHigh(itp->second->max);
            guiToggle->setValue(itp->second->toggle);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }
    for (; its != sliderMap.end(); ++its) {
        if ((selectMode == 2 && its->second->deviceName == selectedDevice->getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(its->second->track)+" "+its->second->parameter.getName()) ||
            (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+its->second->parameter.getName())) {
            guiRange->setValueLow(its->second->min);
            guiRange->setValueHigh(its->second->max);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }
    for (; itb != buttonMap.end(); ++itb) {
        if ((selectMode == 2 && itb->second->deviceName == selectedDevice->getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(itb->second->track)+" "+itb->second->parameter.getName()) ||
            (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+itb->second->parameter.getName())) {
            guiRange->setValueLow(itb->second->min);
            guiRange->setValueHigh(itb->second->max);
            guiToggle->setValue(itb->second->toggle);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }
    for (; itsm != statMap.end(); ++itsm) {
        if ((selectMode == 2 && itsm->second->deviceName == selectedDevice->getName()) ||
            (selectMode == 1 && guiSelectedParameter->getTextString() == "T"+ofToString(itsm->second->track)+" "+itsm->second->parameter.getName()) ||
            (selectMode == 0 && guiSelectedParameter->getTextString() == "Global "+itsm->second->parameter.getName())) {
            guiRange->setValueLow(itsm->second->min);
            guiRange->setValueHigh(itsm->second->max);
            guiDelete->setVisible(true);
            guiMapper->autoSizeToFitWidgets();
            return;
        }
    }
    
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
            mapPadToParameter(row, col, -1, "Track "+ofToString(selectedTrack), *selectedParameter, guiToggle->getValue());
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
            mapSliderToParameter(selectedElement.index, -1, "Track "+ofToString(selectedTrack), *selectedParameter);
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
            mapButtonToParameter(selectedElement.index, -1, "Track "+ofToString(selectedTrack), *selectedParameter, guiToggle->getValue());
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
    buttonMappingLU[newSummaryEntry] = new MantaParameterPairSelection(selectMode, selectedTrack, selectedDevice, selectedElement, guiSelectedParameter->getTextString());
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