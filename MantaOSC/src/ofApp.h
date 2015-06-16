#pragma once

#include "ofMain.h"
#include "ofxManta.h"
#include "ofxUI.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp
{
private:
    void setup();
    void update();
    void draw();
    void exit();
    
    void buildGui();
    
    void windowResized(int w, int h);
    
    void PadEvent(ofxMantaEvent & evt);
    void SliderEvent(ofxMantaEvent & evt);
    void ButtonEvent(ofxMantaEvent & evt);
    void PadVelocityEvent(ofxMantaEvent & evt);
    void ButtonVelocityEvent(ofxMantaEvent & evt);
    
    void togglePadFormat();
    
    void guiEvent(ofxUIEventArgs &e);
    void guiOptionsEvent(ofxUIEventArgs &e);
    
    void setupOscSender(string _host, int _portOut, bool force=false);
    void setupOscReceiver(int _portIn, bool force=false);
    
    void saveSettings();
    void loadSettings();
    
    int view;
    
    ofxManta manta;

    ofxUICanvas *gui, *guiOptions;
    ofxUITextInput *guiHostIn, *guiPortIn, *guiPortOut;
    ofxUILabelButton *guiView;
    ofxUILabelToggle *guiAnimated;
    
    string host;
    int portIn, portOut;
    bool padFormat;
    
    string aOscPad, aOscPadOn, aOscPadOff, aOscSlider, aOscButton, aOscButtonVelocity;
    string dOscPad, dOscPadOn, dOscPadOff, dOscSlider, dOscButton, dOscButtonVelocity;
    string aRLed, aRLedPad, aRLedSlider, aRLedButton;
    string dRLed, dRLedPad, dRLedSlider, dRLedButton;

    bool oscIn, oscOut;
    bool oscPad, oscPadOn, oscPadOff, oscSlider, oscButton, oscButtonVelocity;
    bool rLed, rLedPad, rLedSlider, rLedButton;
    bool animated;
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    bool failedOscIn, failedOscOut;
};

