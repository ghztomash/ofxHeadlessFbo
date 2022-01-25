#pragma once

#include "ofMain.h"
#include "ofxHeadlessFbo.h"
#include <chrono>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        int size = 400;

        bool fill = true;
        bool followMouse = false;

        ofxHeadlessFbo hfbo;
        ofFbo fbo;

        //for benchmark
        std::chrono::high_resolution_clock::time_point t1;
        std::chrono::high_resolution_clock::time_point t2;
        std::chrono::duration<double, std::milli> ms_fbo;
        std::chrono::duration<double, std::milli> ms_hfbo;
};
