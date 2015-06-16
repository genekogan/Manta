#pragma once

#include "ofMain.h"
#include "MantaStats.h"
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
    
    void sendStatOSCMessage(string address, float value, bool isVelocity);
    
    void changedNumPads(float &v);
    void changedPadSum(float &v);
    void changedPadAverage(float &v);
    void changedCentroidX(float &v);
    void changedCentroidY(float &v);
    void changedWeightedCentroidX(float &v);
    void changedWeightedCentroidY(float &v);
    void changedAverageInterFingerDistance(float &v);
    void changedPerimeter(float &v);
    void changedArea(float &v);
    void changedPadWidth(float &v);
    void changedPadHeight(float &v);
    void changedWhRatio(float &v);
    void changedNumPadsVelocity(float &v);
    void changedPadSumVelocity(float &v);
    void changedPadAverageVelocity(float &v);
    void changedCentroidVelocityX(float &v);
    void changedCentroidVelocityY(float &v);
    void changedWeightedCentroidVelocityX(float &v);
    void changedWeightedCentroidVelocityY(float &v);
    void changedAverageInterFingerDistanceVelocity(float &v);
    void changedPerimeterVelocity(float &v);
    void changedAreaVelocity(float &v);
    void changedWidthVelocity(float &v);
    void changedHeightVelocity(float &v);
    void changedWhRatioVelocity(float &v);

    void guiEvent(ofxUIEventArgs &e);
    void guiOptionsEvent(ofxUIEventArgs &e);
    
    void setupOscSender(string _host, int _portOut, bool force=false);
    void setupOscReceiver(int _portIn, bool force=false);
    
    void saveSettings();
    void loadSettings();
    
    void addMantaStatToGui(string name, string address, bool *val, bool *vel);

    int view;
    
    MantaStats manta;

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

    string aOscNumPads, aOscPadSum, aOscPadAverage;
    string aOscCentroidX, aOscCentroidY;
    string aOscWeightedCentroidX, aOscWeightedCentroidY;
    string aOscAverageInterFingerDistance;
    string aOscPerimeter, aOscArea;
    string aOscPadWidth, aOscPadHeight, aOscWhRatio;
   
    bool oscNumPads, oscPadSum, oscPadAverage;
    bool oscCentroidX, oscCentroidY;
    bool oscWeightedCentroidX, oscWeightedCentroidY;
    bool oscAverageInterFingerDistance;
    bool oscPerimeter, oscArea;
    bool oscPadWidth, oscPadHeight, oscWhRatio;
    
    bool oscNumPadsV, oscPadSumV, oscPadAverageV;
    bool oscCentroidXV, oscCentroidYV;
    bool oscWeightedCentroidXV, oscWeightedCentroidYV;
    bool oscAverageInterFingerDistanceV;
    bool oscPerimeterV, oscAreaV;
    bool oscPadWidthV, oscPadHeightV, oscWhRatioV;
    
    bool oscIn, oscOut;
    bool oscPad, oscPadOn, oscPadOff, oscSlider, oscButton, oscButtonVelocity;
    bool rLed, rLedPad, rLedSlider, rLedButton;
    bool animated;
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    bool failedOscIn, failedOscOut;
    
    // from MantaStats
    ofParameter<float> numPads;
    ofParameter<float> padSum, padAverage;
    ofParameter<float> centroidX, centroidY;
    ofParameter<float> weightedCentroidX, weightedCentroidY;
    ofParameter<float> averageInterFingerDistance;
    ofParameter<float> perimeter, area;
    ofParameter<float> padWidth, padHeight, whRatio;
    ofParameter<float> numPadsVelocity;
    ofParameter<float> padSumVelocity, padAverageVelocity;
    ofParameter<float> centroidVelocityX, centroidVelocityY;
    ofParameter<float> weightedCentroidVelocityX, weightedCentroidVelocityY;
    ofParameter<float> averageInterFingerDistanceVelocity;
    ofParameter<float> perimeterVelocity, areaVelocity;
    ofParameter<float> widthVelocity, heightVelocity, whRatioVelocity;
    
    float velocitySmoothness;
};

