#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo info) {

	dataRemover.dragEvent(info);
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

	dataRemover.windowResized(w, h);
}