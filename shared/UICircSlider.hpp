//
//  UICircSlider.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-20.
//
//

#pragma once

#include "ofMath.h"
#include "Graphics.hpp"
#include "UIBaseObject.hpp"
#include "Settings.h"

class UICircSlider : public UIBaseObject
{
public:
    
    float angleStart{.0f};
    float angleArc{1.0f};
    float radiusMin{10.0f};
    float radiusMax{14.0f};
        
    void pointerDownHandler(TouchEvent& touch);
    void pointerMovedHandler(TouchEvent& touch);
    //void pointerUpHandler(TouchEvent touch);
    
    void computeHitArea();
    
    void draw();
    
    float normalizedValue{.0f};

    
protected:
    
};