//
//  Animation.hpp
//  Harp2
//
//  Created by etienne cella on 2015-12-26.
//
//

#include <stack>
#include <functional>
#include "Easing.h"

typedef std::function<float (EasingArgs)> EaseFn;

class Tween
{
    
public:
    
    Tween(float from, float to, float duration, EaseFn easing_) { setup(from, to, duration, easing_); }
    
    Tween() {  };
    
    void setup(float from, float to, float duration, EaseFn easing_, float delay_ = 0)
    {
        delay = delay_;
        args.t = .0f;
        args.d = duration;
        value = args.b = from;
        args.c = to - from;
        easing = easing_;
        running = false;
        complete = false;
    }
    
    void start()
    {
        args.t = .0f;
        running = true;
        complete = false;
    }
    
    bool isComplete() const noexcept { return complete; }
    
    bool isRunning() const noexcept { return running; }
    
    float getValue() const noexcept { return value; }
    
    void setValue(float value_)
    {
        running = false;
        complete = false;
        value = value_;
    }
    
    void pause() { running = false; }
    
    void resume() { running = true; }
    
    float update(float dt)
    {
        if (!running) return value;
        if (delay > dt)
        {
            delay -= dt;
            return value;
        }
        if (delay > 0)
        {
            dt -= delay;
            delay = 0;
        }
        args.t += dt;
        if (args.t >= args.d && !complete)
        {
            complete = true;
            running = false;
            args.t = args.d;
        }
        return value = easing(args);
    }
    
private:
    
    bool complete{false};
    bool running{false};
    float value{.0f};
    float elapsed{.0f};
    float delay{.0f};
    EasingArgs args;
    EaseFn easing;
};