//
//  UISettings.cpp
//  Harp2
//
//  Created by etienne cella on 2016-03-05.
//
//

#include "UISettings.hpp"
/*
void UISettings::setup()
{
    gui = std::make_unique<ofxDatGui>(ofxDatGuiAnchor::TOP_RIGHT);
    
    darkColorPicker = gui->addColorPicker("dark tone", ofColor::steelBlue * .18f);
    darkColorPicker->onColorPickerEvent(this, &UISettings::onColorPickerEvent);
    
    mediumColorPicker = gui->addColorPicker("medium tone", ofColor::steelBlue);
    mediumColorPicker->onColorPickerEvent(this, &UISettings::onColorPickerEvent);
    
    lightColorPicker = gui->addColorPicker("light tone", ofColor::lightSteelBlue);
    lightColorPicker->onColorPickerEvent(this, &UISettings::onColorPickerEvent);
    
    ropeHueOffset = gui->addSlider("rope hue offset", .0f, 1.0f, .0f);
    ropeHueOffset->onSliderEvent(this, &UISettings::onSliderEvent);
    
    ropeHueRange = gui->addSlider("rope hue range", .0f, 1.0f, .2f);
    ropeHueRange->onSliderEvent(this, &UISettings::onSliderEvent);
    
    synchronizeSettings();
}

void UISettings::update() { gui->update(); }

void UISettings::draw() { gui->draw(); }

void UISettings::synchronizeSettings()
{
    Settings::setLightColor(lightColorPicker->getColor());
    Settings::setMediumColor(mediumColorPicker->getColor());
    Settings::setDarkColor(darkColorPicker->getColor());
    Settings::setRopeHueOffset(ropeHueOffset->getValue());
    Settings::setRopeHueRange(ropeHueRange->getValue());
}

void UISettings::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
{
    synchronizeSettings();
}

void UISettings::onSliderEvent(ofxDatGuiSliderEvent e)
{
    synchronizeSettings();
}*/