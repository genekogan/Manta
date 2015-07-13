#include "MantaAbletonController.h"

MantaAbletonController::MantaParameterMapping::MantaParameterMapping(ofParameter<float> & parameter_, int track, string deviceName, bool toggle)
{
    this->parameter.makeReferenceTo(parameter_);
    this->track = track;
    this->deviceName = deviceName;
    this->min = parameter.getMin();
    this->max = parameter.getMax();
    this->toggle = toggle;
    if (!toggle) {
        parameter = max;
        toggleHighLow();
    }
}

MantaAbletonController::MantaAbletonController() : MantaStats()
{
    toSetMidiLedColor = true;
    
    midi.openPort(0);
    //midi.openPort("IAC Driver Pure Data In"); // by name
    //midi.openVirtualPort("ofxMidiOut"); // open a virtual port
    
    addPadListener(this, &MantaAbletonController::PadEvent);
    addSliderListener(this, &MantaAbletonController::SliderEvent);
    addButtonListener(this, &MantaAbletonController::ButtonEvent);
    addPadVelocityListener(this, &MantaAbletonController::PadVelocityEvent);
    addButtonVelocityListener(this, &MantaAbletonController::ButtonVelocityEvent);
    addStatListener(this, &MantaAbletonController::StatEvent);
    
    setPadFreezingEnabled(false);
    for (int i = 0; i < 48; i++) {
        padFrozen[i] = false;
        padReleased[i] = false;
    }
    
    setupTheory();
    setKey(0);
    setMode(0);
    setOctave(5);
}

void MantaAbletonController::setup(ofxAbletonLive & live_)
{
    this->live = &live_;
    live->setup(this, &MantaAbletonController::abletonLoaded);
    MantaStats::setup();
}

void MantaAbletonController::update()
{
    MantaStats::update();
    live->update();
}

void MantaAbletonController::mapPadToParameter(int row, int column, int track, string deviceName, ofParameter<float> & parameter, bool toggle)
{
    padMap[row * 8 + column] = new MantaParameterMapping(parameter, track, deviceName, toggle);
    updatePadColor(row, column);
}

void MantaAbletonController::mapSliderToParameter(int index, int track, string deviceName, ofParameter<float> & parameter)
{
    sliderMap[index] = new MantaParameterMapping(parameter, track, deviceName);
    setSliderColor(index, ofColor::red);
}

void MantaAbletonController::mapButtonToParameter(int index, int track, string deviceName, ofParameter<float> & parameter, bool toggle)
{
    buttonMap[index] = new MantaParameterMapping(parameter, track, deviceName, toggle);
    updateButtonColor(index);
}

void MantaAbletonController::mapStatToParameter(int index, int track, string deviceName, ofParameter<float> & parameter)
{
    statMap[index] = new MantaParameterMapping(parameter, track, deviceName);
    setStatsColor(index, ofColor::red);
}

void MantaAbletonController::removePadMapping(int row, int column)
{
    delete padMap[row * 8 + column];
    padMap.erase(row * 8 + column);
    updatePadColor(row, column);
}

void MantaAbletonController::removeSliderMapping(int index)
{
    delete sliderMap[index];
    sliderMap.erase(index);
    setSliderColor(index, ofColor::white);
}

void MantaAbletonController::removeButtonMapping(int index)
{
    delete buttonMap[index];
    buttonMap.erase(index);
    updateButtonColor(index);
}

void MantaAbletonController::removeStatMapping(int index)
{
    delete statMap[index];
    statMap.erase(index);
    setStatsColor(index, ofColor::white);
}

void MantaAbletonController::mapSelectionToMidiNotes(int channel)
{
    vector<int> selection = getPadSelection();
    for (auto s : selection) {
        setMidiMapping(s, channel);
    }
}

void MantaAbletonController::mapAllPadsToMidiNotes(int channel)
{
    clearMidiMapping();
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 8; c++) {
            addPadToSelection(r, c);
        }
    }
    mapSelectionToMidiNotes(channel);
}

void MantaAbletonController::clearMidiMapping()
{
    midiMap.clear();
    markAllPads(Manta::Off);
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 8; c++) {
            setPadColor(r, c, ofColor::white);
        }
    }
}

void MantaAbletonController::resetMidiMapping()
{
    map<int, AudioUnitNotePair>::iterator it = midiMap.begin();
    for (; it != midiMap.end(); ++it) {
        setMidiMapping(it->first, it->second.channel);
    }
}

void MantaAbletonController::setMidiMapping(int idx, int channel)
{
    int row = floor(idx / 8);
    int col = idx % 8;
    int degree = (2 * row - (int)(row / 2) + col) % 7;
    int octave1 = octave + floor((2 * row - floor(row / 2) + col) / 7);
    midiMap[idx].channel = channel;
    midiMap[idx].note = getNoteAtScaleDegree(key, degree, mode, octave1);
    
    if      (channel == 1)  setPadColor(row, col, ofColor::red);
    else if (channel == 2)  setPadColor(row, col, ofColor::green);
    else if (channel == 3)  setPadColor(row, col, ofColor::yellow);
    else if (channel == 4)  setPadColor(row, col, ofColor::cyan);
    else if (channel == 5)  setPadColor(row, col, ofColor::orange);
    else if (channel == 6)  setPadColor(row, col, ofColor::aqua);
    else if (channel == 7)  setPadColor(row, col, ofColor::purple);
    else if (channel == 8)  setPadColor(row, col, ofColor::magenta);
    else if (channel == 9)  setPadColor(row, col, ofColor::gray);
    else if (channel ==10)  setPadColor(row, col, ofColor::brown);
    else if (channel ==11)  setPadColor(row, col, ofColor::turquoise);
    else if (channel ==12)  setPadColor(row, col, ofColor::tan);
    else if (channel ==13)  setPadColor(row, col, ofColor::lavender);
    else if (channel ==14)  setPadColor(row, col, ofColor::khaki);
    else if (channel ==15)  setPadColor(row, col, ofColor::gold);
    else if (channel ==16)  setPadColor(row, col, ofColor::chocolate);
    
    if (toSetMidiLedColor) {
        setPadLedState(row, col, Manta::Red);
    }
}

void MantaAbletonController::setKey(int key)
{
    this->key = key;
    resetMidiMapping();
}

void MantaAbletonController::setMode(int mode)
{
    this->mode = mode;
    resetMidiMapping();
}
void MantaAbletonController::setOctave(int octave)
{
    this->octave = octave;
    resetMidiMapping();
}

void MantaAbletonController::updatePadColor(int row, int column)
{
    if (padMap.count(row * 8 + column) == 0)
    {
        setPadLedState(row, column, Manta::Off);
        setPadColor(row, column, ofColor::white);
    }
    else if (padMap[row * 8 + column]->toggle)
    {
        ofColor synthColor = ofColor::blue;
        if (padMap[row * 8 + column]->parameter == padMap[row * 8 + column]->min)
        {
            setPadColor(row, column, ofColor(synthColor, 100));
            setPadLedState(row, column, Manta::Off);
        }
        else
        {
            setPadColor(row, column, synthColor);
            setPadLedState(row, column, Manta::Red);
        }
    }
    else
    {
        ofColor synthColor = ofColor::blue;
        setPadLedState(row, column, Manta::Red);
        setPadColor(row, column, synthColor);
    }
}

void MantaAbletonController::updateButtonColor(int index)
{
    if (buttonMap.count(index) == 0)
    {
        setButtonLedState(index, Manta::Off);
        setButtonColor(index, ofColor::white);
    }
    else if (buttonMap[index]->toggle)
    {
        ofColor synthColor = ofColor::blue;
        if (buttonMap[index]->parameter == buttonMap[index]->min)
        {
            setButtonColor(index, ofColor(synthColor, 100));
            setButtonLedState(index, Manta::Off);
        }
        else
        {
            setButtonColor(index, synthColor);
            setButtonLedState(index, Manta::Red);
        }
    }
    else
    {
        ofColor synthColor = ofColor::blue;
        setButtonLedState(index, Manta::Red);
        setButtonColor(index, synthColor);
    }
}

void MantaAbletonController::freezePads()
{
    for (int r = 0; r < 6; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (getPad(r, c) > 0)
            {
                padFrozen[c + 8 * r] = true;
                padReleased[c + 8 * r] = false;
            }
        }
    }
}

void MantaAbletonController::setPadFreezingEnabled(bool toFreezePads)
{
    this->toFreezePads = toFreezePads;
    setButtonLedState(1, toFreezePads ? Manta::Red : Manta::Off);
    setButtonColor(1, toFreezePads ? ofColor::blue : ofColor::white);
}

void MantaAbletonController::PadEvent(ofxMantaEvent & evt)
{
    if (padMap.count(evt.id) != 0)
    {
        if (!padFrozen[evt.id] && !padMap[evt.id]->toggle) {
            padMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_PAD_VALUE, padMap[evt.id]->min, padMap[evt.id]->max));
        }
        else if (padReleased[evt.id]) {
            padFrozen[evt.id] = false;
        }
    }
}

void MantaAbletonController::SliderEvent(ofxMantaEvent & evt)
{
    if (sliderMap.count(evt.id) != 0 && evt.value > -1) {
        sliderMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_SLIDER_VALUE, sliderMap[evt.id]->min, sliderMap[evt.id]->max));
    }
}

void MantaAbletonController::ButtonEvent(ofxMantaEvent & evt)
{
    if (buttonMap.count(evt.id) != 0 && !buttonMap[evt.id]->toggle) {
        buttonMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_BUTTON_VALUE, buttonMap[evt.id]->min, buttonMap[evt.id]->max));
    }
}

void MantaAbletonController::StatEvent(MantaStatsArgs & evt)
{
    if (statMap.count(evt.index) != 0) {
        statMap[evt.index]->parameter.set(ofMap(evt.value, getStatsInfo(evt.index).min, getStatsInfo(evt.index).max, statMap[evt.index]->min, statMap[evt.index]->max));
    }
}

void MantaAbletonController::PadVelocityEvent(ofxMantaEvent & evt)
{
    if (evt.value == 0) padReleased[evt.id] = true;
    if (midiMap.count(evt.id) != 0)
    {
        if (evt.value == 0) {
            midi.sendNoteOff(midiMap[evt.id].channel, midiMap[evt.id].note);
        }
        else {
            midi.sendNoteOn(midiMap[evt.id].channel, midiMap[evt.id].note, evt.value);
        }
    }
    else if (padMap.count(evt.id) != 0 && padMap[evt.id]->toggle && evt.value > 0)
    {
        padMap[evt.id]->toggleHighLow();
        updatePadColor(floor(evt.id / 8), evt.id % 8);
    }
}

void MantaAbletonController::ButtonVelocityEvent(ofxMantaEvent & evt)
{
    if (buttonMap.count(evt.id) != 0 && buttonMap[evt.id]->toggle && evt.value > 0)
    {
        buttonMap[evt.id]->toggleHighLow();
        updateButtonColor(evt.id);
    }
    else if (toFreezePads && evt.id==1 && evt.value==0) {
        freezePads();
    }
}

void MantaAbletonController::setupTheory()
{
    int major_[7]  = {0, 2, 4, 5, 7, 9, 11};
    int minorN_[7] = {0, 2, 3, 5, 7, 8, 10};
    int minorH_[7] = {0, 2, 3, 5, 7, 8, 11};
    int minorM_[7] = {0, 2, 3, 5, 7, 9, 11};
    for (auto m : major_)   major.push_back(m);
    for (auto m : minorN_)  minorN.push_back(m);
    for (auto m : minorH_)  minorH.push_back(m);
    for (auto m : minorM_)  minorM.push_back(m);
}

void MantaAbletonController::getChord(int chord[], int root, int octave)
{
    for (int i=0; i<3; i++) {
        chord[i] = (octave + floor((root + 2 * i) / 7)) * 12 + major[(root + 2 * i) % 7];
    }
}

int MantaAbletonController::getNoteAtScaleDegree(int root, int degree, int mode, int octave)
{
    if      (mode  < 7) {
        return (octave + floor((mode + degree) / 7)) * 12 + root + major[(mode + degree) % 7] - major[mode];
    }
    else if (mode == 7) {
        return (octave + floor(degree / 7)) * 12 + root + minorN[degree % 7];
    }
    else if (mode == 8) {
        return (octave + floor(degree / 7)) * 12 + root + minorH[degree % 7];
    }
    else if (mode == 9) {
        return (octave + floor(degree / 7)) * 12 + root + minorM[degree % 7];
    }
}

void MantaAbletonController::savePreset(string name)
{
    ofXml xml;
    xml.addChild("MantaAbletonController");
    xml.setTo("MantaAbletonController");

    
    
    string pathToAls = "/this/is/the/path/to/ALS";
    
    // locate correct als
    xml.addChild("Ableton");
    xml.setTo("Ableton");
    xml.addValue("ALS", pathToAls);
    xml.setToParent();
    
    
    
    
    
    xml.addChild("Manta");
    xml.setTo("Manta");
    xml.addValue("Key", key);
    xml.addValue("Mode", mode);
    xml.addValue("Octave", octave);
    xml.addValue("FreezeEnabled", toFreezePads);
    
    xml.addChild("Pads");
    xml.setTo("Pads");
    
    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    for (; itp != padMap.end(); ++itp) {
        ofXml xml_;
        xml_.addChild("PadMapping");
        xml_.setTo("PadMapping");
        xml_.addValue("Id", itp->first);
        xml_.addValue("TrackIndex", itp->second->track);
        xml_.addValue("DeviceName", itp->second->deviceName);
        xml_.addValue("ParameterName", itp->second->parameter.getName());
        xml_.addValue("Min", itp->second->min);
        xml_.addValue("Max", itp->second->max);
        xml_.addValue("Toggle", itp->second->toggle);
        xml.addXml(xml_);
    }
    xml.setToParent();

    xml.addChild("Sliders");
    xml.setTo("Sliders");

    map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
    for (; its != sliderMap.end(); ++its) {
        ofXml xml_;
        xml_.addChild("SliderMapping");
        xml_.setTo("SliderMapping");
        xml_.addValue("Id", its->first);
        xml_.addValue("TrackIndex", itp->second->track);
        xml_.addValue("DeviceName", its->second->deviceName);
        xml_.addValue("ParameterName", its->second->parameter.getName());
        xml_.addValue("Min", its->second->min);
        xml_.addValue("Max", its->second->max);
        xml.addXml(xml_);
    }
    xml.setToParent();
    
    xml.addChild("Buttons");
    xml.setTo("Buttons");
    map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
    for (; itb != buttonMap.end(); ++itb) {
        ofXml xml_;
        xml_.addChild("ButtonMapping");
        xml_.setTo("ButtonMapping");
        xml_.addValue("Id", itb->first);
        xml_.addValue("TrackIndex", itp->second->track);
        xml_.addValue("DeviceName", itb->second->deviceName);
        xml_.addValue("ParameterName", itb->second->parameter.getName());
        xml_.addValue("Min", itb->second->min);
        xml_.addValue("Max", itb->second->max);
        xml_.addValue("Toggle", itb->second->toggle);
        xml.addXml(xml_);
    }
    xml.setToParent();
    
    xml.addChild("Stats");
    xml.setTo("Stats");
    map<int, MantaParameterMapping*>::iterator itsm = statMap.begin();
    for (; itsm != statMap.end(); ++itsm) {
        ofXml xml_;
        xml_.addChild("StatMapping");
        xml_.setTo("StatMapping");
        xml_.addValue("Id", itsm->first);
        xml_.addValue("TrackIndex", itp->second->track);
        xml_.addValue("DeviceName", itsm->second->deviceName);
        xml_.addValue("ParameterName", itsm->second->parameter.getName());
        xml_.addValue("Min", itsm->second->min);
        xml_.addValue("Max", itsm->second->max);
        xml.addXml(xml_);
    }
    xml.setToParent();
    
    xml.addChild("MidiMap");
    xml.setTo("MidiMap");
    map<int, AudioUnitNotePair>::iterator itm = midiMap.begin();
    for (; itm != midiMap.end(); ++itm) {
        ofXml xml_;
        xml_.addChild("MidiMapping");
        xml_.setTo("MidiMapping");
        xml_.addValue("Id", itm->first);
        xml_.addValue("Channel", itm->second.channel);
        xml_.addValue("Note", itm->second.note);
        xml.addXml(xml_);
    }
    xml.setToParent();
    
    xml.setToParent();
    
    xml.save(ofToDataPath("presets/"+name+".xml"));
}

void MantaAbletonController::loadPreset(string name)
{
    ofXml xml;
    xml.load(ofToString("presets/"+name+".xml"));
    xml.setTo("MantaAbletonController");

    xml.setTo("Ableton");

    //
    //
    //
    //
    // LOAD ALS FILE
    //
    //
    //
    
    
    
    xml.setToParent();
    
    xml.setTo("Manta");
    
    setKey(xml.getValue<int>("Key"));
    setMode(xml.getValue<int>("Mode"));
    setOctave(xml.getValue<int>("Octave"));
    setPadFreezingEnabled(xml.getValue<int>("FreezeEnabled") == 1 ? true : false);

    xml.setTo("Pads");
    if (xml.exists("PadMapping[0]")) {
        xml.setTo("PadMapping[0]");
        do {
            int id = xml.getValue<int>("Id");
            int track = xml.getValue<int>("TrackIndex");
            string deviceName = xml.getValue<string>("DeviceName");
            string parameterName = xml.getValue<string>("ParameterName");
            bool toggle = xml.getValue<int>("Toggle") == 1 ? true : false;
            ofParameter<float> *p = live->getTrack(track)->getDevice(deviceName)->getParameter(parameterName)->getParameter();
            mapPadToParameter(floor(id / 8), id % 8, track, deviceName, *p);
            padMap[id]->min = xml.getValue<float>("Min");
            padMap[id]->max = xml.getValue<float>("Max");
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();
    
    xml.setTo("Sliders");
    if (xml.exists("SliderMapping[0]")) {
        xml.setTo("SliderMapping[0]");
        do {
            int id = xml.getValue<int>("Id");
            int track = xml.getValue<int>("TrackIndex");
            string deviceName = xml.getValue<string>("DeviceName");
            string parameterName = xml.getValue<string>("ParameterName");
            ofParameter<float> *p = live->getTrack(track)->getDevice(deviceName)->getParameter(parameterName)->getParameter();
            mapSliderToParameter(id, track, deviceName, *p);
            sliderMap[id]->min = xml.getValue<float>("Min");
            sliderMap[id]->max = xml.getValue<float>("Max");
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();

    xml.setTo("Buttons");
    if (xml.exists("ButtonMapping[0]")) {
        xml.setTo("ButtonMapping[0]");
        do {
            int id = xml.getValue<int>("Id");
            int track = xml.getValue<int>("TrackIndex");
            string deviceName = xml.getValue<string>("DeviceName");
            string parameterName = xml.getValue<string>("ParameterName");
            bool toggle = xml.getValue<int>("Toggle") == 1 ? true : false;
            ofParameter<float> *p = live->getTrack(track)->getDevice(deviceName)->getParameter(parameterName)->getParameter();
            mapButtonToParameter(id, track, deviceName, *p);
            buttonMap[id]->min = xml.getValue<float>("Min");
            buttonMap[id]->max = xml.getValue<float>("Max");
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();

    xml.setTo("Stats");
    if (xml.exists("StatMapping[0]")) {
        xml.setTo("StatMapping[0]");
        do {
            int id = xml.getValue<int>("Id");
            int track = xml.getValue<int>("TrackIndex");
            string deviceName = xml.getValue<string>("DeviceName");
            string parameterName = xml.getValue<string>("ParameterName");
            ofParameter<float> *p = live->getTrack(track)->getDevice(deviceName)->getParameter(parameterName)->getParameter();
            mapStatToParameter(id, track, deviceName, *p);
            statMap[id]->min = xml.getValue<float>("Min");
            statMap[id]->max = xml.getValue<float>("Max");
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();

    xml.setTo("MidiMap");
    if (xml.exists("MidiMapping[0]")) {
        clearMidiMapping();
        xml.setTo("MidiMapping[0]");
        do {
            setMidiMapping(xml.getValue<int>("Id"), xml.getValue<int>("Channel"));
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    
    xml.setToParent();
}

MantaAbletonController::~MantaAbletonController()
{
    removePadListener(this, &MantaAbletonController::PadEvent);
    removeSliderListener(this, &MantaAbletonController::SliderEvent);
    removeButtonListener(this, &MantaAbletonController::ButtonEvent);
    removePadVelocityListener(this, &MantaAbletonController::PadVelocityEvent);
    removeButtonVelocityListener(this, &MantaAbletonController::ButtonVelocityEvent);
    removeStatListener(this, &MantaAbletonController::StatEvent);
    
    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
    map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
    map<int, MantaParameterMapping*>::iterator itsm = statMap.begin();
    while (itp != padMap.end()) {
        delete itp->second;
        padMap.erase(itp);
    }
    while (its != sliderMap.end()) {
        delete its->second;
        sliderMap.erase(its);
    }
    while (itb != buttonMap.end()) {
        delete itb->second;
        buttonMap.erase(itb);
    }
    while (itsm != statMap.end()) {
        delete itsm->second;
        statMap.erase(itsm);
    }
    padMap.clear();
    sliderMap.clear();
    buttonMap.clear();
    statMap.clear();
}