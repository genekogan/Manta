#include "AudioUnitInstrument.h"


AudioUnitInstrumentParameter::AudioUnitInstrumentParameter(ofxAudioUnitSampler *synth, AudioUnitParameterInfo parameter, int index)
{
    this->synth = synth;
    this->index = index;
    value.set(parameter.name, parameter.defaultValue, parameter.minValue, parameter.maxValue);
    value.addListener(this, &AudioUnitInstrumentParameter::parameterChanged);
}

void AudioUnitInstrumentParameter::parameterChanged(float & v)
{
    synth->setParameter(index, 0, value);
}

void AudioUnitInstrumentParameter::setValue(float v)
{
    value.set(v);
}

void AudioUnitInstrument::setup(string name, OSType type, OSType subType, OSType manufacturer)
{
    this->name = name;
    this->type = type;
    this->subType = subType;
    this->manufacturer = manufacturer;
    synth = ofxAudioUnitSampler(type, subType, manufacturer);
    loadParameterGroups();
    AUEventListenerCreate(&AudioUnitInstrument::audioUnitParameterChanged,
                          this, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode,
                          0.005, // minimum callback interval (seconds)
                          0.005, // callback granularity (for rate-limiting)
                          &auEventListener);
}

void AudioUnitInstrument::audioUnitParameterChanged(void *context, void *object, const AudioUnitEvent *event, UInt64 hostTime, AudioUnitParameterValue parameterValue)
{
    ((AudioUnitInstrument *) context)->parameters[event->mArgument.mParameter.mParameterID]->setValue(parameterValue);
}

void AudioUnitInstrument::loadParameterGroups()
{
    int clumpId = -1;
    string groupName;
    vector<AudioUnitParameterInfo> params = synth.getParameterList();
    for (int p = 0; p < params.size(); p++) {
        if (params[p].clumpID != clumpId) {
            clumpId = params[p].clumpID;
            groupName = params[p].name;
            vector<AudioUnitParameterInfo> parameters;
            parameterGroups[groupName] = parameters;
        }
        parameterGroups[groupName].push_back(params[p]);
    }
}

void AudioUnitInstrument::draw(int x_, int y_)
{
    int x = x_;
    int y = y_;
    int idxGroup = 0;
    map<string, vector<AudioUnitParameterInfo> >::iterator it = parameterGroups.begin();
    for (; it != parameterGroups.end(); ++it) {
        string s = ofToString("Group "+ofToString(idxGroup++));
        ofDrawBitmapString(s, x, y);
        y += 15;
        vector<AudioUnitParameterInfo>::iterator itp = it->second.begin();
        for (; itp != it->second.end(); ++itp) {
            string s = ofToString((*itp).name) + " (" + ofToString((*itp).minValue) + "," + ofToString((*itp).maxValue) + ")";
            ofDrawBitmapString(s, x + 8, y);
            x = y > 560 ? x + 240 : x;
            y = y > 560 ? y_ : y + 15;
        }
        y = y > 560 ? y_ : y + 8;
    }
    /*
    int x = x_;
    int y = y_;
    int clumpId = -1;
    vector<AudioUnitParameterInfo> params = synth.getParameterList();
    for (int p = 0; p < params.size(); p++) {
        if (params[p].clumpID != clumpId) {
            clumpId = params[p].clumpID;
            string s = ofToString("Group "+ofToString(clumpId));
            y += 8;
            ofDrawBitmapString(s, x, y);
            y += 15;
        }
        string s = ofToString(params[p].name) + " (" + ofToString(params[p].minValue) + "," + ofToString(params[p].maxValue) + ")";
        ofDrawBitmapString(s, x + 8, y);
        y += 15;
        if (y > 560) {
            x += 240;
            y = 20;
        }
    }
    */
    
    
}

ofParameter<float> & AudioUnitInstrument::getParameter(string name)
{
    vector<AudioUnitParameterInfo> params = synth.getParameterList();
    for (int p = 0; p < params.size(); p++)
    {
        if (name == params[p].name)
        {
            AudioUnitParameter param = {synth, p, kAudioUnitScope_Global, 0};
            AUListenerAddParameter(auEventListener, this, &param);
            parameters[p] = new AudioUnitInstrumentParameter(&synth, params[p], p);
            return parameters[p]->getParameter();
        }
    }
    // if we got here, no such parameter exists
    ofLog(OF_LOG_ERROR, "No parameter called "+name+" found");
    return;
}

void AudioUnitInstrument::savePreset(string name)
{
    synth.saveCustomPresetAtPath(ofToDataPath(name+".aupreset"));
}

void AudioUnitInstrument::loadPreset(string name)
{
    synth.loadCustomPresetAtPath(ofToDataPath(name+".aupreset"));
}
