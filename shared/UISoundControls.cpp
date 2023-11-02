//
//  UISoundControls.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-24.
//
//

#include "UISoundControls.hpp"

UISoundControls::UISoundControls()
{
    setSpaceBetweenChilds(24.0f);
    setLayoutType(LayoutType::VERTICAL);
    
    for(auto& s : sliders)
    {
        s.onValueChange.connect<UISoundControls, &UISoundControls::sliderValueChangedHandler>(this);
        s.setFlexibleSize(ofVec2f(1, 1));
        addChild(&s);
    }
    ipDisplay = 1.0f;
}

UISoundControls::~UISoundControls()
{
    for(auto& s : sliders)
    {
        s.onValueChange.disconnect<UISoundControls, &UISoundControls::sliderValueChangedHandler>(this);
    }
}

void UISoundControls::update(float dt)
{
    ipDisplay.update(dt);
    for(auto& s : sliders)
    {
        s.ipDisplay = (float)ipDisplay;
    }
}

void UISoundControls::draw()
{
    auto color = Settings::getLightColor();
    color.a = 255.0f * ipDisplay;
    ofSetColor(color);
    auto font = Fonts::getFontP(); // might end up as a class field
    // draw slider labels
    // not super efficient to recreate the vector each time but come on, we only use one of these
    const auto propNames = vector<string>{"Brilliance", "Sustain"};
    auto i = 0;
    for(auto& s : sliders)
    {
        const auto bbox = font->getStringBoundingBox(propNames[i], 0, 0);
        const auto sliderPos = s.getPosition() +
                            ofVec2f(s.getSize().x,
                                    s.getSize().y * .5f + bbox.getHeight() * .5f);
        font->drawString(propNames[i], sliderPos.x + 12.0f, sliderPos.y);
        ++i;
    }
}

void UISoundControls::writeSettings(RopeAudioSettings& settings)
{
    settings.setBrilliance(sliders[0].getNormalizedValue());
    settings.setSustain(sliders[1].getNormalizedValue());
}

void UISoundControls::readSettings(RopeAudioSettings& settings)
{
    sliders[0].setNormalizedValue(settings.getBrilliance());
    sliders[1].setNormalizedValue(settings.getSustain());
}

void UISoundControls::show(const std::function<void()>& callback)
{
    dirty = true; // force update
    ipDisplay = .0f;
    ipDisplay.to(1.0f, .4f, CircEaseOut(), callback);
}

void UISoundControls::hide(const std::function<void()>& callback)
{
    ipDisplay.to(.0f, .2f, Linear(), callback);
}

void UISoundControls::sliderValueChangedHandler(UISlider* slider)
{
    dirty = true;
}