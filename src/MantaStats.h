#pragma once

#include "ofMain.h"
#include "ofxManta.h"
#include "ofxConvexHull.h"


struct MantaStatsInfo
{
    string name;
    float min;
    float max;
    ofColor color;
    MantaStatsInfo(string name, float min, float max)
    {
        this->name = name;
        this->min = min;
        this->max = max;
        color = ofColor::white;
    }
};

struct MantaStatsArgs
{
    int index;
    float value;
    MantaStatsArgs(int index, float value)
    {
        this->index = index;
        this->value = value;
    }
};

class MantaStats : public ofxManta
{
public:
    MantaStats();
    
    void setMouseActive(bool active);
    void setVisible(bool visible);
    void toggleVisible() {setVisible(!visible);}
    
    bool setup();
    void draw(int x, int y, int width);
    void drawStats(int x, int y, int w);
    
    ofParameter<float> & getVelocityLerpRate() {return velocityLerpRate;}
    void setVelocityLerpRate(float v) {velocityLerpRate=v;}
    
    ofParameter<float> & getNumPads() {return numPads;}
    ofParameter<float> & getPadSum() {return padSum;}
    ofParameter<float> & getPadAverage() {return padAverage;}
    ofParameter<float> & getPerimeter() {return perimeter;}
    ofParameter<float> & getArea() {return area;}
    ofParameter<float> & getAverageInterFingerDistance() {return averageInterFingerDistance;}
    ofParameter<float> & getCentroidX() {return centroidX;}
    ofParameter<float> & getCentroidY() {return centroidY;}
    ofParameter<float> & getWeightedCentroidX() {return weightedCentroidX;}
    ofParameter<float> & getWeightedCentroidY() {return weightedCentroidY;}
    ofParameter<float> & getWidth() {return padWidth;}
    ofParameter<float> & getHeight() {return padHeight;}
    ofParameter<float> & getWidthHeightRatio() {return whRatio;}
    
    ofParameter<float> & getNumPadsVelocity() {return numPadsVelocity;}
    ofParameter<float> & getPadSumVelocity() {return padSumVelocity;}
    ofParameter<float> & getPadAverageVelocity() {return padAverageVelocity;}
    ofParameter<float> & getPerimeterVelocity() {return perimeterVelocity;}
    ofParameter<float> & getAreaVelocity() {return areaVelocity;}
    ofParameter<float> & getAverageInterFingerDistanceVelocity() {return averageInterFingerDistanceVelocity;}
    ofParameter<float> & getCentroidVelocityX() {return centroidVelocityX;}
    ofParameter<float> & getCentroidVelocityY() {return centroidVelocityY;}
    ofParameter<float> & getWeightedCentroidVelocityX() {return weightedCentroidVelocityX;}
    ofParameter<float> & getWeightedCentroidVelocityY() {return weightedCentroidVelocityY;}
    ofParameter<float> & getWidthVelocity() {return widthVelocity;}
    ofParameter<float> & getHeightVelocity() {return heightVelocity;}
    ofParameter<float> & getWidthHeightRatioVelocity() {return whRatioVelocity;}
    
    void setPadSelection(vector<int> idx, int selection);
    void setSliderSelection(vector<int> idx, int selection);
    void setButtonSelection(vector<int> idx, int selection);
    
    ofPoint getPositionAtPad(int row, int col);

    template <typename L, typename M> void addStatListener(L *listener, M method) {ofAddListener(statsEvent, listener, method);}
    template <typename L, typename M> void removeStatListener(L *listener, M method) {ofRemoveListener(statsEvent, listener, method);}

    MantaStatsInfo getStatsInfo(int index) {return statsInfo[index];}
    
protected:
    
    virtual void update();
    void compareStats(int index, ofParameter<float> *statRef, float newStatValue);
    void setStatsColor(int index, ofColor color) {statsInfo[index].color = color;}
    
    void mousePressed(ofMouseEventArgs &evt);
    void mouseDragged(ofMouseEventArgs &evt);
    void mouseReleased(ofMouseEventArgs &evt);
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);
    
    void getMantaElementsInBox(int x, int y);
    void setMouseResponders();
    bool getIsDragging() {return ofDist(dragPoint1.x, dragPoint1.y, dragPoint2.x, dragPoint2.y) > 40;}
    
    ofxConvexHull convexHull;
    ofRectangle padPositions[48], sliderPositions[2], buttonPositions[4];
    ofRectangle mainDrawRect, statsDrawRect;
    ofRectangle statRects[13];
    int x, y, width, height;
    int px, py, pwidth;
    bool visible, animated;
    bool mouseActive;
    bool shift;
    
    // parameters
    ofParameter<float> velocityLerpRate;
    
    // finger trackers
    vector<ofPoint> fingers, fingersHull;
    vector<float> fingerValues;
    
    // tracking pads and sliders (for velocity)
    float padVelocity[8][6];
    float sliderVelocity[2];
    float buttonVelocity[4];
    float prevPad[8][6];
    float prevSlider[2];
    float prevButton[4];
    
    // finger statistics
    ofParameter<float> numPads;
    ofParameter<float> padSum, padAverage;
    ofParameter<float> centroidX, centroidY;
    ofParameter<float> weightedCentroidX, weightedCentroidY;
    ofParameter<float> averageInterFingerDistance;
    ofParameter<float> perimeter, area;
    ofParameter<float> padWidth, padHeight, whRatio;
    
    // velocity statistics
    ofParameter<float> numPadsVelocity;
    ofParameter<float> padSumVelocity, padAverageVelocity;
    ofParameter<float> centroidVelocityX, centroidVelocityY;
    ofParameter<float> weightedCentroidVelocityX, weightedCentroidVelocityY;
    ofParameter<float> averageInterFingerDistanceVelocity;
    ofParameter<float> perimeterVelocity, areaVelocity;
    ofParameter<float> widthVelocity, heightVelocity, whRatioVelocity;
    
    // stats
    vector<MantaStatsInfo> statsInfo;

    // velocity epsilon threshold
    float EPSILON;
    
    // selection
    ofPoint dragPoint1, dragPoint2;
    bool dragging;
    int statSelection;
    
    // events
    ofEvent<int> eventPadClick;
    ofEvent<int> eventSliderClick;
    ofEvent<int> eventButtonClick;
    ofEvent<int> eventStatClick;
    ofEvent<MantaStatsArgs> statsEvent;
    
    // internal
    ofFbo fboStats;
    bool toRedrawStats;
};