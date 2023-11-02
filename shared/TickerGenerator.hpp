//
//  TickerGenerator.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-14.
//
//

#pragma once

#include "ofMath.h"

class TickerGenerator
{
public:
    
    void processAudio(float * output, int bufferSize);
    void tick();
    void reset();
    bool isDirty() const noexcept;
private:
    float t{.0f};
    float env{.0f};
    float volume{.9f};
    bool dirty{false};
    
};