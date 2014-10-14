#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // decide which manta inputs are available
    learn.addAllPadsAsInput();
    learn.addSlidersAsInput();
    learn.addButtonsAsInput();
    learn.addNumFingersAsInput();
    learn.addPadSumAsInput();
    learn.addPadAverageAsInput();
    learn.addPerimterAsInput();
    learn.addAverageInterFingerDistanceAsInput();
    learn.addCentroidAsInput();
    learn.addWeightedCentroidAsInput();

    // add outputs
    learn.addOutput("output1", 20, 50);

    // osc
    learn.setupOscSender("localhost", 1234);
    learn.setupOscReceiver(8000);
}

//--------------------------------------------------------------
void ofApp::update(){
    learn.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    learn.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='g')
        learn.toggleVisible();
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
