#pragma once

#include "ofMain.h"

#include "ofxSurfingDataRemover.h"

#include "ofxWindowApp.h"//->not required

class ofApp : public ofBaseApp {

public:

	ofxSurfingDataRemover dataRemover;

	void dragEvent(ofDragInfo dragInfo) override;

	void windowResized(int w, int h);

	ofxWindowApp windowApp;
};
