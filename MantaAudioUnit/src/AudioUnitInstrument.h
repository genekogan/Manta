#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"


class AudioUnitInstrumentParameter
{
public:
    AudioUnitInstrumentParameter(ofxAudioUnitSampler *synth, AudioUnitParameterInfo parameter, int index);
    ofParameter<float> & getParameter() {return value;}
    void setValue(float v);
    
private:
    void parameterChanged(float & v);
    
    int index;
    ofParameter<float> value;
    ofxAudioUnitSampler *synth;
};


class AudioUnitInstrument
{
public:
    void setup(string name, OSType type, OSType subType, OSType manufacturer);
    void blah();
    void draw(int x_, int y_);
    void showUI() {synth.showUI();}
    
    string getName() {return name;}
    ofxAudioUnitSampler & getSynth() {return synth;}
    ofParameter<float> & getParameter(string name);
    
    void savePreset(string name);
    void loadPreset(string name);
    
    map<string, vector<AudioUnitParameterInfo> > parameterGroups;
    
private:
    
    static void audioUnitParameterChanged(void * context, void * object, const AudioUnitEvent * event, UInt64 hostTime, AudioUnitParameterValue value);
    
    string name;
    map<int, AudioUnitInstrumentParameter*> parameters;
    ofxAudioUnitSampler synth;
    AUEventListenerRef auEventListener;
    
    OSType type;
    OSType subType;
    OSType manufacturer;
};


