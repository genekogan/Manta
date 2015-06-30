#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
    
//    fx.setup("FX", 'aufx', 'SDmk', 'MNor');
//    fx.setup("FX", 'aumu', 'NSA0', 'NSA_');
    fx.setup("FX", 'aufx', 'seq3', 'SNSH');
  
    input.connectTo(tap).connectTo(fx.getSynth());
    fx.getSynth().connectTo(mixer, 0);
    mixer.connectTo(output);

    input.start();
    output.start();
    
    manta.registerAudioUnit(fx);
    
    manta.setup();
    manta.setupUI();
    
    

}

//--------------------------------------------------------------
void ofApp::update(){
    tap.getStereoWaveform(oscLineLeft, oscLineRight, ofGetWidth(), ofGetHeight()/4);

    mixer.setOutputVolume(ofMap(mouseX, 0, ofGetWidth(), 4, 10));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofPushStyle();

    manta.draw(5, 5, 400);
    manta.drawStats(5, 320, 400);
    
    drawOscilloscopeGrid();
    drawOscilloscopeLines();
    drawOscilloscopeLabels();

    ofPopMatrix();
    ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::drawOscilloscopeGrid() {
    ofSetColor(0, 40, 0);
    ofSetLineWidth(1);
    float step = float(ofGetHeight()) / 64;
    for(int x = 0; x < ofGetWidth(); x += step)
    {
        ofLine(x, 0, x, ofGetHeight());
    }
    for(int y = 0; y < ofGetHeight(); y += step)
    {
        ofLine(0, y, ofGetWidth(), y);
    }
}

//--------------------------------------------------------------
void ofApp::drawOscilloscopeLabels() {
    ofSetColor(0, 200, 40);
    ofDrawBitmapString("Left Channel", 25, ofGetHeight() / 8 - 40);
    ofDrawBitmapString("Right Channel", 25, ofGetHeight() - (ofGetHeight() / 8 - 40));
    for(int y = 0; y < ofGetHeight(); y += ofGetHeight() / 2)
    {
        ofPushMatrix();
        {
            ofTranslate(0, y);
            
            ofTranslate(0, ofGetHeight() / 8);
            ofDrawBitmapString("+1", 0, 0);
            ofDrawBitmapString("+1", ofGetWidth() - 20, 0);
            
            ofTranslate(0, ofGetHeight() / 8);
            ofDrawBitmapString("0", 3, 0);
            ofDrawBitmapString("0", ofGetWidth() - 15, 0);
            
            ofTranslate(0, ofGetHeight() / 8);
            ofDrawBitmapString("-1", 0, 0);
            ofDrawBitmapString("-1", ofGetWidth() - 20, 0);
        }
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::drawOscilloscopeLines() {
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetColor(60, 150, 80, 175);
    for(int i = 1; i < 7; i++)
    {
        ofSetLineWidth(i);
        ofPushMatrix();
        {
            ofTranslate(0, ofGetHeight()/8 - 5);
            oscLineLeft.draw();
            ofTranslate(0, ofGetHeight()/2);
            oscLineRight.draw();
        }
        ofPopMatrix();
    }
    ofDisableBlendMode();
    
    ofSetColor(255);
    ofSetLineWidth(2);
    ofPushMatrix();
    {
        ofTranslate(0, ofGetHeight()/8 - 5);
        oscLineLeft.draw();
        ofTranslate(0, ofGetHeight()/2);
        oscLineRight.draw();
    }
    ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='s') {
        manta.savePreset("presetTest");
    }
    else if (key==' ') {
        fx.getSynth().showUI();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
