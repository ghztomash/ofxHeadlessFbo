#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"

//--------------------------------------------------------------
void ofApp::setup(){

    size = (ofGetWidth() / 2.0) - 20;
    
    fbo.allocate(size, size, GL_RGB);
    pix.allocate(size, size, OF_PIXELS_RGB);

    fbo.begin();
    ofClear(ofColor::green);
    ofSetColor(ofColor::darkCyan);
    ofDrawTriangle( 0, size, size/2.0, 0, size, size);
    ofSetColor(ofColor::blue); 
    ofDrawRectangle(size/4.0,size/4.0,size/2.0,size/2.0);
    ofSetColor(ofColor::white);
    ofDrawCircle(size/2.0,size/2.0, size/4.0);
    ofDrawRectRounded(20, 20, size - 40, 50, 30);
    ofSetColor(ofColor::red);
    ofDrawLine(0, 0, size, size);
    ofDrawLine(size, 0, 0, size);
    fbo.end();

    hfbo.allocate(size, size, OF_PIXELS_RGB);
    hfbo.clear(ofColor::red);
    hfbo.setColor(ofColor::darkCyan);
    hfbo.drawTriangle( 0, size, size/2.0, 0, size, size);
    hfbo.setColor(ofColor::blue);
    hfbo.drawRectangle(size/4.0,size/4.0,size/2.0,size/2.0);
    hfbo.setColor(ofColor::white);
    hfbo.drawCircle(size/2.0,size/2.0, size/4.0);
    //hfbo.setNoFill();
    hfbo.drawRectRounded(20, 20, size - 40, 50, 30);
    hfbo.setColor(ofColor::green);
    hfbo.drawLine(0, 0, size, size);
    hfbo.drawLine(size, 0, 0, size);

    //fbo.readToPixels(pix);
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
    fbo.draw(size + 20, 10);
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
