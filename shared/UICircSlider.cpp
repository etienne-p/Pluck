//
//  UICircSlider.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-20.
//
//

#include "UICircSlider.hpp"

void UICircSlider::pointerMovedHandler(TouchEvent& touch)
{
    const auto d = touch.localPosition - ofVec2f(radiusMax, radiusMax);
    
    if (d.length() >= radiusMin && d.length() <= radiusMax)
    {
        const auto angle = atan2(d.y, d.x) ;//- PI * .5f;

        if (angleStart <= angle && angle <= angleStart + angleArc)
        {
            normalizedValue = ofClamp((angle - angleStart) / angleArc, .0f, 1.0f);
        }
    }
}

void UICircSlider::pointerDownHandler(TouchEvent& touch)
{
    pointerMovedHandler(touch);
}

void UICircSlider::computeHitArea()
{
    setSize(ofVec2f(radiusMax * 2.0f, radiusMax * 2.0f));
}

void UICircSlider::draw()
{
    // prevent tesselation errors
    if (angleArc <= .0f || radiusMin == radiusMax) return;
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofColor c = Settings::getLightColor();
    c.a = 40;
    ofSetColor(c);
    Graphics::drawAnnularWedge(getPosition() + ofVec2f(radiusMax, radiusMax),
                               radiusMin, radiusMax, angleStart, angleArc);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    if (normalizedValue == .0f) return; 
    ofSetColor(Settings::getLightColor());
    Graphics::drawAnnularWedge(getPosition() + ofVec2f(radiusMax, radiusMax),
                               radiusMin, radiusMax, angleStart, angleArc * normalizedValue);
}