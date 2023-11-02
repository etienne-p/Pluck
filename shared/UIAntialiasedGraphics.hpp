//
//  UIAntialiasedGraphics.hpp
//  Harp2
//
//  Created by etienne cella on 2016-01-12.
//
//

#pragma once

#include "ofMain.h"
#include "Utils.hpp"

class UIAntialiasedGraphics
{
public:
    
    UIAntialiasedGraphics()
    {
        float w = 1.0f;
        float h = 1.0f;
        
        quad.addVertex(ofVec2f(-w * .5f, -h * .5f));
        quad.addTexCoord(ofVec2f(-1, -1));
        quad.addVertex(ofVec2f(w * .5f, -h * .5f));
        quad.addTexCoord(ofVec2f(1, -1));
        quad.addVertex(ofVec2f(w * .5f, h * .5f));
        quad.addTexCoord(ofVec2f(1, 1));
        quad.addVertex(ofVec2f(-w * .5f, h * .5f));
        quad.addTexCoord(ofVec2f(-1, 1));
        quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
        circleOutlineShader.load(Utils::Platform::getShaderPath("aa_circle.vert"),
                                 Utils::Platform::getShaderPath("aa_circle_outline.frag"));
        
        circleFillShader.load(Utils::Platform::getShaderPath("aa_circle.vert"),
                              Utils::Platform::getShaderPath("aa_circle_fill.frag"));
    }
    
    void drawCircleFill(ofVec2f center, float radius)
    {
        circleFillShader.begin();
        ofPushMatrix();
        ofTranslate(center);
        ofScale(radius * 2.0f, radius * 2.0f);
        quad.draw();
        ofPopMatrix();
        //circleShader.setUniform(); // TODO: set lineWidth;
        circleFillShader.end();
    }
    
    void drawCircleOutline(ofVec2f center, float radius, float lineWidth)
    {
        circleOutlineShader.begin();
        circleOutlineShader.setUniform1f("lineWidth", lineWidth / radius);
        ofPushMatrix();
        ofTranslate(center);
        ofScale(radius * 2.0f, radius * 2.0f);
        quad.draw();
        ofPopMatrix();
        //circleShader.setUniform(); // TODO: set lineWidth;
        circleOutlineShader.end();
    }
    
    static UIAntialiasedGraphics* Instance()
    {
        if (_instance == 0) {
            _instance = new UIAntialiasedGraphics;
        }
        return _instance;
    }
    
private:
    
    ofShader circleOutlineShader;
    ofShader circleFillShader;
    ofVboMesh quad;
    
    static UIAntialiasedGraphics* _instance;
};
