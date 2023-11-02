//
//  UISlider.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-20.
//
//

#pragma once

#include "ofMath.h"
#include "nano_signal_slot.hpp"
#include "UIBaseObject.hpp"
#include "UIAntialiasedGraphics.hpp"
#include "Settings.h"
#include "InterpolatedProperty.h"

class UISlider : public UIBaseObject
{
public:
    
    Nano::Signal<void(UISlider*)> onValueChange;
    
    InterpolatedProperty ipDisplay;
    
    UISlider();
    void pointerMovedHandler(TouchEvent& touch);
    void pointerDownHandler(TouchEvent& touch);
    float getNormalizedValue() const noexcept;
    void setNormalizedValue(float value);
    void draw();
    
protected:
    
    float normalizedValue{.5f};
};