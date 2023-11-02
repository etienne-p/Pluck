//
//  Sequencer.cpp
//  Harp2
//
//  Created by etienne cella on 2015-11-28.
//
//

#pragma once

#include <vector>
#include "ofMain.h"
#include "nano_signal_slot.hpp"
#include "Poco/Timestamp.h"
#include "UIAction.h"
#include "Rope.hpp"

struct SequencerEvent
{
    //float t{1.0f};
    float velocity{1.0f};
    float timeRelativeToBpm{.0f}; // time relative to bars
    int id;
    
    bool operator< (const SequencerEvent &other) const
    {
        return timeRelativeToBpm < other.timeRelativeToBpm;
    }
};

struct SequencerPattern
{
    bool active{true};
    float timeMul{1.0f};
    float timeStampRelativeToBpm{.0f};
    std::vector<SequencerEvent> events;
};

class Sequencer
{
public:
    
    static constexpr int NUM_PATTERNS{4};
    static const float MIN_TIME_MUL;
    static const float MAX_TIME_MUL;
    
    Nano::Signal<void()> changed;
    Nano::Signal<void()> tick;
    Nano::Signal<void()> notesChanged;
    
    Sequencer();
    ofXml toXml(); // save BPM & SequencerPatterns
    void load(ofXml& data);
    bool pushSequencerEvent(int id, float velocity);
    void play();
    void pause();
    void setBPM(float bpm_);
    float getBPM() const;
    void update(float dt);
    void setRecording(bool value);
    bool isRecording() const;
    bool isPlaying() const;
    int getSelectedPattern() const;
    const std::array<SequencerPattern, NUM_PATTERNS>& getPatterns() const;
    const std::vector<SequencerEvent>& getFrameEvents() const;
    //double getTimeRelativeToBpm(float timeMul) const; // in seconds
    void handleUIAction(UIAction action);
    void selectPattern(size_t index);
    void setTickerEnabled(bool val);
    bool isTickerEnabled() const noexcept;
    void filterEvents(const vector<int>& ids);
    void clear();
private:
    
    double timeStampRelativeToBpm{.0f};
    bool tickerEnabled{false};
    int selectedPattern{-1};
    std::vector<SequencerEvent> frameEvents;
    Poco::Timestamp startTime;
    float bpm{90.0f};
    bool paused{true};
    bool recording{false};
    //double timeRelativeToBpm{.0};
    std::array<SequencerPattern, NUM_PATTERNS> patterns;
};