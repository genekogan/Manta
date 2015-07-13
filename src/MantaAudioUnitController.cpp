#include "MantaAudioUnitController.h"

MantaAudioUnitController::MantaParameterMapping::MantaParameterMapping(AudioUnitInstrument & synth, string parameterName, bool toggle)
{
    this->synthName = synth.getName();
    this->parameter.makeReferenceTo(synth.getParameter(parameterName));
    this->min = parameter.getMin();
    this->max = parameter.getMax();
    this->toggle = toggle;
    if (!toggle) {
        parameter = max;
        toggleHighLow();
    }
}

MantaAudioUnitController::MantaAudioUnitController() : MantaStats()
{
    toSetMidiLedColor = true;
    
    addPadListener(this, &MantaAudioUnitController::PadEvent);
    addSliderListener(this, &MantaAudioUnitController::SliderEvent);
    addButtonListener(this, &MantaAudioUnitController::ButtonEvent);
    addPadVelocityListener(this, &MantaAudioUnitController::PadVelocityEvent);
    addButtonVelocityListener(this, &MantaAudioUnitController::ButtonVelocityEvent);
    addStatListener(this, &MantaAudioUnitController::StatEvent);
    
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

void MantaAudioUnitController::registerAudioUnit(AudioUnitInstrument & synth)
{
    if (synths.count(synth.getName()) == 0)
    {
        synths[synth.getName()] = &synth;
        if      (synths.size() == 1) synths[synth.getName()]->setColor(ofColor::orange);
        else if (synths.size() == 2) synths[synth.getName()]->setColor(ofColor::blue);
        else if (synths.size() == 3) synths[synth.getName()]->setColor(ofColor::yellow);
        else if (synths.size() == 4) synths[synth.getName()]->setColor(ofColor::magenta);
        else if (synths.size() == 5) synths[synth.getName()]->setColor(ofColor::red);
        else if (synths.size() == 6) synths[synth.getName()]->setColor(ofColor::cyan);
        else if (synths.size() == 7) synths[synth.getName()]->setColor(ofColor::khaki);
        else if (synths.size() == 8) synths[synth.getName()]->setColor(ofColor::ivory);
    }
}

void MantaAudioUnitController::mapPadToParameter(int row, int column, AudioUnitInstrument & synth, string parameterName, bool toggle)
{
    registerAudioUnit(synth);
    padMap[row * 8 + column] = new MantaParameterMapping(synth, parameterName, toggle);
    updatePadColor(row, column);
}

void MantaAudioUnitController::mapSliderToParameter(int index, AudioUnitInstrument & synth, string parameterName)
{
    registerAudioUnit(synth);
    sliderMap[index] = new MantaParameterMapping(synth, parameterName);
    setSliderColor(index, synth.getColor());
}

void MantaAudioUnitController::mapButtonToParameter(int index, AudioUnitInstrument & synth, string parameterName, bool toggle)
{
    registerAudioUnit(synth);
    buttonMap[index] = new MantaParameterMapping(synth, parameterName, toggle);
    updateButtonColor(index);
}

void MantaAudioUnitController::mapStatToParameter(int index, AudioUnitInstrument & synth, string parameterName)
{
    registerAudioUnit(synth);
    statMap[index] = new MantaParameterMapping(synth, parameterName);
    setStatsColor(index, synth.getColor());
}

void MantaAudioUnitController::removePadMapping(int row, int column)
{
    delete padMap[row * 8 + column];
    padMap.erase(row * 8 + column);
    updatePadColor(row, column);
}

void MantaAudioUnitController::removeSliderMapping(int index)
{
    delete sliderMap[index];
    sliderMap.erase(index);
    setSliderColor(index, ofColor::white);
}

void MantaAudioUnitController::removeButtonMapping(int index)
{
    delete buttonMap[index];
    buttonMap.erase(index);
    updateButtonColor(index);
}

void MantaAudioUnitController::removeStatMapping(int index)
{
    delete statMap[index];
    statMap.erase(index);
    setStatsColor(index, ofColor::white);
}

void MantaAudioUnitController::mapSelectionToMidiNotes(AudioUnitInstrument & synth)
{
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
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 8; c++) {
            setPadColor(r, c, ofColor::white);
        }
    }
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
    setPadColor(row, col, synth->getColor());
    if (toSetMidiLedColor) {
        setPadLedState(row, col, Manta::Red);
    }
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

void MantaAudioUnitController::setOctave(int octave)
{
    this->octave = octave;
    resetMidiMapping();
}

void MantaAudioUnitController::updatePadColor(int row, int column)
{
    if (padMap.count(row * 8 + column) == 0)
    {
        setPadLedState(row, column, Manta::Off);
        setPadColor(row, column, ofColor::white);
    }
    else if (padMap[row * 8 + column]->toggle)
    {
        ofColor synthColor = synths[padMap[row * 8 + column]->synthName]->getColor();
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
        ofColor synthColor = synths[padMap[row * 8 + column]->synthName]->getColor();
        setPadLedState(row, column, Manta::Red);
        setPadColor(row, column, synthColor);
    }
}

void MantaAudioUnitController::updateButtonColor(int index)
{
    if (buttonMap.count(index) == 0)
    {
        setButtonLedState(index, Manta::Off);
        setButtonColor(index, ofColor::white);
    }
    else if (buttonMap[index]->toggle)
    {
        ofColor synthColor = synths[buttonMap[index]->synthName]->getColor();
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
        ofColor synthColor = synths[buttonMap[index]->synthName]->getColor();
        setButtonLedState(index, Manta::Red);
        setButtonColor(index, synthColor);
    }
}

void MantaAudioUnitController::freezePads()
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

void MantaAudioUnitController::setPadFreezingEnabled(bool toFreezePads)
{
    this->toFreezePads = toFreezePads;
    setButtonLedState(1, toFreezePads ? Manta::Red : Manta::Off);
    setButtonColor(1, toFreezePads ? ofColor::blue : ofColor::white);
}

void MantaAudioUnitController::PadEvent(ofxMantaEvent & evt)
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

void MantaAudioUnitController::SliderEvent(ofxMantaEvent & evt)
{
    if (sliderMap.count(evt.id) != 0 && evt.value > -1) {
        sliderMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_SLIDER_VALUE, sliderMap[evt.id]->min, sliderMap[evt.id]->max));
    }
}

void MantaAudioUnitController::ButtonEvent(ofxMantaEvent & evt)
{
    if (buttonMap.count(evt.id) != 0 && !buttonMap[evt.id]->toggle) {
        buttonMap[evt.id]->parameter.set(ofMap(evt.value, 0, MANTA_MAX_BUTTON_VALUE, buttonMap[evt.id]->min, buttonMap[evt.id]->max));
    }
}

void MantaAudioUnitController::StatEvent(MantaStatsArgs & evt)
{
    if (statMap.count(evt.index) != 0) {
        statMap[evt.index]->parameter.set(ofMap(evt.value, getStatsInfo(evt.index).min, getStatsInfo(evt.index).max, statMap[evt.index]->min, statMap[evt.index]->max));
    }
}

void MantaAudioUnitController::PadVelocityEvent(ofxMantaEvent & evt)
{
    if (evt.value == 0) padReleased[evt.id] = true;
    if (midiMap.count(evt.id) != 0)
    {
        if (evt.value == 0) {
            midiMap[evt.id].noteOff(evt.value);
        }
        else {
            midiMap[evt.id].noteOn(evt.value);
        }
    }
    else if (padMap.count(evt.id) != 0 && padMap[evt.id]->toggle && evt.value > 0)
    {
        padMap[evt.id]->toggleHighLow();
        updatePadColor(floor(evt.id / 8), evt.id % 8);
    }
}

void MantaAudioUnitController::ButtonVelocityEvent(ofxMantaEvent & evt)
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
        xml_.addValue("Preset", ita->first+"_"+name);
        xml.addXml(xml_);
        synths[ita->second->getName()]->getSynth().saveCustomPresetAtPath(ofToDataPath("presets/"+ita->first+"_"+name+".aupreset"));
    }
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
        xml_.addValue("SynthName", itp->second->synthName);
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
        xml_.addValue("SynthName", itsm->second->synthName);
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
        xml_.addValue("SynthName", itm->second.synth->getName());
        xml_.addValue("Note", itm->second.note);
        xml.addXml(xml_);
    }
    xml.setToParent();
    
    xml.setToParent();
    
    xml.save(ofToDataPath("presets/"+name+".xml"));
}

void MantaAudioUnitController::loadPreset(string name)
{
    ofXml xml;
    xml.load(ofToString("presets/"+name+".xml"));
    xml.setTo("MantaAudioUnitController");

    xml.setTo("AudioUnits");
    if (xml.exists("AudioUnit[0]")) {
        xml.setTo("AudioUnit[0]");
        do {
            string synthName = xml.getValue<string>("Name");
            synths[synthName]->getSynth().loadCustomPreset(ofToDataPath("presets/"+xml.getValue<string>("Preset")));
        }
        while(xml.setToSibling());
        xml.setToParent();
    }
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
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            bool toggle = xml.getValue<int>("Toggle") == 1 ? true : false;
            mapPadToParameter(floor(id / 8), id % 8, *synths[synthName], parameterName, toggle);
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
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            mapSliderToParameter(id, *synths[synthName], parameterName);
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
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            bool toggle = xml.getValue<int>("Toggle") == 1 ? true : false;
            mapButtonToParameter(id, *synths[synthName], parameterName, toggle);
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
            string synthName = xml.getValue<string>("SynthName");
            string parameterName = xml.getValue<string>("ParameterName");
            mapStatToParameter(id, *synths[synthName], parameterName);
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
    removeSliderListener(this, &MantaAudioUnitController::SliderEvent);
    removeButtonListener(this, &MantaAudioUnitController::ButtonEvent);
    removePadVelocityListener(this, &MantaAudioUnitController::PadVelocityEvent);
    removeButtonVelocityListener(this, &MantaAudioUnitController::ButtonVelocityEvent);
    removeStatListener(this, &MantaAudioUnitController::StatEvent);
    
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