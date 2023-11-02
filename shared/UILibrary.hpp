//
//  UILibrary.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-17.
//
//

#pragma once

#include "UIBaseObject.hpp"
#include "UITextBox.hpp"
#include "UIMenu.hpp"
#include "ImageAssets.hpp"
#include "Locator.h"
#include "Fonts.hpp"
#include "Library.hpp"

class UILibrarySlot : public UIBaseObject
{
public:
    
    Nano::Signal<void(UILibrarySlot* slot, UIAction action)> onEvent;
    
    int index = -1;
    
    UILibrarySlot()
    {
        labelWrap.setLayoutType(Layout::HORIZONTAL);
        labelWrap.setFlexibleSize(ofVec2f(1, 0));
        labelWrap.setPreferredSize(ofVec2f(0, 120));
        labelWrap.setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
        label.setFont(Fonts::getFontP());
        labelWrap.addChild(&label);
        addChild(&labelWrap);
        
        auto imageAssets = Locator::get<ImageAssets>();
        
        constexpr auto menuHeight = 180;
        menu.setLayoutType(HORIZONTAL);
        menu.setAnchor(Layout::HorizontalAlign::LEFT, Layout::VerticalAlign::BOTTOM);
        menu.setAnchoredPosition(ofVec2f(0, -menuHeight));
        menu.setPreferredSize(ofVec2f(0, menuHeight));
        menu.setFlexibleSize(ofVec2f(1, 0));
        menu.setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
        menu.setSpaceBetweenChilds(12);
        openButton = menu.addButton<UIButton>(SCENE_OPEN, imageAssets->getIcon(ImageAssets::Icon::OPEN));
        deleteButton = menu.addButton<UIButton>(SCENE_DELETE, imageAssets->getIcon(ImageAssets::Icon::DUMP));
        menu.changed.connect<UILibrarySlot, &UILibrarySlot::handleUIAction>(this);
        addChild(&menu);
        
        clear();
        ipDisplay = 1.0f;
        ipLabelImageOpacity = 1.0f;
    }
    
    void update(float dt)
    {
        ipDisplay.update(dt);
        auto color = Settings::getLightColor();
        color.a = (float)ipDisplay * (float)ipLabelImageOpacity * 255.0f;
        ofSetColor(color);
        label.color = color;
    }
    
    void draw()
    {
        const auto imageLoaded = thumbnail.isAllocated();
        auto color = imageLoaded ? ofColor::white : Settings::getDarkColor();
        color.a = (float)ipDisplay * (float)ipLabelImageOpacity * 255.0f;
        ofSetColor(color);
        if (imageLoaded)
        {
            const auto imageRatio = (float)thumbnail.getWidth() / (float)thumbnail.getHeight();
            const auto uiRatio = getSize().x / getSize().y;
            
            float sx, sy, sw, sh;
            
            if (uiRatio > imageRatio)
            {
                sx = .0f;
                sw = thumbnail.getWidth();
                sh = sw / uiRatio;
                sy = (thumbnail.getHeight() - sh) * .5f;
            }
            else
            {
                sh = thumbnail.getHeight();
                sw = sh * uiRatio;
                sy = .0f;
                sx = (thumbnail.getWidth() - sw) * .5f;
            }
            
            thumbnail.drawSubsection(0, 0, getSize().x, getSize().y, sx, sy, sw, sh);
        }
        else ofDrawRectangle(0, 0, getSize().x, getSize().y);
    }
    
    void clear()
    {
        empty = true;
        if (thumbnail.isAllocated()) thumbnail.clear();
        label.setText("new_scene");
        deleteButton->setPointerEnabled(false);
    }
    
    void load(string label_, string thumbnailPath_)
    {
        label.setText(label_);
        thumbnail.load(thumbnailPath_);
        deleteButton->setPointerEnabled(true);
        empty = false;
    }
    
    void show(float delay = .0f)
    {
        ipDisplay = .0f;
        ipDisplay.to(1.0f, .5f, Linear(), 0, delay);
        openButton->show(.12f + delay);
        deleteButton->show(.24f + delay);
    }
    
    void hide()
    {
        ipDisplay.to(.0f, .5f, Linear(), 0);
        openButton->hide();
        deleteButton->hide();
    }
    
private:
    
    void handleUIAction(UIButton* button)
    {
        onEvent.emit(this, button->getAction());
    }
    
    InterpolatedProperty ipDisplay;
    InterpolatedProperty ipLabelImageOpacity;
    bool empty{true};
    UIMenu menu;
    UITextBox label;
    UIBaseObject labelWrap;
    UIButton* openButton;
    UIButton* deleteButton;
    ofImage thumbnail;
};

class UILibrary : public UIBaseObject
{
public:
    
    Nano::Signal<void()> onExit;
    
    UILibrary();
    ~UILibrary();
    
    void synchronize();
    void update(float dt);
    void draw();
    void show(const std::function<void()>& callback = 0, bool immediate = false);
    void hide(const std::function<void()>& callback = 0);
    
private:
    
    void handleButton(UIButton* button);
    void handleUIAction(UIAction action);
    void handleSlotAction(UILibrarySlot* slot, UIAction action);
    
    InterpolatedProperty ipDisplay;
   
    UIBaseObject container;
    std::unique_ptr<UIButton> closeButton;
    std::array<UILibrarySlot, Library::NUM_SLOTS> slots;
    std::vector<unique_ptr<UIBaseObject>> uiObjects;
    std::shared_ptr<Library> library;
};
