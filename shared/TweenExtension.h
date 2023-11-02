//
//  TweenSequence.h
//  Harp2
//
//  Created by etienne cella on 2015-12-09.
//
//

/*
#pragma once

#include <memory>
#include <vector>
#include "ofxTween.h"

class ExtendedTween : public ofxTween
{
    void setOnComplete(std::function<void()> cb)
    {
        onComplete = cb;
    }
    
    void update()
    {
        
    }
    
private:
    bool completed{false};
    std::function<void()> onComplete{0};
};

class TweenSequence
{
    template <typename TEasing>
    size_t addTween(ofxTween::ofxEasingType type, float from, float to, float duration, float delay = .0f)
    {
        static_assert(std::is_base_of<ofxEasing, TEasing>::value, "TEasing must be a subclass of ofxEasing");
        
        TweenItem item;
        item.easing = std::make_unique<TEasing>();
        item.tween = std::make_unique<ofxTween>();
        item.tween->setParameters(*(item.easing), type, from, to, duration, delay);
        tweens.push_back(item);
        return tweens.size();
    }
    
    void getValue(size_t index) const
    {
        assert(index < tweens.size());
        return tweens[index].value;
    }
    
    void start()
    {
        completed = false;
        for (auto& i : tweens) i.tween->start();
    }

    void update()
    {
        auto lCompleted = completed;
        for (auto& i : tweens)
        {
            i.value = i.tween->update();
            lCompleted &= i.tween->isCompleted();
        }
        // a transion from completed to uncompleted is very unlikely, but handled
        if (lCompleted && completed != lCompleted && onComplete != 0) onComplete();
        completed = lCompleted;
    }
    
    void setOnComplete(std::function<void()> cb)
    {
        onComplete = cb;
    }
    
    bool isComplete() const { return completed; }
    
private:
    
    struct TweenItem
    {
        float value{.0f};
        std::unique_ptr<ofxTween> tween;
        std::unique_ptr<ofxEasing> easing;
    };
    
    struct CallbackItem
    {
        double time;
        std::function<void()> callback;
    };
    
    std::function<void()> onComplete{0};
    
    bool completed{false};
    
    vector<TweenItem> tweens;
    vector<CallbackItem> callbacks;
};
*/