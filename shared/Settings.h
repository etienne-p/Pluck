//
//  Settings.h
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#pragma once

#include "ofColor.h"

class Settings
{
public:
    static constexpr int THUMBNAIL_SIZE = 256;
    static constexpr int BUFFER_SIZE = 512;
    static constexpr float REF_UI_SCREEN_WIDTH = 2048.0f;
    static constexpr float REF_UI_SCREEN_HEIGHT = 1536.0f;
    
    static ofColor getLightColor() { return ofColor::lightSteelBlue; }
    static ofColor getMediumColor() { return ofColor::steelBlue; }
    static ofColor getDarkColor() { return ofColor::steelBlue * .24f ; }
    static float getRopeHueOffset() { return .12f; }
    static float getRopeHueRange() { return .5f; }
    
    /*
    
    // getters
    static ofColor getLightColor() { return lightColor; }
    static ofColor getMediumColor() { return mediumColor; }
    static ofColor getDarkColor() { return darkColor; }
    static float getRopeHueOffset() { return ropeHueOffset; }
    static float getRopeHueRange() { return ropeHueRange; }
    
    // setters
    static void setLightColor(ofColor v) { lightColor = v; }
    static void setMediumColor(ofColor v) { mediumColor = v; }
    static void setDarkColor(ofColor v) { darkColor = v; }
    static void setRopeHueOffset(float v) { ropeHueOffset = v; }
    static void setRopeHueRange(float v) { ropeHueRange = v; }
    
private:
    
    static ofColor lightColor;
    static ofColor mediumColor;
    static ofColor darkColor;
    static float ropeHueOffset;
    static float ropeHueRange;
    */
};

