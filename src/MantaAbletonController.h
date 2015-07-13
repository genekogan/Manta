#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxAbletonLive.h"
#include "MantaStats.h"


class MantaAbletonController : public MantaStats
{
public:
    MantaAbletonController();
    virtual ~MantaAbletonController();

    void setup(ofxAbletonLive & live);
    void update();
    
    void mapPadToParameter(int row, int column, int track, string deviceName, ofParameter<float> & parameter, bool toggle=false);
    void mapSliderToParameter(int index, int track, string deviceName, ofParameter<float> & parameter);
    void mapButtonToParameter(int index, int track, string deviceName, ofParameter<float> & parameter, bool toggle=false);
    void mapStatToParameter(int index, int track, string deviceName, ofParameter<float> & parameter);

    void removePadMapping(int row, int column);
    void removeSliderMapping(int index);
    void removeButtonMapping(int index);
    void removeStatMapping(int index);

    void mapSelectionToMidiNotes(int channel);
    void mapAllPadsToMidiNotes(int channel);
    void clearMidiMapping();
    
    void setKey(int key);
    void setMode(int mode);
    void setOctave(int octave);

    int getKey() {return key;}
    int getMode() {return mode;}
    int getOctave() {return octave;}

    void savePreset(string name);
    void loadPreset(string name);
    
protected:

    struct MantaParameterMapping
    {
        float min, max;
        bool toggle;
        int track;
        string deviceName;
        ofParameter<float> parameter;
        MantaParameterMapping(ofParameter<float> & parameter_, int track, string deviceName, bool toggle=false);
        void toggleHighLow() {parameter.set(parameter == min ? max : min);}
    };

    struct AudioUnitNotePair
    {
        int channel;
        int note;
    };
    
    virtual void abletonLoaded() { }
    
    void PadEvent(ofxMantaEvent & evt);
    void SliderEvent(ofxMantaEvent & evt);
    void ButtonEvent(ofxMantaEvent & evt);
    void StatEvent(MantaStatsArgs & evt);    
    void PadVelocityEvent(ofxMantaEvent & evt);
    void ButtonVelocityEvent(ofxMantaEvent & evt);

    void resetMidiMapping();
    void setMidiMapping(int idx, int channel);

    void updatePadColor(int row, int column);
    void updateButtonColor(int index);
    
    void freezePads();
    void setPadFreezingEnabled(bool toFreezePads);
    
    void setupTheory();
    void getChord(int chord[], int root, int octave=0);
    int getNoteAtScaleDegree(int root, int degree, int mode, int octave);
    
    ofxMidiOut midi;
    ofxAbletonLive *live;
    map<int, MantaParameterMapping*> padMap;
    map<int, MantaParameterMapping*> sliderMap;
    map<int, MantaParameterMapping*> buttonMap;
    map<int, MantaParameterMapping*> statMap;
    map<int, AudioUnitNotePair> midiMap;

    bool padFrozen[48];
    bool padReleased[48];
    bool toFreezePads;

    vector<int> major, minorM, minorH, minorN;
    int mode;
    int key;
    int octave;
    
    bool toSetMidiLedColor;
};