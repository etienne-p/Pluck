#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	core.setup();
	core.windowResized(ofGetScreenWidth(), ofGetScreenHeight());
	soundStream.setup(this, 2, 0, 44100, Settings::BUFFER_SIZE, 4);
}

//--------------------------------------------------------------
void ofApp::update()
{
	core.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	core.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	core.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ }

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	core.pause();
	core.resume();
	core.windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){ }

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){ }

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){ }

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){ }

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){ }

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){ }

//--------------------------------------------------------------
void ofApp::pause()
{
	core.pause();
}

//--------------------------------------------------------------
void ofApp::stop(){ }

//--------------------------------------------------------------
void ofApp::resume()
{
	ofLogNotice("resume ofapp");
	//core.resume();
}

//--------------------------------------------------------------
void ofApp::reloadTextures()
{
	ofLogNotice("reloadTextures ofapp");

}

//--------------------------------------------------------------
bool ofApp::backPressed(){ return false; }

//--------------------------------------------------------------
void ofApp::okPressed(){ }

//--------------------------------------------------------------
void ofApp::cancelPressed(){ }

void ofApp::audioRequested(float *output, int bufferSize, int nChannels)
{
    core.audioRequested(output, bufferSize, nChannels);
}

void ofApp::audioReceived(float * input,int bufferSize,int nChannels){}
