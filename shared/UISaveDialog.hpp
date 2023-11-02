//
//  UISaveDialog.hpp
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#pragma once

#include "UIBaseObject.hpp"
#include "UITextBox.hpp"
#include "UIMenu.hpp"
#include "ImageAssets.hpp"
#include "Locator.h"
#include "UIKeyboard.hpp"
#include "Fonts.hpp"
#include "Library.hpp"

class UISaveDialog : public UIBaseObject
{
public:
    
    Nano::Signal<void()> onExit;
    
    UISaveDialog();
    ~UISaveDialog();
    
    
    void update(float dt);
    
    void draw();
    void synchronize();
    void show(const std::function<void()>& callback = 0);
    void hide(const std::function<void()>& callback = 0);
    
private:
    void handleButton(UIButton* button);
    void handleUIAction(UIAction action);
    void handleKeyDown(char c);
    void syncUI();

    InterpolatedProperty ipDisplay;
    string name{""};
    UITextBox textBox;
    UIKeyboard keyboard;
    std::unique_ptr<UIButton> closeButton;
    std::unique_ptr<UIButton> saveButton;
    std::unique_ptr<UIButton> resetButton;
    std::unique_ptr<UIBaseObject> wrap;
    std::vector<std::unique_ptr<UIBaseObject>> uiObjects;
    std::shared_ptr<Library> library;
};
