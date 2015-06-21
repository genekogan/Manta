#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "AudioUnitInstrument.h"
#include "MantaAudioUnitController.h"

class MantaAudioUnitControllerUI : public MantaAudioUnitController {
  
public:
    ~MantaAudioUnitControllerUI();
    
    void setupSelectionUI(AudioUnitInstrument *synth);

private:
    
    enum SelectedMantaElementType {PAD, SLIDER, BUTTON };

    struct SelectedMantaElement {
        SelectedMantaElementType type;
        int index;
    };
    
    void eventMantaPadClick(int & e);
    void eventMantaSliderClick(int & e);
    void eventMantaButtonClick(int & e);
    
    void guiEvent(ofxUIEventArgs &e);
    void guiMapEvent(ofxUIEventArgs &e);
    void setMantaParameterPair();

    SelectedMantaElement selectedElement;
    
    ofxUITabBar *guiGroups;
    ofxUICanvas *guiMapper;
    ofxUITextArea *guiMantaElement;
    ofxUITextArea *guiSelectedParameter;
    ofxUIRangeSlider *guiRange;
    
    AudioUnitInstrument *synth;
};