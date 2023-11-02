//
//  InterpolatedProperty.h
//  Harp2
//
//  Created by etienne cella on 2015-12-07.
//
//

#pragma once

#include <map>
#include <functional>
#include "ofMath.h"
#include "Tween.hpp"

class InterpolatedProperty
{
public:
    
    InterpolatedProperty* to(float value_, float duration_, EaseFn ease_, std::function<void(void)> callback_ = 0, float delay_ = .0f)
    {
        if (duration_ == .0f && delay_ == .0f)
        {
            if (callback_) callback_();
            animating = false;
            callback = 0;
            value = value_;
            return this;
        }
        callback = callback_;
        tween.setup(value, value_, duration_, ease_, delay_);
        tween.start();
        animating = true;
        return this;
    }
    
    float update(float dt)
    {
        if (animating)
        {
            value = tween.update(dt);
            if (tween.isComplete())
            {
                animating = false;
                if (callback) callback();
                callback = 0;
            }
        }
        return value;
    }
    
    float operator=(float value_)
    {
        animating = false;
        return value = value_;
    }
    
    operator float() const noexcept { return value; }
    
    float getValue() const noexcept { return value; }
    
    bool isRunning () const noexcept { return tween.isRunning(); }
    
    bool isComplete () const noexcept { return tween.isComplete(); }
    
private:
    
    std::function<void(void)> callback;
    float value{.0f};
    bool animating{false};
    Tween tween;
};
