//
//  UIButton.hpp
//  Harp2
//
//  Created by etienne cella on 2015-11-19.
//
//

#pragma once

#include <memory>
#include "UIBaseObject.hpp"
#include "nano_signal_slot.hpp"
#include "InterpolatedProperty.h"
#include "UIAction.h"
#include "Graphics.hpp"
#include "Settings.h"
#include "UIAntialiasedGraphics.hpp"

class UIButton : public UIBaseObject
{
    
public:
    
    //ofColor color;
    
    Nano::Signal<void(UIButton*)> onPointerDown;
    
    UIButton(UIAction action_, std::shared_ptr<ofImage> icon_);
    UIAction getAction() const { return action; };
    InterpolatedProperty ipDisplay;

    void pointerDownHandler(TouchEvent& touch);
    void draw();
    void update(float dt);
    bool setPointerEnabled(bool v, bool recursize = false);
    void setSelected(bool value);
    void show(float delay = 0);
    void hide(float delay = 0);
    
protected:
    virtual void drawIcon();
    bool selected{false};
    std::shared_ptr<ofImage> icon;
    UIAction action;
    InterpolatedProperty ipTouch;
    InterpolatedProperty ipEnabled;
    InterpolatedProperty ipSelected;
};

class UIMultiStateButton : public UIButton
{
    
public:
    UIMultiStateButton(UIAction action_, const vector<std::shared_ptr<ofImage>>& icons_);
    bool setState(unsigned int state_);
    unsigned int getState() const;
protected:
    unsigned int state{0};
    vector<std::shared_ptr<ofImage>> icons;
};

class UIRecordButton : public UIButton
{
public:
    UIRecordButton(UIAction action_) :
    UIButton(action_, nullptr){};
    void update(float dt);
private:
    void drawIcon();
    float oscillation{.0f};
};
