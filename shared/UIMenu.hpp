//
//  UIModeMenu.hpp
//  Harp2
//
//  Created by Etienne on 2015-12-01.
//
//

#pragma once

#include <vector>
#include "UIBaseObject.hpp"
#include "UIButton.hpp"
#include "nano_signal_slot.hpp"
#include "UIAction.h"

class UIMenu : public UIBaseObject
{

public:
    
    Nano::Signal<void(UIButton* button)> changed;

    ~UIMenu();
    
    template <typename T, typename... TArgs>
    UIButton* addButton(TArgs&&... mArgs)
    {
        auto uPtr = std::make_unique<T>(std::forward<TArgs>(mArgs)...);
        addChild(uPtr.get());
        uPtr->setPreferredSize(ofVec2f(140, 140));
        uPtr->setPreserveSizeRatio(true);
        uPtr->setPivot(ofVec2f(.5f, .5f));
        uPtr->onPointerDown.template connect<UIMenu, &UIMenu::buttonPointerDownHandler>(this);
        buttons.push_back(std::move(uPtr));
        buttonsMap[buttons.back()->getAction()] = buttons.back().get();
        return getButtonByAction(buttons.back()->getAction());
    }
    
    void draw();
    void select(UIAction action);
    UIButton* getButtonByAction(UIAction action);
    
private:
    
    void buttonPointerDownHandler(UIButton* button);
    std::map<UIAction, UIButton*> buttonsMap;
    std::vector<unique_ptr<UIButton>> buttons;
};