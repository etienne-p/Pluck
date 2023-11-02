//
//  TickerGenerator.cpp
//  Harp2
//
//  Created by etienne cella on 2016-02-14.
//
//

#include "TickerGenerator.hpp"

void TickerGenerator::processAudio(float * output, int bufferSize)
{
    constexpr float audioRate = 44100.0f;
    constexpr float freq = 440.0f * 2.0;
    constexpr float periodInSamples = audioRate / freq;
    constexpr float dt = 2.0f * PI / periodInSamples;
    constexpr float envDuration = .03f;
    constexpr float dEnv = 1.0f / (envDuration * audioRate);
    
    for(auto i = 0; i < bufferSize; ++i)
    {
        t += dt;
        env = max(env - dEnv, .0f);
        output[i] += env * volume * sin(t);
    }
    
    dirty = env > .0f;
}

void TickerGenerator::reset()
{
    env = .0f;
    dirty = false;
}

void TickerGenerator::tick()
{
    t = .0f;
    env = 1.0f;
    dirty = true;
}

bool TickerGenerator::isDirty() const noexcept { return dirty; };