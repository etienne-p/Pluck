//
//  UILibrary.cpp
//  Harp2
//
//  Created by etienne cella on 2016-02-17.
//
//

#include "UILibrary.hpp"

UILibrary::UILibrary()
{
    library = Locator::get<Library>();
    
    auto imageAssets = Locator::get<ImageAssets>();
    auto slotIndex = 0;
    
    addChild(&container);
    container.setLayoutType(VERTICAL);
    container.setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    container.setFlexibleSize(ofVec2f(1, 1));
    container.setSpaceBetweenChilds(24);
    
    for(auto i = 0; i < 2; ++i)
    {
        auto slotsContainer = std::make_unique<UIBaseObject>();
        addChild(slotsContainer.get());
        slotsContainer->setLayoutType(HORIZONTAL);
        slotsContainer->setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
        slotsContainer->setFlexibleSize(ofVec2f(1, 1));
        slotsContainer->setSpaceBetweenChilds(24);
        container.addChild(slotsContainer.get());
        
        for(auto j = 0; j < 3; ++j)
        {
            //slots[slotIndex].load(i, "mylabel", "dot.png"); //tmp
            slots[slotIndex].index = slotIndex;
            slots[slotIndex].setFlexibleSize(ofVec2f(1, 1));
            slots[slotIndex].onEvent.connect<UILibrary, &UILibrary::handleSlotAction>(this);
            slotsContainer->addChild(&slots[slotIndex]);
            ++slotIndex;
        }
        
        uiObjects.push_back(std::move(slotsContainer));
    }
    
    closeButton = std::make_unique<UIButton>(EXIT, imageAssets->getIcon(ImageAssets::Icon::CLOSE));
    closeButton->setPreferredSize(ofVec2f(140, 140));
    closeButton->setPreserveSizeRatio(true);
    closeButton->setPivot(ofVec2f(.5f, .5f));
    closeButton->setAnchor(Layout::HorizontalAlign::RIGHT, Layout::VerticalAlign::TOP);
    closeButton->setAnchoredPosition(ofVec2f(-64-140, 48));
    closeButton->onPointerDown.connect<UILibrary, &UILibrary::handleButton>(this);
    addChild(closeButton.get());
    
    ipDisplay = 1.0f;
}

UILibrary::~UILibrary()
{
    for (auto& s : slots) s.onEvent.disconnect<UILibrary, &UILibrary::handleSlotAction>(this);
}

void UILibrary::synchronize()
{
    auto sceneIndex = 0;
    
    for (auto& name : library->getSceneNames())
    {
        if (name == "")  slots[sceneIndex].clear();
        else slots[sceneIndex].load(name, library->getThumbnailPath(name));
        ++sceneIndex;
    }
}

void UILibrary::update(float dt)
{
    ipDisplay.update(dt);
    //for (auto& s : slots) s.ipDisplay = (float)ipDisplay;
    //wrap->setScale(ofVec2f(1, 1) * (.0f + 1.0f * ipDisplay));
}

void UILibrary::draw()
{
    auto c = ofColor::black;
    c.a = (.8f * ipDisplay) * 255;
    ofSetColor(c);
    ofDrawRectangle(ofVec2f::zero(), getSize().x, getSize().y);
}

void UILibrary::handleUIAction(UIAction action)
{
    switch(action)
    {
        case EXIT:
            onExit.emit();
            break;

            
        default:
            ofLogError("library", "unexpected ui action");
    }
}

void UILibrary::handleButton(UIButton* button)
{
    handleUIAction(button->getAction());
}

void UILibrary::handleSlotAction(UILibrarySlot* slot, UIAction action)
{
    switch(action)
    {
        case SCENE_DELETE:
            library->eraseScene(slot->index);
            slot->clear();
            break;
            
        case SCENE_OPEN:
            library->openScene(slot->index);
            onExit.emit();
            break;
            
        default:
            ofLogError("library", "unexpected slot action");
    }
}

void UILibrary::show(const std::function<void()>& callback, bool immediate)
{
    if (immediate)
    {
        ipDisplay = 1.0f;
        if (callback != 0) callback();
    }
    else
    {
        ipDisplay = .0f;
        ipDisplay.to(1.0f, .6f, CircEaseOut(), callback);
    }
    
    const auto sceneSelected = library->getCurrentSceneIndex() != -1;
    container.setPadding(Padding(sceneSelected ? 240 : 48, 48, 48, 48));
    closeButton->visible = sceneSelected;
    closeButton->setPointerEnabled(sceneSelected);
    if (sceneSelected) closeButton->show();
    
    auto i = .0f;
    for (auto& s : slots)
    {
        s.show(i * .1f);
        i += 1.0f;
    }
}

void UILibrary::hide(const std::function<void()>& callback)
{
    ipDisplay.to(.0f, .6f, Linear(), callback);
    closeButton->hide();
    for (auto& s : slots) s.hide();
}