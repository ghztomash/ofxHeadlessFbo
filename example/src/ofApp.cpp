#include "ofApp.h"
#include "ofGraphics.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    fbo.allocate(100, 100, GL_RGB);
    pix.allocate(100, 100, OF_PIXELS_RGB);
    tex.allocate(100, 100, GL_RGB);

    fbo.begin();
    ofClear(ofColor::green);
    ofSetColor(ofColor::darkCyan);
    ofDrawTriangle( 0, 100, 50, 0, 100, 100);
    ofSetColor(ofColor::blue);
    ofDrawRectangle(25,25,50,50);
    ofSetColor(ofColor::red);
    ofDrawLine(0, 0, 100, 100);
    ofDrawLine(1, 1, 1, 100);
    ofDrawLine(1, 1, 100, 1);
    fbo.end();

    hfbo.allocate(100, 100, OF_PIXELS_RGB);
    hfbo.clear(ofColor::red);
    hfbo.setColor(ofColor::darkCyan);
    hfbo.drawTriangle( 0, 100, 50, 0, 100, 100);
    hfbo.setColor(ofColor::blue);
    hfbo.drawRectangle(25,25,50,50);
    hfbo.setColor(ofColor::green);
    hfbo.drawLine(0, 0, 100, 100);
    hfbo.drawLine(1, 1, 1, 100);
    hfbo.drawLine(1, 1, 100, 1);

    fbo.readToPixels(pix);
    //hfbo.setFromPixels(pix, 100, 100, OF_PIXELS_RGB);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    // draw the virtual FBO
    hfbo.draw(10, 10);
    // draw the reference FBO
    fbo.draw(120, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
