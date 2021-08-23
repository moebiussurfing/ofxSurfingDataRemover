#pragma once

#include "ofMain.h"

#include "ofxSurfingDataRemover.h"

//#include "ofxWindowApp.h"

class ofApp : public ofBaseApp{

	public:

		ofxSurfingDataRemover dataRemover;
		void dragEvent(ofDragInfo dragInfo) override;
		void windowResized(int w, int h);

		//ofxWindowApp windowApp;
};
