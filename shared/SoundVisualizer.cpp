//
//  SoundVisualizer.cpp
//  Harp2
//
//  Created by etienne cella on 2016-02-14.
//
//

#include "SoundVisualizer.hpp"

void SoundVisualizer::setup()
{
    //ofLoadImage(noiseTx, "perlin.png");
    
    ofPixels noisePx;
    noisePx.allocate(512, 512, OF_IMAGE_GRAYSCALE);
    computeNoise(noisePx, 12.0f, 12.0f);
    noiseTx.setFromPixels(noisePx);
    //noiseTx.load("perlin.png");
    
    resume();
}

void SoundVisualizer::resume()
{
    shader.load(Utils::Platform::getShaderPath("sndviz.vert"),
                Utils::Platform::getShaderPath("sndviz.frag"));
    
    //
    const auto texWidth = 1;
    const auto texHeight = 1024;
    
    ofFbo::Settings settings;
    settings.useDepth = false;
    settings.useStencil = false;
    settings.numSamples = 0;
    settings.width = texWidth;
    settings.height = texHeight;
    settings.internalformat = GL_RGB;
    
    for (auto i = 0; i < 2; ++i)
    {
        fbos[i].allocate(settings);
        //fbos[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        fbos[i].begin();
        ofClear(0);
        fbos[i].end();
    }
    
    source = &fbos[0];
    dest = &fbos[1];
    
    const auto w = 1.0f;
    const auto h = 1.0f;
    
    quad.addVertex(ofVec2f(0, 0));
    quad.addTexCoord(ofVec2f(0, 0));
    quad.addVertex(ofVec2f(w, 0));
    quad.addTexCoord(ofVec2f(1, 0));
    quad.addVertex(ofVec2f(w, h));
    quad.addTexCoord(ofVec2f(1, 1));
    quad.addVertex(ofVec2f(0, h));
    quad.addTexCoord(ofVec2f(0, 1));
    quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
}

void SoundVisualizer::pause()
{
    shader.unload();
    //noiseTx.clear();
    quad.clear();
    for (auto i = 0; i < 2; ++i) fbos[i].clear();
}

void SoundVisualizer::update(float dt, bool trigger)
{
    // noise position
    // moving back & forth so it loops
    noisePosition = fmod(noisePosition + noiseSpeed * dt, 1.0f);
    
    std::swap(source, dest);
    
    dest->begin();
    
    ofClear(0);
    ofSetColor(ofColor::white);
    source->getTexture().draw(0, speed * dt * dest->getHeight());
    
    // add a new wave
    if (trigger)
    {
        ofDrawRectangle(0, 0, dest->getWidth(), 1.0f);
        ofSetColor(ofColor::black);
        ofDrawRectangle(0, 1, dest->getWidth(), 2.0f);
    }
    
    dest->end();
}

void SoundVisualizer::draw(float width, float height)
{
    shader.begin();
    shader.setUniformTexture("tex0", dest->getTexture(), 1);
    shader.setUniformTexture("texPerlin", noiseTx, 2);
    shader.setUniform1f("noisePosition", noisePosition);
    shader.setUniform1f("noiseMul", noiseMul);
    ofPushMatrix();
    ofScale(width, height);
    quad.draw();
    ofPopMatrix();
    shader.end();
    
    //noiseTx.draw(0, 0, width * .5f, height * .5f);
    
    //ofSetColor(ofColor::white);

    //source->getTexture().draw(0, 0);
    //dest->getTexture().draw(420, 0);
}

void SoundVisualizer::computeNoise(ofPixels& pxs, float scaleX, float scaleY)
{
    const auto w = pxs.getWidth();
    const auto h = pxs.getHeight();
    for (int i = 0; i < h; i++)
    {
        const auto y = (float)i / (float)(h - 1);
        for (int j = 0; j < w; j++)
        {
            const auto x = (float)j / (float)(w - 1);
            const auto noise = ofNoise(x * scaleX, y * scaleY) *
                                ofNoise(x * scaleX * 4.0f, y * scaleY * 4.0f) * 255.0f;
            //pxs.setColor(j, i, ofColor(noise, noise, noise));
            pxs[i * w + j] = noise;
        }
    }
}