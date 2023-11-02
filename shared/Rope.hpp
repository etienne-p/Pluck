//
//  Rope.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-16.
//
//

#pragma once

#include <array>
#include "ofMath.h"
#include "ofMain.h"
#include "Utils.hpp"
#include "InterpolatedProperty.h"
#include "Locator.h"
#include "Settings.h"
#include "Easing.h"

class RopeAudioSettings
{
public:
    
    void setSustain(float input);
    float getSustain() const noexcept;
    void setBrilliance(float input);
    float getBrilliance() const noexcept;
    
    float getDry() const noexcept;
    float getFeedback() const noexcept;
    float getLowpass() const noexcept;
    void setRawFrequency(float input);
    float getRawFrequency() const noexcept;
    Utils::Math::Note getNote() const noexcept;
    
    void copy(const RopeAudioSettings& other)
    {
        setSustain(other.getSustain());
        setBrilliance(other.getBrilliance());
        setRawFrequency(other.getRawFrequency());
    }
    
    RopeAudioSettings clone()
    {
        RopeAudioSettings rv;
        rv.copy(*this);
        return rv;
    }
    
private:
    float sustain{.5f};
    float brilliance{.5f};
    float frequency{220.0f};
};

class Rope
{
    
public:
    
    bool destroyScheduled{false};
    
    static constexpr auto NUM_PARTICLES = 48;
    static constexpr auto LAST_INDEX = NUM_PARTICLES - 1;
    static constexpr auto RESOLUTION = 8;
    static constexpr auto NUM_VERTICES = NUM_PARTICLES * 2 + 2 * (RESOLUTION + 1) + 2;
    static constexpr auto NUM_INDICES = ((NUM_PARTICLES - 1) * 2 + 2 * RESOLUTION) * 3;
    
    InterpolatedProperty anim; // TODO RENAME
    InterpolatedProperty ipSelected;
    
    Rope(int id_);
    int getId() const;
    
    int physicsUpdateRate{12};
    float lineWidth{1.0f};
    float coeff{0.995f};
    float strength{.495f};
    
    void update(float dt);
    void draw();
    void reset();
    void resume();
    void pause();
    
    void moveAnchor(ofVec3f position, size_t index);
    ofVec3f getAnchorPosition(size_t index) const noexcept;
    float getLength() const noexcept;
    RopeAudioSettings audioSettings;
    void syncAudioSettings();
    
    // TMP
    void pluck(float velocity = .5f);
    void pluck(const ofVec3f& origin, const ofVec3f& force);
    
    void setSelected(bool value);
    bool isSelected() const noexcept;
    
    bool isDestroyed() const noexcept;
    void destroy();
    
    ofColor getColor() const noexcept;
    
private:
    
    InterpolatedProperty ipPluck;
    
    bool destroyed{false};
    
    struct Particle
    {
        ofVec3f position, prevPosition;
        int axisIndexoffset;
        float mobility;
    };

    ofVec3f vertices[NUM_VERTICES];
    Particle particles[NUM_PARTICLES];
    float linkLength{1.0f};
    ofVbo vbo;
    int id;
    std::shared_ptr<ofCamera> camera;
    bool selected{false};
    
    void updatePhysics();
    void updateParticlesRange(int from, int to);
    void updateVertices();
    void computeIndices();
    void computeTexcoords();
    
};
