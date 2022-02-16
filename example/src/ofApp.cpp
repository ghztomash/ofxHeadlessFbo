/*
Software License Agreement (BSD License)

Copyright (c) 2022 Tomash GHz.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofGraphics.h"
#include "ofUtils.h"
#include <chrono>

//--------------------------------------------------------------
void ofApp::setup(){
    size = (ofGetWidth() / 2.0) - 20;
    
    // allocate FBO buffers
    fbo.allocate(size, size, GL_RGB);
    hfbo.allocate(size, size, OF_PIXELS_RGB);
}

//--------------------------------------------------------------
void ofApp::update(){

    // draw in refference FBO
    t1 = std::chrono::high_resolution_clock::now(); // start benchmark timer
    fbo.begin();

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
    if(followMouse)
        ofDrawLine(0, 0, mouseX, mouseY);

    fbo.end();
    t2 = std::chrono::high_resolution_clock::now(); // end bemchmark timer
    ms_fbo = t2-t1;

    // draw in headless FBO
    t1 = std::chrono::high_resolution_clock::now(); // start benchmark timer

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
    if(followMouse)
        hfbo.drawLine(0, 0, mouseX, mouseY);

    t2 = std::chrono::high_resolution_clock::now(); // end benchmark timer
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
    if(key == 'f')
        fill = false;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'f')
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
