//
//  UITextBox.hpp
//  Harp2
//
//  Created by etienne cella on 2015-12-23.
//
//

#pragma once

#include <stack>
#include <algorithm>
#include "ofMain.h"
#include "UIBaseObject.hpp"
#include "InterpolatedProperty.h"
#include "Settings.h"

class UITextBox : public UIBaseObject
{

public:
    
    InterpolatedProperty ipDisplay;
    ofColor color;
    
    UITextBox();
    void setFont(ofTrueTypeFont* font_);
    void setText(std::string text_);
    std::string getText() const;
    void setSize(const ofVec2f& v);
    void fitSize();
    void update(float dt);
    void draw();
    void show(float delay = .0f);
    void hide(float delay = .0f);
    
private:
    
    //bool dirty{false};
    bool canBreak(char c);
    ofTrueTypeFont* font{nullptr};
    std::string rawText{""};
    std::string text{""};
};
