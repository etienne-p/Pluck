//
//  AudioEngine.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-12.
//
//

#pragma once

#include <array>
#include "KarplusStrongGenerator.hpp"
#include "TickerGenerator.hpp"
#include "Rope.hpp"

class AudioEngine
{
public:
    
    static constexpr size_t GENERATORS_COUNT = 8;
    
    void processAudio(float * output, int bufferSize, int nChannels);
    
    // proxy method as the audio engine is responsible for prividing a generator
    void pluck(const RopeAudioSettings& settings, float velocity, int id);
    
    void tick();
    
    void reset(int bufferSize);
    
    const vector<float>& getBuffer() const noexcept;
    
private:
    std::list<int> indexes;
    std::array<KarplusStrongGenerator, GENERATORS_COUNT> generators;
    TickerGenerator tickerGenerator;
    vector<float> buffer;
};
