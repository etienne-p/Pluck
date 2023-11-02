//
//  AudioEngine.cpp
//  Harp2
//
//  Created by etienne cella on 2016-02-12.
//
//

#include "AudioEngine.hpp"


const vector<float>& AudioEngine::getBuffer() const noexcept { return buffer; };

void AudioEngine::pluck(const RopeAudioSettings& settings, float velocity, int id)
{
    // do we have an active generator with that id?
    
    auto index = -1, i = 0;
    // attempt to reuse the generator prviously used for that rope
    for (auto& gen : generators)
    {
        if (gen.isDirty() && gen.id == id)
        {
            index = i;
            break;
        }
        ++i;
    }
    
    if (index == -1)
    {
        index = indexes.front();
    }
    indexes.remove(index);
    indexes.push_back(index);
    generators[index].id = id;
    generators[index].pluck(settings, velocity);
}

void AudioEngine::tick()
{
    tickerGenerator.tick();
}

void AudioEngine::reset(int bufferSize)
{
    indexes.empty();
    for (auto i = 0; i < GENERATORS_COUNT; ++i) indexes.push_back(i);
    buffer.resize(bufferSize);
    std::fill(std::begin(buffer), std::end(buffer), .0f);
    for (auto& gen : generators) gen.reset();
    tickerGenerator.reset();
}

void AudioEngine::processAudio(float * output, int bufferSize, int nChannels)
{
    std::fill(std::begin(buffer), std::end(buffer), .0f);
    
    for (auto& gen : generators)
    {
        if (gen.isDirty()) gen.processAudio(&buffer[0], bufferSize);
    }
    
    if (tickerGenerator.isDirty()) tickerGenerator.processAudio(&buffer[0], bufferSize);
    
    for (int i = 0; i < bufferSize; ++i)
    {
        output[i * nChannels] = output[i * nChannels + 1] = buffer[i] * .2f; // mono to stereo
    }
}