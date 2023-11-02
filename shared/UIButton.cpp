//
//  UIButton.cpp
//  Harp2
//
//  Created by etienne cella on 2015-11-19.
//
//

#include "UIButton.hpp"

UIButton::UIButton(UIAction action_, std::shared_ptr<ofImage> icon_)
{
    //color = Settings::getLightColor();
    
    action = action_;
    icon = icon_;
    
    ipDisplay = 1.0f;
    ipEnabled = 1.0f;
    ipTouch = .0f;
    ipSelected = .0f;
}

void UIButton::setSelected(bool value)
{
    selected = value;
    if (value) ipSelected.to(1.0f, .2f, Linear());
    else ipSelected.to(.0f, .2f, Linear());
}

bool UIButton::setPointerEnabled(bool value, bool recursize)
{
    if (value) ipEnabled.to(1.0f, .2f, Linear());
    else ipEnabled.to(.0f, .2f, Linear());
    PointerHandler::setPointerEnabled(value, recursize);
}

void UIButton::pointerDownHandler(TouchEvent& touch)
{
    //ofLog(OF_LOG_NOTICE, "down id: " + id + " down");
    touch.canceled = true;
    onPointerDown.emit(this);
    ipTouch = 1.0f;
    ipTouch.to(.0f, .5f, CircEaseOut());
}

void UIButton::update(float dt)
{
    ipEnabled.update(dt);
    ipDisplay.update(dt);
    ipTouch.update(dt);
    ipSelected.update(dt);
    float s = (1.0f + ipTouch * .12f) * ipDisplay;
    setScale(ofVec2f(s, s));
}

void UIButton::drawIcon()
{
    const auto s = .42f;
    icon->draw(getSize().x * (.5f - s * .5f), getSize().y * .5f - getSize().x * s * .5f,
               getSize().x * s, getSize().x * s);
}

void UIButton::draw()
{
    auto c = Settings::getLightColor();
    c.a = 255.0f * (.2f + .8f * ipEnabled);
    ofSetColor(c);//Settings::COLOR_LIGHT);
    UIAntialiasedGraphics::Instance()->drawCircleOutline(getSize() * .5f,
                                                         getSize().x * .5f,
                                                         getSize().x *
                                                         (.0f + .06f * std::min(1.0f, ipSelected + ipTouch)));
    drawIcon();
    /*
    return;
    Graphics::drawAnnularWedge(getSize() * .5f,
                               getSize().x * (.5f - .05f * ipSelected - .025f),
                               getSize().x * .5f, 0, 2.0f * PI);
     */
}

void UIButton::show(float delay)
{
    ipDisplay = .0f;
    ipDisplay.to(1.0f, .4f, BackEaseOut(), 0, delay);
}

void UIButton::hide(float delay)
{
    ipDisplay.to(.0f, .4f, CircEaseIn(), 0, delay);
}

UIMultiStateButton::UIMultiStateButton(UIAction action_, const vector<std::shared_ptr<ofImage>>& icons_) :
    UIButton(action_, icons_[0])
{
    icons = icons_;
    setState(0);
}

unsigned int UIMultiStateButton::getState() const { return state; }

bool UIMultiStateButton::setState(unsigned int state_)
{
    if (state_ < icons.size())
    {
        state = state_;
        icon = icons[state];
        return true;
    }
    return false;
}

void UIRecordButton::update(float dt)
{
    UIButton::update(dt);
    oscillation = fmod(oscillation + dt * 4.0f, 2.0f * PI);
}

void UIRecordButton::drawIcon()
{
    float r = getSize().x * (.24f + .024f * cos(oscillation) * ipSelected);
    //ofDrawCircle(getSize() * .5f, r);
    UIAntialiasedGraphics::Instance()->drawCircleFill(getSize() * .5f, r);
}
