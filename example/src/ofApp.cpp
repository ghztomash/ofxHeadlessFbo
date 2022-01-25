#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    fbo.allocate(100, 100, GL_RGB);
    pix.allocate(100, 100, OF_PIXELS_RGB);
    tex.allocate(100, 100, GL_RGB);

    fbo.begin();
    ofClear(ofColor::red);
    fbo.end();

    hfbo.allocate(100, 100, OF_PIXELS_RGB);
    hfbo.setColor(ofColor::red);
    hfbo.setBackground(ofColor::red);
}

//--------------------------------------------------------------
void ofApp::update(){
    hfbo.readPixels(pix);
    tex.allocate(pix);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // draw the virtual FBO
    tex.draw(10, 10);
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
