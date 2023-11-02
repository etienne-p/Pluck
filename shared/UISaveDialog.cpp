//
//  UISaveDialog.cpp
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#include "UISaveDialog.hpp"

UISaveDialog::UISaveDialog()
{
    library = Locator::get<Library>();
    
    auto imageAssets = Locator::get<ImageAssets>();
    
    closeButton = std::make_unique<UIButton>(EXIT, imageAssets->getIcon(ImageAssets::Icon::CLOSE));
    closeButton->setPreferredSize(ofVec2f(140, 140));
    closeButton->setPreserveSizeRatio(true);
    closeButton->setPivot(ofVec2f(.5f, .5f));
    closeButton->setAnchor(Layout::HorizontalAlign::RIGHT, Layout::VerticalAlign::TOP);
    closeButton->setAnchoredPosition(ofVec2f(-64-140, 48));
    closeButton->onPointerDown.connect<UISaveDialog, &UISaveDialog::handleButton>(this);
    addChild(closeButton.get());
    
    wrap = std::make_unique<UIBaseObject>();
    wrap->setFlexibleSize(ofVec2f(1, 1));
    wrap->setLayoutType(VERTICAL);
    wrap->setSpaceBetweenChilds(32);
    wrap->setPivot(ofVec2f(.5f, .5f));
    wrap->setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    addChild(wrap.get());
    
    auto subWrap = std::make_unique<UIBaseObject>();
    subWrap->setFlexibleSize(ofVec2f(1, 0));
    //subWrap->setPreferredSize(ofVec2f(648, 240));
    subWrap->setFitContent(false, true);
    subWrap->setLayoutType(HORIZONTAL);
    subWrap->setSpaceBetweenChilds(32);
    subWrap->setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    addChild(subWrap.get());
    wrap->addChild(subWrap.get());
    
    textBox.setFlexibleSize(ofVec2f(1, 0));
    textBox.setPreferredSize(ofVec2f(0, 140));
    textBox.setFont(Fonts::getFontH1());
    textBox.setAnchor(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    textBox.setText("Hello");
    subWrap->addChild(&textBox);
    
    resetButton = std::make_unique<UIButton>(DELETE, imageAssets->getIcon(ImageAssets::Icon::CLOSE));
    resetButton->setPreferredSize(ofVec2f(62, 62));
    resetButton->setPreserveSizeRatio(true);
    resetButton->setPivot(ofVec2f(.5f, .5f));
    resetButton->setAnchor(Layout::HorizontalAlign::RIGHT, Layout::VerticalAlign::MIDDLE);
    //saveButton->setAnchoredPosition(ofVec2f(-160, 0));
    resetButton->onPointerDown.connect<UISaveDialog, &UISaveDialog::handleButton>(this);
    subWrap->addChild(resetButton.get());
    
    keyboard.setFlexibleSize(ofVec2f(1, 0));
    keyboard.setFitContent(false, true);
    keyboard.onKeyDown.connect<UISaveDialog, &UISaveDialog::handleKeyDown>(this);
    wrap->addChild(&keyboard);
    
    saveButton = std::make_unique<UIButton>(SAVE, imageAssets->getIcon(ImageAssets::Icon::SAVE));
    saveButton->setPreferredSize(ofVec2f(140, 140));
    saveButton->setPreserveSizeRatio(true);
    saveButton->setPivot(ofVec2f(.5f, .5f));
    saveButton->setAnchor(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    //saveButton->setAnchoredPosition(ofVec2f(-160, 0));
    saveButton->onPointerDown.connect<UISaveDialog, &UISaveDialog::handleButton>(this);
    wrap->addChild(saveButton.get());
    
    uiObjects.push_back(std::move(subWrap));
    
    ipDisplay = 1.0f;
    
    syncUI();
}

UISaveDialog::~UISaveDialog()
{
    resetButton->onPointerDown.disconnect<UISaveDialog, &UISaveDialog::handleButton>(this);
    saveButton->onPointerDown.disconnect<UISaveDialog, &UISaveDialog::handleButton>(this);
    closeButton->onPointerDown.disconnect<UISaveDialog, &UISaveDialog::handleButton>(this);
    keyboard.onKeyDown.disconnect<UISaveDialog, &UISaveDialog::handleKeyDown>(this);
}

void UISaveDialog::syncUI()
{
    const auto t = textBox.getText();
    const auto e =  textBox.getText().length() > 0;
    saveButton->setPointerEnabled(e);
    resetButton->setPointerEnabled(textBox.getText().length() > 1);
}

void UISaveDialog::synchronize()
{
    textBox.setText(library->getCurrentSceneName());
    syncUI();
}

void UISaveDialog::handleKeyDown(char c)
{
    const std::string s = {c};
    auto t = textBox.getText() + s;
    if (t.length() > 12) t = t.substr(0, 12);
    //ofLogNotice() << t;
    textBox.setText(t);
    syncUI();
}

void UISaveDialog::update(float dt)
{
    ipDisplay.update(dt);
    //wrap->setScale(ofVec2f(1, 1) * (.0f + 1.0f * ipDisplay));
}

void UISaveDialog::draw()
{
    auto c = ofColor::black;
    c.a = (.8f * ipDisplay) * 255;
    ofSetColor(c);
    ofDrawRectangle(ofVec2f::zero(), getSize().x, getSize().y);
}

void UISaveDialog::handleUIAction(UIAction action)
{
    switch(action)
    {
        case SAVE:
            library->saveCurrentScene(textBox.getText());
            onExit.emit();
            break;
            
        case EXIT:
            onExit.emit();
            break;
            
        case DELETE:
            textBox.setText("");
            syncUI();
            break;
            
        default: return;
    }
}

void UISaveDialog::handleButton(UIButton* button)
{
    handleUIAction(button->getAction());
}

void UISaveDialog::show(const std::function<void()>& callback)
{
    ipDisplay = .0f;
    ipDisplay.to(1.0f, .6f, CircEaseOut(), callback);
    keyboard.show();
    closeButton->show();
    saveButton->show(.3f);
    resetButton->show();
    textBox.show();
}

void UISaveDialog::hide(const std::function<void()>& callback)
{
    ipDisplay.to(.0f, .8f, Linear(), callback);
    keyboard.hide();
    closeButton->hide(.3f);
    saveButton->hide();
    resetButton->hide();
    textBox.hide();
}

