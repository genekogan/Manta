#include "ofApp.h"

//----------
void ofApp::setup(){
    ofSetEscapeQuitsApp(false);
    
    ofSetWindowTitle("Manta OSC");
    
    manta.setup();
    
    manta.addPadListener(this, &ofApp::PadEvent);
    manta.addSliderListener(this, &ofApp::SliderEvent);
    manta.addButtonListener(this, &ofApp::ButtonEvent);
    manta.addPadVelocityListener(this, &ofApp::PadVelocityEvent);
    manta.addButtonVelocityListener(this, &ofApp::ButtonVelocityEvent);
    
    host = "localhost";
    portIn = 2758;
    portOut = 2759;
    
    oscPad = true;
    oscPadVelocity = true;
    oscSlider = true;
    oscButton = true;
    oscButtonVelocity = true;
    rLed = false;
    rLedPad = true;
    rLedSlider = true;
    rLedButton = true;
    animated = true;
    
    failedOscIn = false;
    failedOscOut = true;
    
    aOscPad = "/manta/pad";
    aOscPadVelocity = "/manta/padVelocity";
    aOscSlider = "/manta/slider";
    aOscButton = "/manta/button";
    aOscButtonVelocity = "/manta/buttonVelocity";
    
    guiOptions = new ofxUICanvas("Manta Options");
    guiOptions->setFont("AndaleMono.ttf");
    guiOptions->clearWidgets();
    guiOptions->setPosition(5, 5);
    guiOptions->setWidth(310);
    guiOptions->setHeight(24);
    guiOptions->addSpacer(20,0);
    guiOptions->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiOptions->addLabelToggle("animated", &animated, 100.0f);
    guiOptions->addSpacer(20,0);
    guiOptions->addLabelButton("osc info", false, 100.0f);
    
    gui = new ofxUICanvas("Manta OSC");
    gui->setWidth(256);
    gui->setHeight(290);
    gui->setFont("AndaleMono.ttf");
    gui->clearWidgets();
    gui->setPosition(320, 5);

    gui->addLabel("host out:");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiHostIn = gui->addTextInput("hostOut", host);
    guiHostIn->setAutoClear(false);
    guiHostIn->getRect()->setWidth(150);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addLabel("port out:");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiPortOut = gui->addTextInput("portOut", ofToString(portOut));
    guiPortOut->setAutoClear(false);
    guiPortOut->getRect()->setWidth(150);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addLabel("port in:");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiPortIn = gui->addTextInput("portIn", ofToString(portIn));
    guiPortIn->setAutoClear(false);
    guiPortIn->getRect()->setWidth(150);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addSpacer();
    
    gui->addTextArea("noteOscOut", "OSC outputs");
    
    gui->addToggle("bPad", &oscPad)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tPad", aOscPad)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bPadVelocity", &oscPadVelocity)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tPadVelocity", aOscPadVelocity)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bSlider", &oscSlider)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tSlider", aOscSlider)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bButton", &oscButton)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tButton", aOscButton)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    gui->addToggle("bButtonVelocity", &oscButtonVelocity)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tButtonVelocity", aOscButtonVelocity)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSpacer();

    gui->addTextArea("noteOscIn", "LED control");
    
    gui->addToggle("bRLed", &rLed)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLed", "/manta/led/enable")->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bRLedPad", &rLedPad)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLedPad", "/manta/led/pad")->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    gui->addToggle("bRLedSlider", &rLedSlider)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLedSlider", "/manta/led/slider")->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    gui->addToggle("bRLedButton", &rLedButton)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLedButton", "/manta/led/button")->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    setupOscSender(host, portOut, true);
    setupOscReceiver(portIn, true);

    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    ofAddListener(guiOptions->newGUIEvent, this, &ofApp::guiOptionsEvent);
}

//----------
void ofApp::setupOscSender(string _host, int _portOut, bool force) {
    if ((force || host!=_host || portOut!=_portOut) && host!="" && portOut!=ofToInt("")) {
        try {
            host = _host;
            portOut = _portOut;
            sender.setup(host, portOut);
            oscOut = true;
            failedOscOut = false;
        }
        catch(runtime_error &e) {
            oscOut = false;
            failedOscOut = true;
            ofSystemAlertDialog("Error: OSC sender failed to connect. Check to make sure port "+ofToString(portOut)+" is not occupied.");
        }
    }
}

//----------
void ofApp::setupOscReceiver(int _portIn, bool force) {
    if ((force || portIn != _portIn) && portIn != ofToInt("")) {
        try {
            portIn = _portIn;
            receiver.setup(portIn);
            oscIn = true;
            failedOscIn = false;
        }
        catch(runtime_error &e) {
            oscIn = false;
            failedOscIn = true;
            ofSystemAlertDialog("Error: OSC receiver failed to connect. Check to make sure port "+ofToString(portIn)+" is not occupied.");
        }
    }
}

//----------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    if      (e.getName() == "hostOut") {
        setupOscSender(guiHostIn->getTextString(), ofToInt(guiPortOut->getTextString()));
    }
    else if (e.getName() == "portOut") {
        setupOscSender(guiHostIn->getTextString(), ofToInt(guiPortOut->getTextString()));
    }
    else if (e.getName() == "portIn") {
        setupOscReceiver(ofToInt(guiPortIn->getTextString()));
    }
    else if (e.getName() == "bRLed") {
        if (rLed) {
            
        }
        else {
            
        }
    }
    else if (e.getName() == "bRLedPad") {
        if (rLedPad) {
            
        }
        else {
            
        }
        
    }
    else if (e.getName() == "bRLedSlider") {
        if (rLedSlider) {
            
        }
        else {
            
        }
        
    }
    else if (e.getName() == "bRLedButton") {
        if (rLedButton) {
            
        }
        else {
            
        }
    }
}

//----------
void ofApp::guiOptionsEvent(ofxUIEventArgs &e) {
    if      (e.getName() == "osc info") {
        if (e.getButton()->getValue() == 1) return;
        string msg = "tbd";
        ofSystemAlertDialog(msg);
    }
    else if (e.getName() == "animated") {
        manta.setAnimated(animated);
    }
}

//----------
void ofApp::update(){
    if (oscIn) {
        while (receiver.hasWaitingMessages()){
            ofxOscMessage msg;
            receiver.getNextMessage(&msg);
            string address = msg.getAddress();
        }
    }
}

//----------
void ofApp::draw(){
    ofBackground(100);
    manta.draw(5, 36, 310);

    if (failedOscIn && failedOscOut) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofRect(3, 279, 314, 19);
        ofSetColor(255);
        ofDrawBitmapString("OSC sender & receiver failed to connect. ", 5, 294);
        ofPopStyle();
    }
    else if (failedOscIn) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofRect(3, 279, 314, 19);
        ofSetColor(255);
        ofDrawBitmapString("OSC receiver failed to connect. ", 5, 294);
        ofPopStyle();
    }
    else if (failedOscOut) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofRect(3, 279, 314, 19);
        ofSetColor(255);
        ofDrawBitmapString("OSC sender failed to connect. ", 5, 294);
        ofPopStyle();
    }
}

//----------
void ofApp::exit() {
    manta.close();
}

//----------
void ofApp::PadEvent(ofxMantaEvent & evt) {
    if (oscOut && oscPad) {
        ofxOscMessage msg;
        msg.setAddress(aOscPad);
        msg.addIntArg(evt.row);
        msg.addIntArg(evt.col);
        msg.addIntArg(evt.value);
        sender.sendMessage(msg);
        //cout << "Pad event: " << ofGetElapsedTimeMicros() << ", id " << evt.id << ", row "<< evt.row <<", column "<< evt.col << ", value "<< evt.value << endl;
    }
}

//----------
void ofApp::SliderEvent(ofxMantaEvent & evt) {
    if (oscOut && oscSlider) {
        ofxOscMessage msg;
        msg.setAddress(aOscSlider);
        msg.addIntArg(evt.id);
        msg.addIntArg(evt.value);
        sender.sendMessage(msg);
        //cout << "Slider event: id " << evt.id << ", value "<< evt.value << endl;
    }
}

//----------
void ofApp::ButtonEvent(ofxMantaEvent & evt) {
    if (oscOut && oscButton) {
        ofxOscMessage msg;
        msg.setAddress(aOscButton);
        msg.addIntArg(evt.id);
        msg.addIntArg(evt.value);
        sender.sendMessage(msg);
        //cout << "Button event: id " << evt.id << ", value "<< evt.value << endl;
    }
}

//----------
void ofApp::PadVelocityEvent(ofxMantaEvent & evt) {
    if (oscOut && oscPadVelocity) {
        ofxOscMessage msg;
        msg.setAddress(aOscPadVelocity);
        msg.addIntArg(evt.row);
        msg.addIntArg(evt.col);
        msg.addIntArg(evt.value);
        sender.sendMessage(msg);
        //cout << "Pad velocity event: id " << evt.id << ", row "<< evt.row <<", column "<< evt.col << ", value "<< evt.value << endl;
    }
}

//----------
void ofApp::ButtonVelocityEvent(ofxMantaEvent & evt) {
    if (oscOut && oscButtonVelocity) {
        ofxOscMessage msg;
        msg.setAddress(aOscButtonVelocity);
        msg.addIntArg(evt.id);
        msg.addIntArg(evt.value);
        sender.sendMessage(msg);
        //cout << "Button velocity event: id " << evt.id << ", value "<< evt.value << endl;
    }
}

//----------
void ofApp::windowResized(int w, int h) {
    ofHideCursor();
    ofShowCursor();
}
