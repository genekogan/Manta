#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "MantaAbletonController.h"


class MantaAbletonControllerUI : public MantaAbletonController
{
public:
    MantaAbletonControllerUI();
    ~MantaAbletonControllerUI();
    void setupUI();
    void setupUIPresets();
    
private:
    
    enum SelectedMantaElementType {
        NONE=0, PAD, SLIDER, BUTTON, STAT
    };

    struct SelectedMantaElement {
        SelectedMantaElementType type;
        int index;
    };
    
    struct MantaParameterPairSelection
    {
        int selectMode;
        int selectedTrack;
        ofxAbletonLiveDevice *selectedDevice;
        SelectedMantaElement mantaElement;
        string parameterName;
        MantaParameterPairSelection(int selectMode, int selectedTrack, ofxAbletonLiveDevice *selectedDevice, SelectedMantaElement mantaElement, string parameterName)
        {
            this->selectMode = selectMode;
            this->selectedTrack = selectedTrack;
            this->selectedDevice = selectedDevice;
            this->mantaElement = mantaElement;
            this->parameterName = parameterName;
        }
    };
    
    void loadPresetData();
    void abletonLoaded();
    
    void eventMantaPadClick(int & e);
    void eventMantaSliderClick(int & e);
    void eventMantaButtonClick(int & e);
    void eventMantaStatClick(int & e);
    
    void checkSelectedPair();
    void addSelectedMapping();
    void removeSelectedMapping();
    
    void guiSelectEvent(ofxUIEventArgs &e);
    void guiSelectTrackEvent(ofxUIEventArgs &e);
    void guiSelectGlobalEvent(ofxUIEventArgs &e);
    void guiMidiEvent(ofxUIEventArgs &e);
    void guiMapEvent(ofxUIEventArgs &e);
    void guiViewEvent(ofxUIEventArgs &e);
    void guiPresetsEvent(ofxUIEventArgs &e);
    void resetKeyString();
    
    ofxUITabBar *guiGroups;
    ofxUICanvas *guiMapper, *guiView, *guiMidi, *guiPresets;
    vector<ofxUICanvas*> guiParameterGroups;
    ofxUITextArea *guiMantaElement, *guiSelectedParameter;
    ofxUIRangeSlider *guiRange;
    ofxUILabelToggle *guiToggle;
    ofxUIButton *guiDelete;
    ofxUIIntSlider *guiMidiChannel;
    ofxUILabelButton *guiMidiMap;
    vector<ofxUICanvas*> innerGuis;
    
    map<pair<ofxAbletonLiveDevice*, string>, ofxAbletonLiveParameter*> parameterLU;
    map<ofxUICanvas*, ofxAbletonLiveDevice*> groupDeviceLU;
    map<ofxUICanvas*, int> groupTrackLU;
    map<ofxUIButton*, MantaParameterPairSelection*> buttonMappingLU;
    
    SelectedMantaElement selectedElement;
    ofxAbletonLiveDevice *selectedDevice;
    int selectedTrack;
    int selectMode;
    ofxUIToggle *selectedParameterToggle, *selectedViewMappingToggle;
};