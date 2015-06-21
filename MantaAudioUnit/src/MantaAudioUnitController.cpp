#include "MantaAudioUnitController.h"

MantaAudioUnitController::MantaParameterMapping::MantaParameterMapping(AudioUnitInstrument & synth, string parameterName)
{
    this->synthName = synth.getName();
    this->parameter.makeReferenceTo(synth.getParameter(parameterName));
    this->min = parameter.getMin();
    this->max = parameter.getMax();
}

MantaAudioUnitController::MantaAudioUnitController() : MantaStats()
{
    setupTheory();
    addPadListener(this, &MantaAudioUnitController::PadEvent);
    addSliderListener(this, &MantaAudioUnitController::SliderEvent);
    addPadListener(this, &MantaAudioUnitController::ButtonEvent);
    addPadVelocityListener(this, &MantaAudioUnitController::PadVelocityEvent);
    addButtonVelocityListener(this, &MantaAudioUnitController::ButtonVelocityEvent);
}

void MantaAudioUnitController::registerAudioUnit(AudioUnitInstrument & synth)
{
    if (synths.count(synth.getName()) == 0) {
        synths[synth.getName()] = &synth;
    }
}

void MantaAudioUnitController::mapPadToParameter(int row, int column, AudioUnitInstrument & synth, string parameterName)
{
    registerAudioUnit(synth);
    padMap[row * 8 + column] = new MantaParameterMapping(synth, parameterName);
    setPadLedState(row, column, Manta::Red);
}

void MantaAudioUnitController::mapSliderToParameter(int index, AudioUnitInstrument & synth, string parameterName)
{
    registerAudioUnit(synth);
    sliderMap[index] = new MantaParameterMapping(synth, parameterName);
}

void MantaAudioUnitController::mapButtonToParameter(int index, AudioUnitInstrument & synth, string parameterName)
{
    registerAudioUnit(synth);
    buttonMap[index] = new MantaParameterMapping(synth, parameterName);
}

void MantaAudioUnitController::mapSelectionToMidiNotes(AudioUnitInstrument & synth)
{
    //clearMidiMapping();
    vector<int> selection = getPadSelection();
    for (auto s : selection) {
        setMidiMapping(s, &synth);
    }
}

void MantaAudioUnitController::mapAllPadsToMidiNotes(AudioUnitInstrument & synth)
{
    clearMidiMapping();
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 8; c++) {
            addPadToSelection(r, c);
        }
    }
    mapSelectionToMidiNotes(synth);
}

void MantaAudioUnitController::clearMidiMapping()
{
    midiMap.clear();
    markAllPads(Manta::Off);
}

void MantaAudioUnitController::resetMidiMapping()
{
    map<int, AudioUnitNotePair>::iterator it = midiMap.begin();
    for (; it != midiMap.end(); ++it) {
        setMidiMapping(it->first, it->second.synth);
    }
}

void MantaAudioUnitController::setMidiMapping(int idx, AudioUnitInstrument * synth)
{
    int row = floor(idx / 8);
    int col = idx % 8;
    int degree = (2 * row - (int)(row / 2) + col) % 7;
    int octave1 = octave + floor((2 * row - floor(row / 2) + col) / 7);
    midiMap[idx].synth = synth;
    midiMap[idx].note = getNoteAtScaleDegree(key, degree, mode, octave1);
    setLedManual(true);
    setPadLedState(row, col, Manta::Red);
}

void MantaAudioUnitController::setKey(int key)
{
    this->key = key;
    resetMidiMapping();
}

void MantaAudioUnitController::setMode(int mode)
{
    this->mode = mode;
    resetMidiMapping();
}

void MantaAudioUnitController::PadEvent(ofxMantaEvent & evt)
{
    if (padMap.count(evt.id) != 0) {
        padMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_PAD_VALUE, padMap[evt.id]->min, padMap[evt.id]->max));
    }
}

void MantaAudioUnitController::SliderEvent(ofxMantaEvent & evt)
{
    if (sliderMap.count(evt.id) != 0 && evt.value > -1) {
        sliderMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_SLIDER_VALUE, sliderMap[evt.id]->min, sliderMap[evt.id]->max));
    }
}

void MantaAudioUnitController::ButtonEvent(ofxMantaEvent & evt)
{
    if (buttonMap.count(evt.id) != 0) {
        buttonMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_BUTTON_VALUE, buttonMap[evt.id]->min, buttonMap[evt.id]->max));
    }
}

void MantaAudioUnitController::PadVelocityEvent(ofxMantaEvent & evt)
{
    if (midiMap.count(evt.id) != 0)
    {
        if (evt.value == 0) {
            midiMap[evt.id].noteOff(evt.value);
        }
        else {
            midiMap[evt.id].noteOn(evt.value);
        }
    }
}

void MantaAudioUnitController::ButtonVelocityEvent(ofxMantaEvent & evt)
{
    
}

void MantaAudioUnitController::setupTheory()
{
    int major_[7]  = {0, 2, 4, 5, 7, 9, 11};
    int minorN_[7] = {0, 2, 3, 5, 7, 8, 10};
    int minorH_[7] = {0, 2, 3, 5, 7, 8, 11};
    int minorM_[7] = {0, 2, 3, 5, 7, 9, 11};
    for (auto m : major_)   major.push_back(m);
    for (auto m : minorN_)  minorN.push_back(m);
    for (auto m : minorH_)  minorH.push_back(m);
    for (auto m : minorM_)  minorM.push_back(m);
    key = 0;
    mode = 0;
    octave = 5;
}

void MantaAudioUnitController::getChord(int chord[], int root, int octave)
{
    for (int i=0; i<3; i++) {
        chord[i] = (octave + floor((root + 2 * i) / 7)) * 12 + major[(root + 2 * i) % 7];
    }
}

int MantaAudioUnitController::getNoteAtScaleDegree(int root, int degree, int mode, int octave)
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

void MantaAudioUnitController::savePreset(string name)
{
    ofXml xml;
    xml.addChild("MantaAudioUnitController");
    xml.setTo("MantaAudioUnitController");

    xml.addChild("AudioUnits");
    xml.setTo("AudioUnits");
    map<string, AudioUnitInstrument*>::iterator ita = synths.begin();
    for (; ita != synths.end(); ++ita) {
        ofXml xml_;
        xml_.addChild("AudioUnit");
        xml_.setTo("AudioUnit");
        xml_.addValue("Name", ita->second->getName());
        xml_.addValue("Preset", name+".aupreset");
        xml.addXml(xml_);
        synths[ita->second->getName()]->getSynth().saveCustomPresetAtPath(ofToDataPath(name+".aupreset"));
    }
    xml.setToParent();
    
    xml.addChild("Manta");
    xml.setTo("Manta");
    xml.addValue("Key", key);
    xml.addValue("Mode", mode);
    
    xml.addChild("Pads");
    xml.setTo("Pads");
    
    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    for (; itp != padMap.end(); ++itp) {
        ofXml xml_;
        xml_.addChild("PadMapping");
        xml_.setTo("PadMapping");
        xml_.addValue("Id", itp->first);
        xml_.addValue("SynthName", itp->second->synthName);
        xml_.addValue("ParameterName", itp->second->parameter.getName());
        xml_.addValue("Min", itp->second->min);
        xml_.addValue("Max", itp->second->max);
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
        xml_.addValue("SynthName", its->second->synthName);
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
        xml_.addValue("SynthName", itb->second->synthName);
        xml_.addValue("ParameterName", itb->second->parameter.getName());
        xml_.addValue("Min", itb->second->min);
        xml_.addValue("Max", itb->second->max);
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
        xml_.addValue("SynthName", itm->second.synth->getName());
        xml_.addValue("Note", itm->second.note);
        xml.addXml(xml_);
    }
    xml.setToParent();
    
    xml.setToParent();
    
    xml.save(ofToDataPath(name+".xml"));
}

void MantaAudioUnitController::loadPreset(string name)
{
    ofXml xml;
    xml.load(ofToString(name+".xml"));
    xml.setTo("MantaAudioUnitController");

    xml.setTo("AudioUnits");
    if (xml.exists("AudioUnit[0]")) {
        xml.setTo("AudioUnit[0]");
        do {
            string synthName = xml.getValue<string>("Name");
            synths[synthName]->getSynth().loadCustomPreset(ofToDataPath(xml.getValue<string>("Preset")));
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();
    
    xml.setTo("Manta");
    
    setKey(xml.getValue<int>("Key"));
    setMode(xml.getValue<int>("Mode"));

    xml.setTo("Pads");
    if (xml.exists("PadMapping[0]")) {
        xml.setTo("PadMapping[0]");
        do {
            int id = xml.getValue<int>("Id");
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            float min = xml.getValue<float>("Min");
            float max = xml.getValue<float>("Max");
            int row = floor(id / 8);
            int col = id % 8;
            mapPadToParameter(row, col, *synths[synthName], parameterName);
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
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            float min = xml.getValue<float>("Min");
            float max = xml.getValue<float>("Max");
            mapSliderToParameter(id, *synths[synthName], parameterName);
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
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            float min = xml.getValue<float>("Min");
            float max = xml.getValue<float>("Max");
            mapButtonToParameter(id, *synths[synthName], parameterName);
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();

    xml.setTo("MidiMap");
    if (xml.exists("MidiMapping[0]")) {
        xml.setTo("MidiMapping[0]");
        do {
            setMidiMapping(xml.getValue<int>("Id"), synths[xml.getValue<string>("SynthName")]);
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
    xml.setToParent();
}

MantaAudioUnitController::~MantaAudioUnitController()
{
    removePadListener(this, &MantaAudioUnitController::PadEvent);
    removeButtonListener(this, &MantaAudioUnitController::SliderEvent);
    removePadListener(this, &MantaAudioUnitController::ButtonEvent);
    removePadVelocityListener(this, &MantaAudioUnitController::PadVelocityEvent);
    removeButtonVelocityListener(this, &MantaAudioUnitController::ButtonVelocityEvent);

    map<int, MantaParameterMapping*>::iterator itp = padMap.begin();
    map<int, MantaParameterMapping*>::iterator its = sliderMap.begin();
    map<int, MantaParameterMapping*>::iterator itb = buttonMap.begin();
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
    padMap.clear();
    sliderMap.clear();
    buttonMap.clear();
}