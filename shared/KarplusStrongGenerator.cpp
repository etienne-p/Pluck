//
//  KarplusStrongGenerator.cpp
//  Harp2
//
//  Created by etienne cella on 2016-02-12.
//
//

#include "KarplusStrongGenerator.hpp"

KarplusStrongGenerator::KarplusStrongGenerator()
{

}

/*
void KarplusStrongGenerator::setAlpha(float val)
{
    alpha = ofClamp(val, .0f, 1.0f);
}

void KarplusStrongGenerator::setDry(float val)
{
    dry = ofClamp(val, .0f, 1.0f);
}

void KarplusStrongGenerator::setFeedback(float val)
{
    feedback = ofClamp(val, .0f, .999f); // NOT up to one
}
*/

void KarplusStrongGenerator::reset()
{
    std::fill(std::begin(wave), std::end(wave), .0f);
    dirty = false;
}

void KarplusStrongGenerator::processAudio(float* output, int bufferSize)
{
    int prevIndex = index % delay;
    float fdry = feedback * dry;
    float flp = feedback * (1.0f - dry);
    float sum = 0.0;
    
    for (int i = 0; i < bufferSize; ++i)
    {
        index = (prevIndex + 1) % delay;
        wave[index] = fdry * wave[index] + flp * (alpha * wave[index] + (1.0f - alpha) * wave[prevIndex]);
        sum += fabs(output[i] += mul * wave[index]);
        prevIndex = index;
    }
    dirty = sum > 0.001f;
}

void KarplusStrongGenerator::pluck(const RopeAudioSettings& settings, float velocity)
{
    alpha = settings.getLowpass();
    dry = settings.getDry();
    feedback = settings.getFeedback(); // NOT up to one
    const auto noise = settings.getBrilliance();

    delay = std::min<int>(2048, (int) floorf(44100.0f / settings.getNote().frequency));
    // something more complex, noise * saw
    //const auto burstLength = std::max<int>(1, delay * .5);
    for (int i = 0; i < delay; ++i)
    {
        wave[i] = velocity *
        (noise * ofRandomf() + sin((float)i / (float)delay * M_2_PI));
    }
    dirty = true;
}

bool KarplusStrongGenerator::isDirty() const noexcept { return dirty; };
