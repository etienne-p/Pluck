//
//  SoundVisualizer.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-14.
//
//

#pragma once

#include "Utils.hpp"
#include "ofMain.h"

class SoundVisualizer
{
public:
    
    void setup();
    void resume();
    void pause();
    void update(float dt, bool trigger);
    void draw(float width, float height);
    
private:
    
    void computeNoise(ofPixels& pxs, float scaleX, float scaleY);
    
    float speed{.8f};
    float noiseSpeed{.1f};
    float noiseMul{.2f};
    
    ofShader shader;
    ofVboMesh quad;
    ofFbo fbos[2];
    ofFbo* source;
    ofFbo* dest;
    ofImage noiseTx;
    bool fboToggle{true};
    float noisePosition{.0f};
};
