//
//  UISlider.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-20.
//
//

#include "UISlider.hpp"

UISlider::UISlider()
{
    ipDisplay = 1.0f;
}

float UISlider::getNormalizedValue() const noexcept { return normalizedValue; }

void UISlider::setNormalizedValue(float value)
{
    normalizedValue = ofClamp(value, .0f, 1.0f);
}

void UISlider::pointerMovedHandler(TouchEvent& touch)
{
    touch.canceled = true;
    setNormalizedValue((touch.localPosition.x - getSize().y * .5f) / (getSize().x - getSize().y));
    onValueChange.emit(this);
}

void UISlider::pointerDownHandler(TouchEvent& touch)
{
    pointerMovedHandler(touch);
}

void UISlider::draw()
{
    const auto animFactor = float{ipDisplay};
    auto btnPos = ofVec2f(getSize().y * .5f + (getSize().x - getSize().y) * normalizedValue,
                          getSize().y * .5f);
    auto c = Settings::getLightColor();
    c.a = 60;
    
    ofSetColor(c);
    ofDrawLine(getSize().y * .5f, getSize().y * .5f,
               (getSize().x - (getSize().y * .5f)) * animFactor,
               getSize().y * .5f);
    
    ofSetColor(Settings::getMediumColor());
    ofDrawLine(getSize().y * .5f, getSize().y * .5f,
               btnPos.x, btnPos.y);
    
    ofSetColor(Settings::getLightColor());
    
    UIAntialiasedGraphics::Instance()->drawCircleOutline(btnPos,
                                                         getSize().y * .5f * animFactor,
                                                         getSize().y * .04f * animFactor);
    //UIAntialiasedGraphics::Instance()->drawCircleFill(btnPos, getSize().y * .1f);
    ofDrawCircle(btnPos, getSize().y * .1f * animFactor);
}