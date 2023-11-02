//
//  UIKeyboard.hpp
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#pragma once

#include "UIBaseObject.hpp"
#include "UIButton.hpp"
#include "Fonts.hpp"

class UIKeyboardButton : public UIButton
{
public:
    
    UIKeyboardButton(UIAction action, char c_, ofTrueTypeFont* font_) :
    UIButton(action, nullptr), c(c_), font(font_) {};
    
    char getChar() const { return c; };
    
    void drawIcon()
    {
        const string t = std::string(1, c);
        const auto bb = font->getStringBoundingBox(t, 0, 0);
        font->drawString(t,
                         getSize().x * .5f - bb.width * .5f,
                         getSize().y * .5f + bb.height * .5f);
    }
    
private:
    
    char c{' '};
    ofTrueTypeFont* font;
};

class UIKeyboard : public UIBaseObject
{
public:
    Nano::Signal<void(char)> onKeyDown;

    UIKeyboard();
    ~UIKeyboard();
    
    void show();
    void hide();
    
private:
    
    void buttonPointerDownHandler(UIButton* btn);
    
    std::vector<std::unique_ptr<UIKeyboardButton>> buttons;
    std::vector<std::unique_ptr<UIBaseObject>> uiObjects; // wrappers, etc...

};