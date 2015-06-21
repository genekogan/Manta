#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    synth.setup("Aalto", 'aumu', 'Aalt', 'MLbs');
    synth.showUI();
    
    
    synth.getSynth().connectTo(mixer, 0);
    mixer.connectTo(output);
    output.start();
    
    manta.setup();
    manta.setupSelectionUI(&synth);
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    manta.draw(5, 5, 400);
    manta.drawStats(5, 320, 400);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='s') {
        manta.savePreset("presetTest");
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
