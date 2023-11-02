//
//  RopesController.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-24.
//
//

#pragma once

#include <map>
#include <vector>
#include <array>
#include <queue>
#include "Graphics.hpp"
#include "Rope.hpp"
#include "Utils.hpp"
#include "nano_signal_slot.hpp"
#include "UISoundControls.hpp"
#include "UIBaseObject.hpp"
#include "InterpolatedProperty.h"
#include "Pool.cc"
#include "Locator.h"
#include "Utils.hpp"
#include "Fonts.hpp"
#include "UITextBox.hpp"
#include "Poco/Timestamp.h"
#include "Sequencer.hpp"
#include "AudioEngine.hpp"

class CreateRopeControl : public UIBaseObject
{
public:
    bool destroyed{false};
    ofVec2f from;
    ofVec2f to;
    InterpolatedProperty ipDisplay;
    CreateRopeControl();
    void update(float dt);
    void draw();
    UITextBox textBox;
};

class MovingRopeControl : public UIBaseObject
{
public:
    bool destroyed{false};
    int dragExtremity;
    int ropeId;
    InterpolatedProperty ipDisplay;
    MovingRopeControl();
    void update(float dt);
    void draw();
    UITextBox textBox;
};

class RopesController : public UIBaseObject
{
public:
    
    static constexpr int MAX_TOUCHES = 5;
    
    enum State { NONE, EDIT, PLAY, DELETE, SETTINGS };
    
    //Nano::Signal<void(Rope*, float)> onPluck;
    Nano::Signal<void(State)> onStateChanged;

    int eventPriority{1};
    
    RopesController(AudioEngine* audioEngine_);
    ~RopesController();
    
    void setState(State state_);
    State getState();
    void update(float dt);
    //void draw();
    ofXml toXml();
    void fromXml(ofXml& xml);
    void reset();
    void clear();
    void resume();
    void pause();
    
    // a bit hacky
    void setSoundControlsParent(Transform2D* parent);
    
    const std::vector<std::unique_ptr<Rope>>& getRopes() const;

    int getPluckCountElapsed();
    
private:
    
    int getUId();
    // don't reuse id's
    //std::queue<int> availableIds;
    int maxId{10};
    int pluckCount{0};
    
    std::vector<std::unique_ptr<Rope>> ropes;
    std::map<uint32_t, Rope*> ropesById;
    std::map<int, ofVec2f> previousPositions;
    std::map<int, MovingRopeControl*> movingRopeControls;
    std::map<int, CreateRopeControl*> createRopeControls;
    
    StaticPool<CreateRopeControl, MAX_TOUCHES> createRopeControlsPool;
    StaticPool<MovingRopeControl, MAX_TOUCHES> movingRopeControlsPool;
    
    void pointerDownHandler(TouchEvent& touch);
    void pointerMovedHandler(TouchEvent& touch);
    void pointerUpHandler(TouchEvent& touch);
    
    void editPointerDownHandler(TouchEvent &touch);
    void editPointerMovedHandler(TouchEvent &touch);
    void editPointerUpHandler(TouchEvent &touch);
    
    void playPointerDownHandler(TouchEvent &touch);
    void playPointerMovedHandler(TouchEvent &touch);
    void playPointerUpHandler(TouchEvent &touch);
    
    void settingsPointerDownHandler(TouchEvent &touch);
    void settingsPointerMovedHandler(TouchEvent &touch);
    
    void exitSettingsMode();
    void removeDeletedEvents();
    
    void deletePointerHandler(TouchEvent &touch);
    
    UISoundControls soundControls;
    bool soundControlsActivated{false};
    void showSoundControls();
    void hideSoundControls();
    
    RopeAudioSettings audioSettings;
    int selectedCount{0};
    
    bool isTouchingRope(ofVec2f touchScreenPosition, Rope* rope);
    
    State state{NONE};
    std::shared_ptr<ofCamera> camera;
    
    ofVec2f worldToScreen(const ofVec3f& input);
    ofVec3f screenToWorld(const ofVec2f& input, float depth = .95f);
    
    vector<ofVec3f> tri;
    
    class TouchInfo
    {
    public:
        
        static constexpr int NUM_FRAMES = 12;
        
        float distance;
        ofVec3f position;
        
        TouchInfo() { reset(); }
        
        void reset()
        {
            std::fill(std::begin(distances), std::end(distances), .0f);
            std::fill(std::begin(times), std::end(times), .0f);
            distance = .0f;
            velocity = .0f;
            index = -1;
        }
        
        void pushInfo(float distance, float time)
        {
            index = (index + 1) % distances.size();
            distances[index] = distance;
            times[index] = time;
            updateVelocity();
        }
        
        float getVelocity() const noexcept { return velocity; }
        
    private:
        
        int index;
        float velocity;
        std::array<float, NUM_FRAMES> distances;
        std::array<float, NUM_FRAMES> times;
        
        void updateVelocity()
        {
            auto distance = .0f;
            auto time = .0f;
            for (auto i = 0; i < NUM_FRAMES; ++i)
            {
                distance += distances[i];
                time += times[i];
            }
            if (time <= .0f)
            {
                velocity = .0f;
                return;
            }
            const auto speed = distance / time;
            velocity = ofClamp(speed, .0f, 1.0f);
        }
    };
    
    std::array<TouchInfo, MAX_TOUCHES> touchesInfo;
    
    std::shared_ptr<Sequencer> sequencer;
    
    AudioEngine* audioEngine;
};

