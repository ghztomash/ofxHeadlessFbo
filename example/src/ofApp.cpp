#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofUtils.h"
#include <chrono>

//--------------------------------------------------------------
void ofApp::setup(){
    size = (ofGetWidth() / 2.0) - 20;
    
    fbo.allocate(size, size, GL_RGB);
    hfbo.allocate(size, size, OF_PIXELS_RGB);
}

//--------------------------------------------------------------
void ofApp::update(){
    fbo.begin();
    t1 = std::chrono::high_resolution_clock::now();
    if (fill)
        ofFill();
    else
        ofNoFill();
    ofClear(ofColor::green);
    ofSetColor(ofColor::darkCyan);
    ofDrawTriangle( 0, size, size/2.0, 0, size, size);
    ofSetColor(ofColor::blue); 
    ofDrawRectangle(size/4.0,size/4.0,size/2.0,size/2.0);
    ofSetColor(ofColor::white);
    ofDrawCircle(size/2.0,size/2.0, size/4.0);
    ofDrawRectRounded(20, 20, size - 40, 50, 30);
    ofDrawEllipse(size/2.0, size - 40, size/2.0, 40);
    ofSetColor(ofColor::red);
    ofDrawLine(0, 0, size, size);
    if(followMouse)
        ofDrawLine(0, 0, mouseX, mouseY);
    ofDrawLine(size, 0, 0, size);
    t2 = std::chrono::high_resolution_clock::now();
    fbo.end();

    ms_fbo = t2-t1;

    t1 = std::chrono::high_resolution_clock::now();
    if (fill)
        hfbo.setFill();
    else
        hfbo.setNoFill();
    hfbo.clear(ofColor::red);
    hfbo.setColor(ofColor::darkCyan);
    hfbo.drawTriangle( 0, size, size/2.0, 0, size, size);
    hfbo.setColor(ofColor::blue);
    hfbo.drawRectangle(size/4.0,size/4.0,size/2.0,size/2.0);
    hfbo.setColor(ofColor::white);
    hfbo.drawCircle(size/2.0,size/2.0, size/4.0);
    hfbo.drawRectRounded(20, 20, size - 40, 50, 30);
    hfbo.drawEllipse(size/2.0, size - 40, size/2.0, 40);
    hfbo.setColor(ofColor::green);
    hfbo.drawLine(0, 0, size, size);
    if(followMouse)
        hfbo.drawLine(0, 0, mouseX, mouseY);
    hfbo.drawLine(size, 0, 0, size);
    t2 = std::chrono::high_resolution_clock::now();

    ms_hfbo = t2-t1;
}

//--------------------------------------------------------------
void ofApp::draw(){
    // draw the virtual FBO
    hfbo.draw(10, 10);
    // draw the reference FBO
    fbo.draw(size + 20, 10);

    ofSetColor(ofColor::white);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10);
    ofDrawBitmapString("Headless FBO: " + ofToString(ms_hfbo.count()) + "ms", 10, size + 30);
    ofDrawBitmapString("FBO: " + ofToString(ms_fbo.count()) + "ms", size + 20, size + 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    fill = false;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    fill = true;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    followMouse = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    followMouse = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
