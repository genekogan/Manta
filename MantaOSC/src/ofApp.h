#pragma once

#include "ofMain.h"
#include "ofxManta.h"
#include "ofxUI.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp
{
private:
    ~ofApp() {exit();}
    void setup();
    void update();
    void draw();
    void exit();
    
    void windowResized(int w, int h);
    
    void PadEvent(ofxMantaEvent & evt);
    void SliderEvent(ofxMantaEvent & evt);
    void ButtonEvent(ofxMantaEvent & evt);
    void PadVelocityEvent(ofxMantaEvent & evt);
    void ButtonVelocityEvent(ofxMantaEvent & evt);
    
    void guiEvent(ofxUIEventArgs &e);
    void guiOptionsEvent(ofxUIEventArgs &e);
    
    void setupOscSender(string _host, int _portOut, bool force=false);
    void setupOscReceiver(int _portIn, bool force=false);
    
    ofxManta manta;

    ofxUICanvas *gui, *guiOptions;
    ofxUITextInput *guiHostIn, *guiPortIn, *guiPortOut;
    
    string host;
    int portIn, portOut;
    
    string aOscPad, aOscPadVelocity, aOscSlider, aOscButton, aOscButtonVelocity;
    
    bool oscIn, oscOut;
    bool oscPad, oscPadVelocity, oscSlider, oscButton, oscButtonVelocity;
    bool rLed, rLedPad, rLedSlider, rLedButton;
    bool animated;
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    bool failedOscIn, failedOscOut;
    
};

