//
//  KarplusStrongGenerator.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-12.
//
//

#pragma once

#include <algorithm>
#include <math.h>
#include <array>
#include "ofMain.h"
#include "Rope.hpp"

class KarplusStrongGenerator
{
    
public:
    
    int id{-1};
    
    void processAudio(float * output, int bufferSize);
    void pluck(const RopeAudioSettings& settings, float velocity);
    
    KarplusStrongGenerator();
    
    bool isDirty() const noexcept;
    
    /*
    void setAlpha(float val);
    void setDry(float val);
    void setFeedback(float val);
     */
    
    void reset();
    
private:
    
    float alpha{.5f}; // low pass
    float dry{.5f}; // low pass VS dry balance
    float feedback{.98f};
    
    int delay{400};
    float mul{1.0f}; // = volume
    bool dirty{false};
    int index{0};
    std::array<float, 2048> wave;
};