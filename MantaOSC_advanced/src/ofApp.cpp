#include "ofApp.h"

//----------
void ofApp::setup(){
    //ofSetDataPathRoot("../Resources/data/");
    
    ofSetEscapeQuitsApp(false);
    
    ofSetWindowTitle("Manta OSC ++");
    
    manta.setup();
    
    manta.addPadListener(this, &ofApp::PadEvent);
    manta.addSliderListener(this, &ofApp::SliderEvent);
    manta.addButtonListener(this, &ofApp::ButtonEvent);
    manta.addPadVelocityListener(this, &ofApp::PadVelocityEvent);
    manta.addButtonVelocityListener(this, &ofApp::ButtonVelocityEvent);
    
    numPads.addListener(this, &ofApp::changedNumPads);
    padSum.addListener(this, &ofApp::changedPadSum);
    padAverage.addListener(this, &ofApp::changedPadAverage);
    centroidX.addListener(this, &ofApp::changedCentroidX);
    centroidY.addListener(this, &ofApp::changedCentroidY);
    weightedCentroidX.addListener(this, &ofApp::changedWeightedCentroidX);
    weightedCentroidY.addListener(this, &ofApp::changedWeightedCentroidY);
    averageInterFingerDistance.addListener(this, &ofApp::changedAverageInterFingerDistance);
    perimeter.addListener(this, &ofApp::changedPerimeter);
    area.addListener(this, &ofApp::changedArea);
    padWidth.addListener(this, &ofApp::changedPadWidth);
    padHeight.addListener(this, &ofApp::changedPadHeight);
    whRatio.addListener(this, &ofApp::changedWhRatio);
    numPadsVelocity.addListener(this, &ofApp::changedNumPadsVelocity);
    padSumVelocity.addListener(this, &ofApp::changedPadSumVelocity);
    padAverageVelocity.addListener(this, &ofApp::changedPadAverageVelocity);
    centroidVelocityX.addListener(this, &ofApp::changedCentroidVelocityX);
    centroidVelocityY.addListener(this, &ofApp::changedCentroidVelocityY);
    weightedCentroidVelocityX.addListener(this, &ofApp::changedWeightedCentroidVelocityX);
    weightedCentroidVelocityY.addListener(this, &ofApp::changedWeightedCentroidVelocityY);
    averageInterFingerDistanceVelocity.addListener(this, &ofApp::changedAverageInterFingerDistanceVelocity);
    perimeterVelocity.addListener(this, &ofApp::changedPerimeterVelocity);
    areaVelocity.addListener(this, &ofApp::changedAreaVelocity);
    widthVelocity.addListener(this, &ofApp::changedWidthVelocity);
    heightVelocity.addListener(this, &ofApp::changedHeightVelocity);
    whRatioVelocity.addListener(this, &ofApp::changedWhRatioVelocity);
    
    host = "localhost";
    portIn = 27518;
    portOut = 27519;
    
    oscPad = true;
    oscPadOn = true;
    oscPadOff = true;
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
    
    velocitySmoothness = 1.0 - manta.getVelocityLerpRate();
    
    loadSettings();
    
    aOscPad = "/manta/pad";
    aOscPadOn = "/manta/padOn";
    aOscPadOff = "/manta/padOff";
    aOscSlider = "/manta/slider";
    aOscButton = "/manta/button";
    aOscButtonVelocity = "/manta/buttonVelocity";
    
    dOscPad = padFormat ? "(row,col,value)" : "(index,value)";
    dOscPadOn = padFormat ? "(row,col,value)" : "(index,value)";
    dOscPadOff = padFormat ? "(row,col,value)" : "(index,value)";
    dOscSlider = "(index,value)";
    dOscButton = "(index,value)";
    dOscButtonVelocity = "(index,value)";
    
    aOscNumPads = "/manta/numPads";
    aOscPadSum = "/manta/padSum";
    aOscPadAverage = "/manta/padAvg";
    aOscCentroidX = "/manta/centroidX";
    aOscCentroidY = "/manta/centroidY";
    aOscWeightedCentroidX = "/manta/weightedCentroidX";
    aOscWeightedCentroidY = "/manta/weightedCentroidY";
    aOscAverageInterFingerDistance = "/manta/avgFingerDist";
    aOscPerimeter = "/manta/perimeter";
    aOscArea = "/manta/area";
    aOscPadWidth = "/manta/padWidth";
    aOscPadHeight = "/manta/padHeight";
    aOscWhRatio = "/manta/padWhRatio";
    
    aRLed = "/manta/led/enable";
    aRLedPad = "/manta/led/pad";
    aRLedSlider = "/manta/led/slider";
    aRLedButton = "/manta/led/button";
    
    dRLed = "(0/1)";
    dRLedPad = padFormat ? "(row,col,0/1/2)" : "(index,0/1/2)";
    dRLedSlider = "(index,value)";
    dRLedButton = "(index,value)";
    
    guiOptions = new ofxUICanvas("Manta Options");
    guiOptions->setFont("AndaleMono.ttf");
    guiOptions->clearWidgets();
    guiOptions->setPosition(5, 5);
    guiOptions->setWidth(400);
    guiOptions->setHeight(24);
    guiOptions->addSpacer(5,0);
    guiOptions->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiView = guiOptions->addLabelButton("osc info", false, 100.0f);
    guiOptions->addSpacer(165,0);
    guiAnimated = guiOptions->addLabelToggle("animated", &animated, 100.0f);
    
    gui = new ofxUICanvas("Manta OSC");
    gui->setFont("AndaleMono.ttf");
    buildGui();
    gui->setVisible(false);

    setupOscSender(host, portOut, true);
    setupOscReceiver(portIn, true);

    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    ofAddListener(guiOptions->newGUIEvent, this, &ofApp::guiOptionsEvent);
}

//----------
void ofApp::buildGui() {
    gui->clearWidgets();
    gui->setPosition(5, 36);
    gui->setWidth(400);
    gui->setHeight(310);
    
    gui->addLabel("host out: ");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiHostIn = gui->addTextInput("hostOut", host);
    guiHostIn->setAutoClear(false);
    guiHostIn->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addLabel("port out: ");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiPortOut = gui->addTextInput("portOut", ofToString(portOut));
    guiPortOut->setAutoClear(false);
    guiPortOut->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addLabel("port in:  ");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiPortIn = gui->addTextInput("portIn", ofToString(portIn));
    guiPortIn->setAutoClear(false);
    guiPortIn->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addLabel("pad index format (row, col)");
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addToggle("pad format", &padFormat)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addSpacer();
    
    gui->addTextArea("noteOscOut", "OSC outputs");
    
    gui->addToggle("bPad", &oscPad)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tPad", aOscPad+"            "+dOscPad)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bPadOn", &oscPadOn)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tPadOn", aOscPadOn+"          "+dOscPadOn)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bPadOff", &oscPadOff)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tPadOff", aOscPadOff+"         "+dOscPadOff)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bSlider", &oscSlider)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tSlider", aOscSlider+"         "+dOscSlider)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bButton", &oscButton)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tButton", aOscButton+"         "+dOscButton)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bButtonVelocity", &oscButtonVelocity)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tButtonVelocity", aOscButtonVelocity+" "+dOscButtonVelocity)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSpacer();
    
    gui->addTextArea("statsOscOut", "Manta Stats {Value, Velocity***}");
    
    addMantaStatToGui("NumPads", aOscNumPads, &oscNumPads, &oscNumPadsV);
    addMantaStatToGui("PadSum", aOscPadSum, &oscPadSum, &oscPadSumV);
    addMantaStatToGui("PadAvg", aOscPadAverage, &oscPadAverage, &oscPadAverageV);
    addMantaStatToGui("CentroidX", aOscCentroidX, &oscCentroidX, &oscCentroidXV);
    addMantaStatToGui("CentroidY", aOscCentroidY, &oscCentroidY, &oscCentroidYV);
    addMantaStatToGui("wCentroidX", aOscWeightedCentroidX, &oscWeightedCentroidX, &oscWeightedCentroidXV);
    addMantaStatToGui("wCentroidY", aOscWeightedCentroidY, &oscWeightedCentroidY, &oscWeightedCentroidYV);
    addMantaStatToGui("AvgFingerDist", aOscAverageInterFingerDistance, &oscAverageInterFingerDistance, &oscAverageInterFingerDistanceV);
    addMantaStatToGui("Perimeter", aOscPerimeter, &oscPerimeter, &oscPerimeterV);
    addMantaStatToGui("Area", aOscArea, &oscArea, &oscAreaV);
    addMantaStatToGui("PadWidth", aOscPadWidth, &oscPadWidth, &oscPadWidthV);
    addMantaStatToGui("PadHeight", aOscPadHeight, &oscPadHeight, &oscPadHeightV);
    addMantaStatToGui("WidthHeightRatio", aOscWhRatio, &oscWhRatio, &oscWhRatioV);
    
    gui->addSlider("Velocity smoothness", 0, 1, &velocitySmoothness);
    
    gui->addSpacer();
    
    gui->addTextArea("noteOscIn", "LED control");
    
    gui->addToggle("bRLed", &rLed)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLed", aRLed+"     "+dRLed)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bRLedPad", &rLedPad)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLedPad", aRLedPad+"        "+dRLedPad)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bRLedSlider", &rLedSlider)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLedSlider", aRLedSlider+"     "+dRLedSlider)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addToggle("bRLedButton", &rLedButton)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addTextArea("tRLedButton", aRLedButton+"     "+dRLedButton)->getRect()->setWidth(200);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addSpacer();
    
    gui->addTextArea("note", "*** append \"Velocity\" to OSC address");
    
    gui->autoSizeToFitWidgets();
}

//----------
void ofApp::addMantaStatToGui(string name, string address, bool *val, bool *vel) {
    gui->addToggle("b"+name, val)->setLabelVisible(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addToggle("b"+name+"V", vel)->setLabelVisible(false);
    gui->addTextArea("t"+name, address);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
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
    if      (e.getName() == "Velocity smoothness") {
        manta.setVelocityLerpRate(1.0-velocitySmoothness);
    }
    else if (e.getName() == "hostOut") {
        setupOscSender(guiHostIn->getTextString(), ofToInt(guiPortOut->getTextString()));
    }
    else if (e.getName() == "portOut") {
        setupOscSender(guiHostIn->getTextString(), ofToInt(guiPortOut->getTextString()));
    }
    else if (e.getName() == "portIn") {
        setupOscReceiver(ofToInt(guiPortIn->getTextString()));
    }
    else if (e.getName() == "pad format") {
        togglePadFormat();
    }
    else if (e.getName() == "bRLed") {
        oscIn = rLed;
    }
    /*
    else if (e.getName() == "bRLedPad") {
    }
    else if (e.getName() == "bRLedSlider") {
    }
    else if (e.getName() == "bRLedButton") {
    }
    */
}

//----------
void ofApp::guiOptionsEvent(ofxUIEventArgs &e) {
    if      (e.getName() == "osc info") {
        if (e.getButton()->getValue() == 1) return;
        view = 1-view;
        guiView->setLabelText(view == 0 ? "osc info" : "manta view");
        guiAnimated->setVisible(view == 0);
        gui->setVisible(view == 1);
    }
    else if (e.getName() == "animated") {
        manta.setAnimated(animated);
    }
}

//----------
void ofApp::update(){
    
    if (oscNumPads && numPads != manta.getNumPads()) numPads.set(manta.getNumPads());
    if (oscPadSum && padSum != manta.getPadSum()) padSum.set(manta.getPadSum());
    if (oscPadAverage && padAverage != manta.getPadAverage()) padAverage.set(manta.getPadAverage());
    if (oscCentroidX && centroidX != manta.getCentroidX()) centroidX.set(manta.getCentroidX());
    if (oscCentroidY && centroidY != manta.getCentroidY()) centroidY.set(manta.getCentroidY());
    if (oscWeightedCentroidX && weightedCentroidX != manta.getWeightedCentroidX()) weightedCentroidX.set(manta.getWeightedCentroidX());
    if (oscWeightedCentroidY && weightedCentroidY != manta.getWeightedCentroidY()) weightedCentroidY.set(manta.getWeightedCentroidY());
    if (oscAverageInterFingerDistance && averageInterFingerDistance != manta.getAverageInterFingerDistance()) averageInterFingerDistance.set(manta.getAverageInterFingerDistance());
    if (oscPerimeter && perimeter != manta.getPerimeter()) perimeter.set(manta.getPerimeter());
    if (oscArea && area != manta.getArea()) area.set(manta.getArea());
    if (oscPadWidth && padWidth != manta.getWidth()) padWidth.set(manta.getWidth());
    if (oscPadHeight && padHeight != manta.getHeight()) padHeight.set(manta.getHeight());
    if (oscWhRatio && whRatio != manta.getWidthHeightRatio()) whRatio.set(manta.getWidthHeightRatio());

    if (oscNumPadsV && numPadsVelocity != manta.getNumPadsVelocity())  numPadsVelocity.set(manta.getNumPadsVelocity());
    if (oscPadSumV && padSumVelocity != manta.getPadSumVelocity()) padSumVelocity.set(manta.getPadSumVelocity());
    if (oscPadAverageV && padAverageVelocity != manta.getPadAverageVelocity()) padAverageVelocity.set(manta.getPadAverageVelocity());
    if (oscCentroidXV && centroidVelocityX != manta.getCentroidVelocityX()) centroidVelocityX.set(manta.getCentroidVelocityX());
    if (oscCentroidYV && centroidVelocityY != manta.getCentroidVelocityY()) centroidVelocityY.set(manta.getCentroidVelocityY());
    if (oscWeightedCentroidXV && weightedCentroidVelocityX != manta.getWeightedCentroidVelocityX()) weightedCentroidVelocityX.set(manta.getWeightedCentroidVelocityX());
    if (oscWeightedCentroidYV && weightedCentroidVelocityY != manta.getWeightedCentroidVelocityY()) weightedCentroidVelocityY.set(manta.getWeightedCentroidVelocityY());
    if (oscAverageInterFingerDistanceV && averageInterFingerDistanceVelocity != manta.getAverageInterFingerDistanceVelocity()) averageInterFingerDistanceVelocity.set(manta.getAverageInterFingerDistanceVelocity());
    if (oscPerimeterV && perimeterVelocity != manta.getPerimeterVelocity()) perimeterVelocity.set(manta.getPerimeterVelocity());
    if (oscAreaV && areaVelocity != manta.getAreaVelocity()) areaVelocity.set(manta.getAreaVelocity());
    if (oscPadWidthV && widthVelocity != manta.getWidthVelocity()) widthVelocity.set(manta.getWidthVelocity());
    if (oscPadHeightV && heightVelocity != manta.getHeightVelocity()) heightVelocity.set(manta.getHeightVelocity());
    if (oscWhRatioV && whRatioVelocity != manta.getWidthHeightRatioVelocity()) whRatioVelocity.set(manta.getWidthHeightRatioVelocity());
    
    while (receiver.hasWaitingMessages())
    {
        try {
            ofxOscMessage msg;
            receiver.getNextMessage(&msg);
            string address = msg.getAddress();
            
            if (oscIn) {
                if      (address == aRLedPad) {
                    if (rLedPad) {
                        int row, col, value;
                        if (padFormat) {
                            row = msg.getArgAsInt32(0);
                            col = msg.getArgAsInt32(1);
                            value = msg.getArgAsInt32(2);
                        }
                        else {
                            row = floor(msg.getArgAsInt32(0) / 8);
                            col = msg.getArgAsInt32(0) % 8;
                            value = msg.getArgAsInt32(1);
                        }
                        if      (value == 0)
                            manta.setPadLedState(row, col, Manta::Off);
                        else if (value == 1)
                            manta.setPadLedState(row, col, Manta::Amber);
                        else if (value == 2)
                            manta.setPadLedState(row, col, Manta::Red);
                    }
                }
                else if (address == aRLedSlider) {
                    if (rLedSlider) {
                        int index = msg.getArgAsInt32(0);
                        int value = msg.getArgAsInt32(1);
                        manta.setSliderLedState(index, Manta::Amber, value);
                    }
                }
                else if (address == aRLedButton) {
                    if (rLedButton) {
                        int index = msg.getArgAsInt32(0);
                        int value = msg.getArgAsInt32(1);
                        if      (value == 0)
                            manta.setButtonLedState(index, Manta::Off);
                        else if (value == 1)
                            manta.setButtonLedState(index, Manta::Amber);
                        else if (value == 2)
                            manta.setButtonLedState(index, Manta::Red);
                    }
                }
            }
            
            if (address == aRLed) {
                rLed = msg.getArgAsInt32(0) == 1;
                oscIn = rLed;
                manta.setLedManual(rLed);
            }
        }
        catch (runtime_error &e) {
            ofLog(OF_LOG_ERROR, e.what());
        }
    }
}

//----------
void ofApp::togglePadFormat(){
    dOscPad = padFormat ? "(row,col,value)" : "(index,value)";
    dOscPadOn = padFormat ? "(row,col,value)" : "(index,value)";
    dOscPadOff = padFormat ? "(row,col,value)" : "(index,value)";
    dRLedPad = padFormat ? "(row,col,0/1/2)" : "(index,0/1/2)";
    buildGui();
}

//----------
void ofApp::draw(){
    ofBackground(100);

    if (view == 0) {
        manta.draw(5, 36, ofGetWidth() - 10);
        manta.drawStats(5, 350, ofGetWidth() - 10);
    }

    if (failedOscIn && failedOscOut) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofRect(0, 336, 410, 19);
        ofSetColor(255);
        ofDrawBitmapString("OSC sender & receiver failed to connect. ", 6, 350);
        ofPopStyle();
    }
    else if (failedOscIn) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofRect(0, 336, 410, 19);
        ofSetColor(255);
        ofDrawBitmapString("OSC receiver failed to connect. ", 6, 350);
        ofPopStyle();
    }
    else if (failedOscOut) {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofRect(0, 336, 410, 19);
        ofSetColor(255);
        ofDrawBitmapString("OSC sender failed to connect. ", 6, 350);
        ofPopStyle();
    }
}

//----------
void ofApp::exit() {
    saveSettings();
    manta.removePadListener(this, &ofApp::PadEvent);
    manta.removeSliderListener(this, &ofApp::SliderEvent);
    manta.removeButtonListener(this, &ofApp::ButtonEvent);
    manta.removePadVelocityListener(this, &ofApp::PadVelocityEvent);
    manta.removeButtonVelocityListener(this, &ofApp::ButtonVelocityEvent);
    manta.close();
    ofRemoveListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    ofRemoveListener(guiOptions->newGUIEvent, this, &ofApp::guiOptionsEvent);
    delete gui;
    delete guiOptions;
}

//----------
void ofApp::saveSettings() {
    ofXml xml;
    xml.addChild("Manta");
    xml.setTo("Manta");
    
    xml.addValue("host", host);
    xml.addValue("portIn", portIn);
    xml.addValue("portOut", portOut);
    
    xml.addValue("oscPad", oscPad);
    xml.addValue("oscPadOn", oscPadOn);
    xml.addValue("oscPadOff", oscPadOff);
    xml.addValue("oscSlider", oscSlider);
    xml.addValue("oscButton", oscButton);
    xml.addValue("oscButtonVelocity", oscButtonVelocity);
    
    xml.addValue("oscNumPads", oscNumPads);
    xml.addValue("oscPadSum", oscPadSum);
    xml.addValue("oscPadAverage", oscPadAverage);
    xml.addValue("oscCentroidX", oscCentroidX);
    xml.addValue("oscCentroidY", oscCentroidY);
    xml.addValue("oscWeightedCentroidX", oscWeightedCentroidX);
    xml.addValue("oscWeightedCentroidY", oscWeightedCentroidY);
    xml.addValue("oscAverageInterFingerDistance", oscAverageInterFingerDistance);
    xml.addValue("oscPerimeter", oscPerimeter);
    xml.addValue("oscArea", oscArea);
    xml.addValue("oscPadWidth", oscPadWidth);
    xml.addValue("oscPadHeight", oscPadHeight);
    xml.addValue("oscWhRatio", oscWhRatio);
    
    xml.addValue("oscNumPadsV", oscNumPadsV);
    xml.addValue("oscPadSumV", oscPadSumV);
    xml.addValue("oscPadAverageV", oscPadAverageV);
    xml.addValue("oscCentroidXV", oscCentroidXV);
    xml.addValue("oscCentroidYV", oscCentroidYV);
    xml.addValue("oscWeightedCentroidXV", oscWeightedCentroidXV);
    xml.addValue("oscWeightedCentroidYV", oscWeightedCentroidYV);
    xml.addValue("oscAverageInterFingerDistanceV", oscAverageInterFingerDistanceV);
    xml.addValue("oscPerimeterV", oscPerimeterV);
    xml.addValue("oscAreaV", oscAreaV);
    xml.addValue("oscPadWidthV", oscPadWidthV);
    xml.addValue("oscPadHeightV", oscPadHeightV);
    xml.addValue("oscWhRatioV", oscWhRatioV);
    
    xml.addValue("velocitySmoothness", velocitySmoothness);
    
    xml.addValue("rLed", rLed);
    xml.addValue("rLedPad", rLedPad);
    xml.addValue("rLedSlider", rLedSlider);
    xml.addValue("rLedButton", rLedButton);
    
    xml.setToParent();
    xml.save("settings.xml");
}

//----------
void ofApp::loadSettings() {
    ofXml xml;
    xml.load("settings.xml");
    xml.setTo("Manta");
    
    host = xml.getValue<string>("host");
    portIn = xml.getValue<int>("portIn");
    portOut = xml.getValue<int>("portOut");
    padFormat = xml.getValue<bool>("padFormat");

    oscPad = xml.getValue<bool>("oscPad");
    oscPadOn = xml.getValue<bool>("oscPadOn");
    oscPadOff = xml.getValue<bool>("oscPadOff");
    oscSlider = xml.getValue<bool>("oscSlider");
    oscButton = xml.getValue<bool>("oscButton");
    oscButtonVelocity = xml.getValue<bool>("oscButtonVelocity");
 
    oscNumPads = xml.getValue<bool>("oscNumPads");
    oscPadSum = xml.getValue<bool>("oscPadSum");
    oscPadAverage = xml.getValue<bool>("oscPadAverage");
    oscCentroidX = xml.getValue<bool>("oscCentroidX");
    oscCentroidY = xml.getValue<bool>("oscCentroidY");
    oscWeightedCentroidX = xml.getValue<bool>("oscWeightedCentroidX");
    oscWeightedCentroidY = xml.getValue<bool>("oscWeightedCentroidY");
    oscAverageInterFingerDistance = xml.getValue<bool>("oscAverageInterFingerDistance");
    oscPerimeter = xml.getValue<bool>("oscPerimeter");
    oscArea = xml.getValue<bool>("oscArea");
    oscPadWidth = xml.getValue<bool>("oscPadWidth");
    oscPadHeight = xml.getValue<bool>("oscPadHeight");
    oscWhRatio = xml.getValue<bool>("oscWhRatio");
    
    oscNumPadsV = xml.getValue<bool>("oscNumPadsV");
    oscPadSumV = xml.getValue<bool>("oscPadSumV");
    oscPadAverageV = xml.getValue<bool>("oscPadAverageV");
    oscCentroidXV = xml.getValue<bool>("oscCentroidXV");
    oscCentroidYV = xml.getValue<bool>("oscCentroidYV");
    oscWeightedCentroidXV = xml.getValue<bool>("oscWeightedCentroidXV");
    oscWeightedCentroidYV = xml.getValue<bool>("oscWeightedCentroidYV");
    oscAverageInterFingerDistanceV = xml.getValue<bool>("oscAverageInterFingerDistanceV");
    oscPerimeterV = xml.getValue<bool>("oscPerimeterV");
    oscAreaV = xml.getValue<bool>("oscAreaV");
    oscPadWidthV = xml.getValue<bool>("oscPadWidthV");
    oscPadHeightV = xml.getValue<bool>("oscPadHeightV");
    oscWhRatioV = xml.getValue<bool>("oscWhRatioV");
    
    velocitySmoothness = xml.getValue<float>("velocitySmoothness");
    manta.setVelocityLerpRate(1.0 - velocitySmoothness);
    
    rLed = xml.getValue<bool>("rLed");
    rLedPad = xml.getValue<bool>("rLedPad");
    rLedSlider = xml.getValue<bool>("rLedSlider");
    rLedButton = xml.getValue<bool>("rLedButton");
}

//----------
void ofApp::PadEvent(ofxMantaEvent & evt) {
    if (oscOut && oscPad) {
        ofxOscMessage msg;
        msg.setAddress(aOscPad);
        if (padFormat) {
            msg.addIntArg(evt.row);
            msg.addIntArg(evt.col);
        }
        else {
            msg.addIntArg(evt.id);
        }
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
    if (oscOut && oscPadOn && evt.value > 0) {
        ofxOscMessage msg;
        msg.setAddress(aOscPadOn);
        if (padFormat) {
            msg.addIntArg(evt.row);
            msg.addIntArg(evt.col);
        }
        else {
            msg.addIntArg(evt.id);
        }
        msg.addIntArg(evt.value);
        sender.sendMessage(msg);
        //cout << "Pad velocity event: id " << evt.id << ", row "<< evt.row <<", column "<< evt.col << ", value "<< evt.value << endl;
    }
    else if (oscOut && oscPadOff && evt.value == 0) {
        ofxOscMessage msg;
        msg.setAddress(aOscPadOff);
        if (padFormat) {
            msg.addIntArg(evt.row);
            msg.addIntArg(evt.col);
        }
        else {
            msg.addIntArg(evt.id);
        }
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
void ofApp::sendStatOSCMessage(string address, float value, bool isVelocity) {
    //cout << "send ("<<ofGetFrameNum()<<")" << address << " " << value << " VEL " << isVelocity << endl;
    ofxOscMessage msg;
    isVelocity ? msg.setAddress(address+"Velocity") : msg.setAddress(address);
    msg.addFloatArg(value);
    sender.sendMessage(msg);
}

void ofApp::changedNumPads(float &v) {
    sendStatOSCMessage(aOscNumPads, numPads, false);
}

void ofApp::changedPadSum(float &v) {
    sendStatOSCMessage(aOscPadSum, padSum, false);
}

void ofApp::changedPadAverage(float &v) {
    sendStatOSCMessage(aOscPadAverage, padAverage, false);
}

void ofApp::changedCentroidX(float &v) {
    sendStatOSCMessage(aOscCentroidX, centroidX, false);
}

void ofApp::changedCentroidY(float &v) {
    sendStatOSCMessage(aOscCentroidY, centroidY, false);
}

void ofApp::changedWeightedCentroidX(float &v) {
    sendStatOSCMessage(aOscWeightedCentroidX, weightedCentroidX, false);
}

void ofApp::changedWeightedCentroidY(float &v) {
    sendStatOSCMessage(aOscWeightedCentroidY, weightedCentroidY, false);
}

void ofApp::changedAverageInterFingerDistance(float &v) {
    sendStatOSCMessage(aOscAverageInterFingerDistance, averageInterFingerDistance, false);
}

void ofApp::changedPerimeter(float &v) {
    sendStatOSCMessage(aOscPerimeter, perimeter, false);
}

void ofApp::changedArea(float &v) {
    sendStatOSCMessage(aOscArea, area, false);
}

void ofApp::changedPadWidth(float &v) {
    sendStatOSCMessage(aOscPadWidth, padWidth, false);
}

void ofApp::changedPadHeight(float &v) {
    sendStatOSCMessage(aOscPadHeight, padHeight, false);
}

void ofApp::changedWhRatio(float &v) {
    sendStatOSCMessage(aOscWhRatio, whRatio, false);
}

void ofApp::changedNumPadsVelocity(float &v) {
    sendStatOSCMessage(aOscNumPads, numPadsVelocity, true);
}

void ofApp::changedPadSumVelocity(float &v) {
    sendStatOSCMessage(aOscPadSum, padSumVelocity, true);
}

void ofApp::changedPadAverageVelocity(float &v) {
    sendStatOSCMessage(aOscPadAverage, padAverageVelocity, true);
}

void ofApp::changedCentroidVelocityX(float &v) {
    sendStatOSCMessage(aOscCentroidX, centroidVelocityX, true);
}

void ofApp::changedCentroidVelocityY(float &v) {
    sendStatOSCMessage(aOscCentroidY, centroidVelocityY, true);
}

void ofApp::changedWeightedCentroidVelocityX(float &v) {
    sendStatOSCMessage(aOscWeightedCentroidX, weightedCentroidVelocityX, true);
}

void ofApp::changedWeightedCentroidVelocityY(float &v) {
    sendStatOSCMessage(aOscWeightedCentroidY, weightedCentroidVelocityY, true);
}

void ofApp::changedAverageInterFingerDistanceVelocity(float &v) {
    sendStatOSCMessage(aOscAverageInterFingerDistance, averageInterFingerDistanceVelocity, true);
}

void ofApp::changedPerimeterVelocity(float &v) {
    sendStatOSCMessage(aOscPerimeter, perimeterVelocity, true);
}

void ofApp::changedAreaVelocity(float &v) {
    sendStatOSCMessage(aOscArea, areaVelocity, true);
}

void ofApp::changedWidthVelocity(float &v) {
    sendStatOSCMessage(aOscPadWidth, widthVelocity, true);
}

void ofApp::changedHeightVelocity(float &v) {
    sendStatOSCMessage(aOscPadHeight, heightVelocity, true);
}

void ofApp::changedWhRatioVelocity(float &v) {
    sendStatOSCMessage(aOscWhRatio, whRatioVelocity, true);
}

//----------
void ofApp::windowResized(int w, int h) {
    ofHideCursor();
    ofShowCursor();
}
