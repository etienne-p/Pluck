//
//  UI.cpp
//  Harp2
//
//  Created by etienne cella on 2015-11-19.
//
//

#include "UI.hpp"

UI::UI()
{
    screens.push_back(&saveDialog);
    screens.push_back(&library);
    screens.push_back(&screenPlay);
    
    auto imageAssets = Locator::get<ImageAssets>();
    
    screenPlay.setFlexibleSize(ofVec2f(1, 1));
    addChild(&screenPlay);
    
    menu.setLayoutType(HORIZONTAL);
    menu.setPreferredSize(ofVec2f(0, 300));
    menu.setFlexibleSize(ofVec2f(1, 0));
    menu.setPadding(Padding(48, 64, 0, 0));
    menu.setSpaceBetweenChilds(12);
    menu.addButton<UIButton>(SCENE_OPEN, imageAssets->getIcon(ImageAssets::Icon::OPEN));
    menu.addButton<UIButton>(SCENE_SAVE, imageAssets->getIcon(ImageAssets::Icon::SAVE));
    menu.changed.connect<UI, &UI::handleUIAction>(this);
    addChild(&menu);
    
    saveDialog.setPivot(ofVec2f(.5f, .5f));
    saveDialog.setFlexibleSize(ofVec2f(1.0f, 1.0f));
    saveDialog.onExit.connect<UI, &UI::hideSaveDialog>(this);
    
    library.setPivot(ofVec2f(.5f, .5f));
    library.setFlexibleSize(ofVec2f(1.0f, 1.0f));
    library.onExit.connect<UI, &UI::hideLibrary>(this);
    
    showLibrary(true);
}

UI::~UI()
{
    saveDialog.onExit.disconnect<UI, &UI::hideSaveDialog>(this);
    menu.changed.disconnect<UI, &UI::handleUIAction>(this);
}

void UI::showSaveDialog()
{
    menu.setPointerEnabled(false);
    for (auto& s : screens) s->setPointerEnabled(false);
    saveDialog.synchronize();
    addChild(&saveDialog);
    setLayoutDirty();
    saveDialog.show([this] { saveDialog.setPointerEnabled(true);});
}

void UI::hideSaveDialog()
{
    for (auto& s : screens) s->setPointerEnabled(false);
    saveDialog.hide([this]
    {
        removeChild(&saveDialog);
        screenPlay.setPointerEnabled(true);
        menu.setPointerEnabled(true);
    });
}

void UI::showLibrary(bool immediate)
{
    menu.setPointerEnabled(false);
    for (auto& s : screens) s->setPointerEnabled(false);
    library.synchronize();
    addChild(&library);
    setLayoutDirty();
    library.show([this] { library.setPointerEnabled(true);}, immediate);
}

void UI::hideLibrary()
{
    for (auto& s : screens) s->setPointerEnabled(false);
    library.hide([this]
    {
        removeChild(&library);
        screenPlay.setPointerEnabled(true);
        menu.setPointerEnabled(true);
    });
}

void UI::handleUIAction(UIButton* button)
{
    switch(button->getAction())
    {
        case SCENE_SAVE:
            showSaveDialog();
            break;
            
        case SCENE_OPEN:
            showLibrary();
            break;
            
        default: return;
    };
}


