//
//  Rope.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-16.
//
//

#include "Rope.hpp"

//void RopeAudioSettings::setVolume(float input){ volume = ofClamp(input, .0f, 1.0f); }

//float RopeAudioSettings::getVolume() const noexcept { return volume; }

void RopeAudioSettings::setSustain(float input) { sustain = ofClamp(input, .0f, 1.0f); }
float RopeAudioSettings::getSustain() const noexcept { return sustain; };

void RopeAudioSettings::setBrilliance(float input) { brilliance = ofClamp(input, .0f, 1.0f); }
float RopeAudioSettings::getBrilliance() const noexcept { return brilliance; };

float RopeAudioSettings::getDry() const noexcept
{
    return brilliance * .4f;//ofClamp(brilliance, .05f, .8f);
}

float RopeAudioSettings::getFeedback() const noexcept
{
    auto easedValue = QuartEaseOut()(EasingArgs(sustain, .0f, 1.0f, 1.0f));
    return .999f - (1.0f - easedValue) * .1f;
}

float RopeAudioSettings::getLowpass() const noexcept
{
    return .3f + brilliance * .4f;
}

void RopeAudioSettings::setRawFrequency(float input) { frequency = ofClamp(input, 60.0f, 3000.0f); }

float RopeAudioSettings::getRawFrequency() const noexcept { return frequency; };

Utils::Math::Note RopeAudioSettings::getNote() const noexcept { return Utils::Math::snapToNote(frequency); }

Rope::Rope(int id_)
{
    id = id_;
    anim = 1.0f;
    ipSelected = .0f;
    camera = Locator::get<ofCamera>();
    resume();
}

void Rope::resume()
{
    vbo.setVertexData(&vertices[0], NUM_VERTICES, GL_DYNAMIC_DRAW);
    computeIndices();
    computeTexcoords();
    reset();
}

void Rope::pause()
{
    vbo.clear();
}

void Rope::computeIndices()
{
    std::array<ofIndexType, NUM_INDICES> indices;
    auto index = 0, i = 0;
    
    for (i = 0; i < NUM_PARTICLES - 1; ++i)
    {
        indices[index++] = i * 2;
        indices[index++] = i * 2 + 2;
        indices[index++] = i * 2 + 3;
        
        indices[index++] = i * 2 + 3;
        indices[index++] = i * 2 + 1;
        indices[index++] = i * 2;
    }
    
    auto offset = 2 * NUM_PARTICLES;
    
    // Top & Bottom
    for (i = 0; i < 2; ++i)
    {
        for (auto j = 1; j < RESOLUTION + 1; ++j)
        {
            indices[index++] = offset + j;
            indices[index++] = offset;
            indices[index++] = offset + ((j + 1) % (RESOLUTION + 2));
        }
        offset += RESOLUTION + 2;
    }
    
    assert(index == NUM_INDICES);
    
    //vbo.enableIndices();
    vbo.setIndexData(&indices[0], NUM_INDICES, GL_STATIC_DRAW);
    
}

void Rope::computeTexcoords()
{
    auto index = 0;
    auto i = 0;
    std::array<ofVec2f, NUM_VERTICES> texCoords; // 1 texCoord per vertice
    
    for (i = 0; i < NUM_PARTICLES; ++i)
    {
        texCoords[index++] = ofVec2f( .0f, .5f);
        texCoords[index++] = ofVec2f(1.0f, .5f);
    }
    
    const auto center = ofVec2f(.5f, .5f);
    
    texCoords[index++] = center;
    
    for (i = 0; i < RESOLUTION + 1; ++i)
    {
        const auto angle = PI * (1.0f + (float)i / (float)(RESOLUTION));
        texCoords[index++] = center + ofVec2f(cos(angle), sin(angle)) * .5f;
    }
    
    texCoords[index++] = center;
    
    for (i = 0; i < RESOLUTION + 1; ++i)
    {
        const auto angle = PI * ((float)i / (float)(RESOLUTION));
        texCoords[index++] = center + ofVec2f(cos(angle), sin(angle)) * .5f;
    }
    
    assert(index == NUM_VERTICES);
    
    //vbo.enableTexCoords();
    vbo.setTexCoordData(&texCoords[0], NUM_VERTICES, GL_STATIC_DRAW);
}

int Rope::getId() const { return id; };

void Rope::reset()
{
    ipPluck = .0f;
    const auto anchorA = particles[0].position;
    const auto anchorB = particles[NUM_PARTICLES - 1].position;
    for (auto i = 0; i < NUM_PARTICLES; ++i)
    {
        particles[i].prevPosition = particles[i].position =
        (anchorB - anchorA) * (float)i / (float)(NUM_PARTICLES - 1) + anchorA;
        particles[i].axisIndexoffset = i == NUM_PARTICLES - 1 ? -1 : 0;
        particles[i].mobility = i == 0 || i == NUM_PARTICLES - 1 ? .0f : 1.0f;
    }
}

void Rope::update(float dt)
{
    anim.update(dt);
    ipPluck.update(dt);
    ipSelected.update(dt);
    for (auto i = 0; i < physicsUpdateRate; ++i) updatePhysics();
    updateVertices();
}

void Rope::draw()
{
    vbo.drawElements(GL_TRIANGLES, NUM_INDICES);
}

float Rope::getLength() const noexcept
{
    return (particles[NUM_PARTICLES - 1].position - particles[0].position).length();
}

void Rope::syncAudioSettings()
{
    audioSettings.setRawFrequency(Utils::Math::lengthTofreq(getLength()));
}

// TMP
void Rope::pluck(const ofVec3f& origin, const ofVec3f& force)
{
    ipPluck = 1.0f;
    ipPluck.to(.0f, 3.0f, Linear());
    for (auto i = 1; i < NUM_PARTICLES - 1; ++i)
    {
        particles[i].position += force / max(1.0f, (particles[i].position - origin).lengthSquared());
        particles[i].prevPosition = particles[i].position;
    }
}

void Rope::pluck(float velocity)
{
    auto origin = particles[NUM_PARTICLES / 2].position;
    auto dir = particles[NUM_PARTICLES - 1].position - particles[0].position;
    auto force = origin;
    force.cross(dir);
    force.normalize();
    force *= 10.0f * velocity;
    pluck(origin, force);
}

void Rope::moveAnchor(ofVec3f position, size_t index)
{
    switch(index)
    {
        case 0: particles[0].position = particles[0].prevPosition = position; break;
        case 1: particles[NUM_PARTICLES - 1].position = particles[NUM_PARTICLES - 1].prevPosition = position; break;
    }
    linkLength = (particles[0].position - particles[NUM_PARTICLES - 1].position).length() / (float)(NUM_PARTICLES - 1) * .9f; // .9f to put a bit of tension
    syncAudioSettings();
}

ofVec3f Rope::getAnchorPosition(size_t index) const noexcept
{
    switch(index)
    {
        case 0: return particles[0].position;
        case 1:
        default:
            return particles[NUM_PARTICLES - 1].position;
    }
}

void Rope::updatePhysics()
{
    updateParticlesRange(1, NUM_PARTICLES);
    updateParticlesRange(NUM_PARTICLES - 2, -1);
}

void Rope::updateParticlesRange(int from, int to)
{
    const auto d = from < to ? 1 : -1;
    
    for (auto i = from; i != to; i = i + d)
    {
        const auto tmp = particles[i].position * (1 + coeff) - particles[i].prevPosition * coeff;
        
        particles[i].prevPosition = particles[i].position;
        
        particles[i].position = tmp;
        
        const auto delta = particles[i].position - particles[i - d].position;
        
        const auto deltaLength = delta.length();
        
        const auto ratio = deltaLength > 0 ? (deltaLength - linkLength) / deltaLength : deltaLength;
        
        particles[i - d].position += delta * ratio * strength * particles[i - d].mobility;
        
        particles[i].position -= delta * ratio * strength * particles[i].mobility;
    }
}

void Rope::updateVertices()
{
    const auto MID_POINT = particles[0].position + (particles[NUM_PARTICLES - 1].position - particles[0].position) * .5f;
    // ropes as 2d shapes facing camera
    const auto NORMAL = (MID_POINT - camera->getPosition()).normalize();
    const float lineWidthMul = anim;
    
    auto index = 0;
    
    for (auto i = 0; i < NUM_PARTICLES; ++i)
    {
        const auto axis =
        particles [i + particles [i].axisIndexoffset].position -
        particles [i + 1 + particles [i].axisIndexoffset].position;
        
        const auto extrude = axis.getCrossed(NORMAL).normalize();
        
        vertices[index++] = particles [i].position + extrude * lineWidth * lineWidthMul * .5f;
        vertices[index++] = particles [i].position + ofQuaternion(180.0f, axis) * extrude * lineWidth * lineWidthMul * .5f;
    }
    
    // Top
    {
        vertices[index++] = particles [0].position;
        
        const auto axis = particles [0].position - particles [1].position;
        const auto extrude = axis.getCrossed(NORMAL).normalize();
        const auto axisR = extrude.getCrossed(axis).normalize();
        
        for (auto j = 0; j < RESOLUTION + 1; ++j)
        {
            vertices[index++] = particles [0].position + \
            ofQuaternion (180.0f * (float)j / (float)(RESOLUTION), axisR) * extrude * lineWidth * lineWidthMul * .5f;
        }
    }
    
    // Bottom
    {
        vertices[index++] = particles [NUM_PARTICLES - 1].position;
        
        const auto axis = particles [NUM_PARTICLES - 1].position - particles [NUM_PARTICLES - 2].position;
        const auto extrude = axis.getCrossed(NORMAL).normalize();
        const auto axisR = extrude.getCrossed(axis).normalize();
        
        for (auto j = 0; j < RESOLUTION + 1; ++j)
        {
            vertices[index++] = particles [NUM_PARTICLES - 1].position + \
            ofQuaternion(180.0f * (float)j / (float)(RESOLUTION), axisR) * extrude * lineWidth * lineWidthMul * .5f;
        }
    }
    
    assert(index == NUM_VERTICES);
    
    //vbo.bind();
    vbo.updateVertexData(&vertices[0], NUM_VERTICES);
    //vbo.unbind();
}

void Rope::setSelected(bool value)
{
    selected = value;
    if (value) ipSelected.to(1.0f, .2f, Linear());
    else ipSelected.to(.0f, .2f, Linear());
}

bool Rope::isSelected() const noexcept { return selected; }

bool Rope::isDestroyed() const noexcept { return destroyed; }

void Rope::destroy() { destroyed = true; }

ofColor Rope::getColor() const noexcept
{
    auto c = Settings::getMediumColor();
    auto ratio = ofClamp(audioSettings.getRawFrequency() / 600.0f, .0f, 1.0f);
    
    c.setSaturation( ofLerp(c.getSaturation(), c.limit(), (float)ipPluck));
    c.setBrightness( ofLerp(c.getBrightness(), c.limit(), (float)ipPluck));
    const auto hue = fmod(c.getHue() +
                          c.limit() *
                            (Settings::getRopeHueOffset() + (ratio - .5f) * Settings::getRopeHueRange()),
                          c.limit());
    c.setHue(hue);
    return c;
};

