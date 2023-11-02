//
//  UIKeyboard.cpp
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#include "UIKeyboard.hpp"

UIKeyboard::UIKeyboard()
{
    const auto l0 = "qwertyuiop";
    const auto l1 = "asdfghjkl";
    const auto l2 = "zxcvbnm_";
    const auto ls = {l0, l1, l2};
    
    setLayoutType(VERTICAL);
    setSpaceBetweenChilds(12);
    
    for (const string& l : ls)
    {
        auto wrap = std::make_unique<UIBaseObject>();
        wrap->setFitContent(false, true);
        wrap->setFlexibleSize(ofVec2f(1, 0));
        wrap->setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
        wrap->setLayoutType(HORIZONTAL);
        //wrap->setPadding(Padding(0, 24, 0, 0));
        wrap->setSpaceBetweenChilds(12);
        
        for (const char& c : l)
        {
            auto uPtr = std::make_unique<UIKeyboardButton>(KEYBOARD, c, Fonts::getFontH1());
            wrap->addChild(uPtr.get());
            uPtr->setPreferredSize(ofVec2f(148, 148));
            uPtr->setPreserveSizeRatio(true);
            uPtr->setPivot(ofVec2f(.5f, .5f));
            uPtr->onPointerDown.connect<UIKeyboard, &UIKeyboard::buttonPointerDownHandler>(this);
            buttons.push_back(std::move(uPtr));
        }
        
        addChild(wrap.get());
        uiObjects.push_back(std::move(wrap));
    }
}

UIKeyboard::~UIKeyboard()
{
    for (auto& b : buttons)
    {
        b->onPointerDown.disconnect<UIKeyboard, &UIKeyboard::buttonPointerDownHandler>(this);
    }
}

void UIKeyboard::buttonPointerDownHandler(UIButton* btn)
{
    onKeyDown.emit(dynamic_cast<UIKeyboardButton*>(btn)->getChar());
}

void UIKeyboard::show()
{
    auto i = .0f;
    for (auto& b : buttons)
    {
        //b->ipDisplay = .0f;
        //b->ipDisplay.to(1.0f, .4f, BackEaseOut(), 0, i * .01f);
        b->show(i * .01f);
        ++i;
    }
}

void UIKeyboard::hide()
{
    auto i = .0f;
    for (auto it = buttons.rbegin(); it != buttons.rend(); ++it )
    {
        (*it)->hide(i * .01f);
        ++i;
    }
}



