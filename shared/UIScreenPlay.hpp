//
//  UIScreenPlay.hpp
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#pragma once

#include <memory>
#include "ofMain.h"
#include "UIBaseObject.hpp"
#include "UIMenu.hpp"
#include "UISequencer.hpp"
#include "Sequencer.hpp"
#include "RopesController.hpp"
#include "ImageAssets.hpp"
#include "UISlider.hpp"

class UIScreenPlay : public UIBaseObject
{
    
public:
    
    UIScreenPlay();
    ~UIScreenPlay();
    //void resize(float screenWidth, float screenHeight);
    
    void show();
    void hide();
    
private:
    
    void handleBpmSlider(UISlider* slider);
    void handleButton(UIButton* button);
    void handleUIAction(UIAction action);
    void ropesControllerModeChanged(RopesController::State state);
    //void ropePlucked(Rope* rope, float velocity);
    void sequencerFullScreenChanged();
    void syncUI();
    
    // we only used one container,
    // but we don't want both menus to be at the same depth anymore
    UIBaseObject modeMenuContainer;
    UIBaseObject seqMenuContainer;
    //UISoundControls soundControls;
    UIMenu modeMenu;
    UIMenu seqMenu;
    UISlider bpmSlider;
    std::shared_ptr<RopesController> ropesController;
    std::shared_ptr<Sequencer> sequencer;
    std::unique_ptr<UISequencer> uiSequencer;
};
