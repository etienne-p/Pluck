//
//  Easing.h
//  Harp2
//
//  Created by etienne cella on 2015-12-26.
//
//

#pragma once

#include <math.h>

struct EasingArgs
{
    float t; //time
    float b; //beginning
    float c; //change
    float d; //duration
    
    EasingArgs(float t_, float b_, float c_, float d_)
    : t(t_), b(b_), c(c_), d(d_){}
    
    EasingArgs() : t(0), b(0), c(0), d(0){};
};

struct Linear
{
    float operator() (EasingArgs args) const
    {
        return args.c * args.t / args.d + args.b;
    }
};

struct CircEaseIn
{
    float operator() (EasingArgs args) const
    {
        return - args.c * (sqrt(1 - (args.t /= args.d) * args.t) - 1) + args.b;
    }
};

struct CircEaseOut
{
    float operator() (EasingArgs args) const
    {
        return args.c * sqrt(1 - (args.t = args.t / args.d-1) * args.t) + args.b;
    }
};

struct CircEaseInOut
{
    float operator() (EasingArgs args) const
    {
        if ((args.t /= args.d / 2) < 1) return -args.c / 2 * (sqrt(1 - args.t * args.t) - 1) + args.b;
        return args.c / 2 * (sqrt(1 - args.t * (args.t -= 2)) + 1) + args.b;
    }
};

struct BackEaseIn
{
    float operator() (EasingArgs args) const
    {
        constexpr float s = 1.70158f;
        const float postFix = args.t /= args.d;
        return args.c * (postFix) * args.t * ((s + 1) * args.t - s) + args.b;
    }
};

struct BackEaseOut
{
    float operator() (EasingArgs args) const
    {
        constexpr float s = 1.70158f;
        return args.c * ((args.t = args.t / args.d - 1) * args.t * ((s + 1) * args.t + s) + 1) + args.b;
    }
};

struct ExpoEaseIn
{
    float operator() (EasingArgs args) const
    {
        return (args.t == 0) ? args.b : args.c * pow(2, 10 * (args.t / args.d - 1)) + args.b;
    }
};

struct ExpoEaseOut
{
    float operator() (EasingArgs args) const
    {
        return (args.t == args.d) ? args.b + args.c :
        args.c * (-pow(2, -10 * args.t / args.d) + 1) + args.b;
    }
};

struct QuartEaseIn
{
    float operator()(EasingArgs args) const
    {
        return args.c * (args.t /= args.d) * args.t * args.t * args.t + args.b;
    }
};

struct QuartEaseOut
{
    float operator()(EasingArgs args) const
    {
        return -args.c * ((args.t = args.t / args.d - 1) * args.t * args.t * args.t - 1) + args.b;
    }
};


/*


float Back::easeInOut(float t,float b , float c, float d) {
    float s = 1.70158f;
    if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
    float postFix = t-=2;
    return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
}*/