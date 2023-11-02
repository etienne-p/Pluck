//
//  UIModeMenu.cpp
//  Harp2
//
//  Created by Etienne on 2015-12-01.
//
//

#include "UIMenu.hpp"

UIMenu::~UIMenu()
{
    for (auto& b : buttons)
    {
        b->onPointerDown.disconnect<UIMenu, &UIMenu::buttonPointerDownHandler>(this);
    }
}

void UIMenu::draw()
{
    //ofSetColor(ofColor::darkGrey);
    //ofDrawRectangle(0, 0, getSize().x, getSize().y);
}

void UIMenu::select(UIAction action)
{
    for (auto& b : buttons) b->setSelected(b->getAction() == action);
}

UIButton* UIMenu::getButtonByAction(UIAction action)
{
    return buttonsMap[action];
}

void UIMenu::buttonPointerDownHandler(UIButton* button)
{
    //ofLogNotice("UIModeMenu, clicked [" + id + "]");
    changed.emit(button);
}