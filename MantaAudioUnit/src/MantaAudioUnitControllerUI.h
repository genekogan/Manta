#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "AudioUnitInstrument.h"
#include "MantaAudioUnitController.h"

class MantaAudioUnitControllerUI : public MantaAudioUnitController {
  
public:
    MantaAudioUnitControllerUI();
    ~MantaAudioUnitControllerUI();
    void setupUI();

private:
    
    enum SelectedMantaElementType { NONE=0, PAD, SLIDER, BUTTON };

    struct SelectedMantaElement {
        SelectedMantaElementType type;
        int index;
    };
    
    struct MantaParameterPairSelection {
        AudioUnitInstrument *selectedAudioUnit;
        SelectedMantaElement mantaElement;
        string parameterName;
        MantaParameterPairSelection(AudioUnitInstrument *selectedAudioUnit, SelectedMantaElement mantaElement, string parameterName) {
            this->selectedAudioUnit = selectedAudioUnit;
            this->mantaElement = mantaElement;
            this->parameterName = parameterName;
        }
    };
    
    void eventMantaPadClick(int & e);
    void eventMantaSliderClick(int & e);
    void eventMantaButtonClick(int & e);
    
    void checkSelectedPair();
    void addSelectedMapping();
    void removeSelectedMapping();
    
    void guiSelectEvent(ofxUIEventArgs &e);
    void guiMidiEvent(ofxUIEventArgs &e);
    void guiMapEvent(ofxUIEventArgs &e);
    void guiViewEvent(ofxUIEventArgs &e);
    void guiPresetsEvent(ofxUIEventArgs &e);

    ofxUITabBar *guiGroups;
    ofxUICanvas *guiMapper, *guiView, *guiMidi, *guiPresets;
    vector<ofxUICanvas*> guiParameterGroups;
    ofxUITextArea *guiMantaElement, *guiSelectedParameter;
    ofxUIRangeSlider *guiRange;
    ofxUIButton *guiDelete;
    
    map<pair<AudioUnitInstrument*, string>, AudioUnitParameterInfo> parameterLU;
    map<ofxUIButton*, AudioUnitInstrument*> synthLU;
    map<ofxUICanvas*, AudioUnitInstrument*> groupSynthLU;
    map<ofxUIButton*, MantaParameterPairSelection*> buttonMappingLU;
    
    SelectedMantaElement selectedElement;
    AudioUnitInstrument *selectedAudioUnit;
    ofxUIToggle *selectedParameterToggle, *selectedViewMappingToggle;
};