//
//  UI.hpp
//  Harp2
//
//  Created by etienne cella on 2015-11-19.
//
//

#pragma once

#include <memory>
#include "ofMain.h"
#include "UIBaseObject.hpp"
#include "UIMenu.hpp"
#include "UIScreenPlay.hpp"
#include "ImageAssets.hpp"
#include "UISaveDialog.hpp"
#include "Settings.h"
#include "UILibrary.hpp"

class UI : public UIBaseObject
{
    
public:
    
    UI();
    ~UI();
    //void resize(float screenWidth, float screenHeight);
    
private:
    
    void handleUIAction(UIButton* button);
    void showSaveDialog();
    void hideSaveDialog();
    void showLibrary(bool immediate = false);
    void hideLibrary();
    
    UIScreenPlay screenPlay;
    UISaveDialog saveDialog;
    UILibrary library;
    UIMenu menu;
    UIBaseObject* currentScreen{nullptr};
    std::vector<UIBaseObject*> screens;

};