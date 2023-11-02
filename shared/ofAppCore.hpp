//
//  ofAppCore.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-16.
//
//

#pragma once

#include <stdio.h>
#include <stack>
#include <memory>

#include "ofMain.h"
#include "ofCamera.h"
#include "Fonts.hpp"
#include "UI.hpp"
#include "RopesController.hpp"
#include "UIBaseObject.hpp"
#include "Locator.h"
#include "ImageAssets.hpp"
#include "Settings.h"
#include "Sequencer.hpp"
#include "AudioEngine.hpp"
#include "SoundVisualizer.hpp"
#include "Library.hpp"
#include "Utils.hpp"

//#include "UISettings.hpp"

//#define draw_fingers

class ofAppCore
{
    
public:
    
    ofAppCore();
    ~ofAppCore();
    
    void setup();
    void update();
    void draw();
    
    void windowResized(int w, int h);
    
    void keyPressed(int key);

    bool handleTouchEvent(ofTouchEventArgs &touch);
    
    void audioRequested(float * output, int bufferSize, int nChannels);
    
    void saveScreenshot(const std::string& path);
    
    void resume(); //void onGLReset();
    void pause();
    
private:
    
    void draw3DWorld(float width, float height);
    void drawUI();
    
    void handleTick();
    
    //std::mutex touchMutex;
    stack<TouchEvent> touches;
    std::shared_ptr<Sequencer> sequencer;
    std::shared_ptr<ofCamera> camera;
    std::shared_ptr<RopesController> ropesController;
    std::shared_ptr<Library> library;
    std::shared_ptr<ImageAssets> imageAssets;
    
#ifdef draw_fingers
    std::map<int, ofVec2f> fingers;
#endif

    ofShader ropeShader;
    std::unique_ptr<UI> ui;
    ofFbo fbo;
    //ofFbo uiFbo;
    //RingBuffer<float, 2048> ringBuffer;
    AudioEngine audioEngine;
    SoundVisualizer soundVis;
    //UIAntilias antialias;
    
    //UISettings uiSettings;
    
};
