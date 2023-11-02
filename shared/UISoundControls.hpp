//
//  UISoundControls.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-24.
//
//

#pragma once

#include <array>
#include <functional>
#include "UISlider.hpp"
#include "ofMath.h"
#include "Rope.hpp"
#include "UIBaseObject.hpp"
#include "InterpolatedProperty.h"
#include "UITextBox.hpp"
#include "Fonts.hpp"

// TODO: use InterpolatedProperty
class UISoundControls : public UIBaseObject
{
    
public:
    
    static constexpr size_t NUM_SLIDERS = 2;
    
    float radius{400.0f};
    float angleStart{PI * .5f};
    float angleArc{PI * 1.5f};
    
    InterpolatedProperty ipDisplay;
    
    UISoundControls();
    ~UISoundControls();
    
    void update(float dt);
    //void pointerDownHandler(TouchEvent& touch);
    
    void draw();
    
    void writeSettings(RopeAudioSettings& settings);
    void readSettings(RopeAudioSettings& settings);
    
    void show(const std::function<void()>& callback = 0);
    void hide(const std::function<void()>& callback = 0);
    
private:
    
    void sliderValueChangedHandler(UISlider* slider);
    bool dirty{false};
    std::array<UISlider, NUM_SLIDERS> sliders;
};