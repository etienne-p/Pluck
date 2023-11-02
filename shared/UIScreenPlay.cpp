//
//  UIScreenPlay.cpp
//  Harp2
//
//  Created by etienne cella on 2016-01-02.
//
//

#include "UIScreenPlay.hpp"

UIScreenPlay::UIScreenPlay()
{
    auto imageAssets = Locator::get<ImageAssets>();
    
    ropesController = Locator::get<RopesController>();
    ropesController->onStateChanged.connect<UIScreenPlay, &UIScreenPlay::ropesControllerModeChanged>(this);
    //ropesController->onPluck.connect<UIScreenPlay, &UIScreenPlay::ropePlucked>(this);
    ropesController->setFlexibleSize(ofVec2f(1, 1));  // fill screen
    addChild(ropesController.get());
    
    modeMenuContainer.setPreferredSize(ofVec2f(0, 220));
    modeMenuContainer.setAnchoredPosition(ofVec2f(0, -220));
    modeMenuContainer.setAnchor(LEFT, BOTTOM);
    modeMenuContainer.setFlexibleSize(ofVec2f(1, 0));
    modeMenuContainer.setLayoutType(HORIZONTAL);
    addChild(&modeMenuContainer);

    modeMenu.setLayoutType(HORIZONTAL);
    modeMenu.setFlexibleSize(ofVec2f(1, 1));
    modeMenu.setPadding(Padding(0, 64, 0, 0));
    modeMenu.setSpaceBetweenChilds(24);
    modeMenu.addButton<UIButton>(MODE_EDIT, imageAssets->getIcon(ImageAssets::Icon::PEN));
    modeMenu.addButton<UIButton>(MODE_PLAY, imageAssets->getIcon(ImageAssets::Icon::NOTE));
    modeMenu.addButton<UIButton>(MODE_DELETE, imageAssets->getIcon(ImageAssets::Icon::DUMP));
    modeMenu.addButton<UIButton>(MODE_SETTINGS, imageAssets->getIcon(ImageAssets::Icon::SETTINGS));
    modeMenu.changed.connect<UIScreenPlay, &UIScreenPlay::handleButton>(this);
    modeMenuContainer.addChild(&modeMenu);
    // soundControls parent is a button from the mode menu
    ropesController->setSoundControlsParent(modeMenu.getButtonByAction(MODE_SETTINGS));
    
    sequencer = Locator::get<Sequencer>();
    uiSequencer = std::make_unique<UISequencer>(sequencer.get());
    //uiSequencer->captureEvents = true;
    uiSequencer->setFlexibleSize(ofVec2f(1, 1)); // fill screen
    uiSequencer->onFullScreenChanged.connect<UIScreenPlay, &UIScreenPlay::sequencerFullScreenChanged>(this);
    sequencer->changed.connect<UIScreenPlay, &UIScreenPlay::syncUI>(this);
    addChild(uiSequencer.get());
    uiSequencer->syncUI();
    
    seqMenuContainer.setPreferredSize(ofVec2f(0, 220));
    seqMenuContainer.setAnchoredPosition(ofVec2f(0, -220));
    seqMenuContainer.setAnchor(LEFT, BOTTOM);
    seqMenuContainer.setFlexibleSize(ofVec2f(1, 0));
    seqMenuContainer.setLayoutType(VERTICAL);
    addChild(&seqMenuContainer);
    
    seqMenu.setLayoutType(HORIZONTAL);
    seqMenu.setAlign(Layout::HorizontalAlign::RIGHT, Layout::VerticalAlign::TOP);
    seqMenu.setPadding(Padding(0, 0, 0, 64));
    seqMenu.setSpaceBetweenChilds(24);
    seqMenu.setFlexibleSize(ofVec2f(1, 1));
    seqMenu.addButton<UIMultiStateButton>(SEQ_PLAY_PAUSE, vector<std::shared_ptr<ofImage>>
                                          {
                                              imageAssets->getIcon(ImageAssets::Icon::PLAY),
                                              imageAssets->getIcon(ImageAssets::Icon::PAUSE)
                                          }); // multi state button
    seqMenu.addButton<UIRecordButton>(SEQ_RECORD);
    seqMenu.addButton<UIButton>(SEQ_METRO, imageAssets->getIcon(ImageAssets::Icon::METRO));
    seqMenu.changed.connect<UIScreenPlay, &UIScreenPlay::handleButton>(this);
    seqMenuContainer.addChild(&seqMenu);
    
    bpmSlider.setAnchor(RIGHT, BOTTOM);
    constexpr auto sliderWidth = 468;
    bpmSlider.setAnchoredPosition(ofVec2f(-sliderWidth - 64, -360));
    bpmSlider.setPreferredSize(ofVec2f(sliderWidth, 100));
    addChild(&bpmSlider);
    bpmSlider.onValueChange.connect<UIScreenPlay, &UIScreenPlay::handleBpmSlider>(this);
    
    ropesController->setState(RopesController::EDIT);
    sequencerFullScreenChanged();
    uiSequencer->setFullScreen(false, 0, true);
    
    syncUI();
}

UIScreenPlay::~UIScreenPlay()
{
    bpmSlider.onValueChange.disconnect<UIScreenPlay, &UIScreenPlay::handleBpmSlider>(this);
    modeMenu.changed.disconnect<UIScreenPlay, &UIScreenPlay::handleButton>(this);
    ropesController->onStateChanged.disconnect<UIScreenPlay, &UIScreenPlay::ropesControllerModeChanged>(this);
    //ropesController->onPluck.disconnect<UIScreenPlay, &UIScreenPlay::ropePlucked>(this);
    sequencer->changed.disconnect<UIScreenPlay, &UIScreenPlay::syncUI>(this);
    uiSequencer->onFullScreenChanged.disconnect<UIScreenPlay, &UIScreenPlay::sequencerFullScreenChanged>(this);
}

void UIScreenPlay::ropesControllerModeChanged(RopesController::State state)
{
    switch (state)
    {
        case RopesController::State::EDIT:     modeMenu.select(MODE_EDIT);     return;
        case RopesController::State::PLAY:     modeMenu.select(MODE_PLAY);     return;
        case RopesController::State::DELETE:   modeMenu.select(MODE_DELETE);   return;
        case RopesController::State::SETTINGS: modeMenu.select(MODE_SETTINGS); return;
        default: return;
    }
}

void UIScreenPlay::sequencerFullScreenChanged()
{
    ropesController->setPointerEnabled(!uiSequencer->isFullScreen());
    modeMenu.setPointerEnabled(!uiSequencer->isFullScreen());
}

/*
void UIScreenPlay::ropePlucked(Rope* rope, float velocity)
{
    sequencer->pushSequencerEvent(rope->getId(), velocity);
}
*/

void UIScreenPlay::syncUI()
{
    uiSequencer->syncUI();
    seqMenu.getButtonByAction(SEQ_RECORD)->setSelected(sequencer->isRecording());
    seqMenu.getButtonByAction(SEQ_METRO)->setSelected(sequencer->isTickerEnabled());
    const auto btn = dynamic_cast<UIMultiStateButton*>(seqMenu.getButtonByAction(SEQ_PLAY_PAUSE));
    btn->setState(sequencer->isPlaying() ? 1 : 0);
    const auto normalizedBpm = ofClamp(sequencer->getBPM() / 160.0f, .0f, 1.0f);
    bpmSlider.setNormalizedValue(normalizedBpm);
}

void UIScreenPlay::handleBpmSlider(UISlider* slider)
{
    //ofLogNotice("bpm", ofToString(slider->getNormalizedValue()));
    const auto bpm = 160.0f * slider->getNormalizedValue();
    sequencer->setBPM(bpm);
}

void UIScreenPlay::handleUIAction(UIAction action)
{
    switch (action)
    {
            // Mode
        case MODE_EDIT:     ropesController->setState(RopesController::EDIT);     return;
        case MODE_PLAY:     ropesController->setState(RopesController::PLAY);     return;
        case MODE_DELETE:   ropesController->setState(RopesController::DELETE);   return;
        case MODE_SETTINGS: ropesController->setState(RopesController::SETTINGS); return;

            // Sequencer
        case SEQ_PLAY_PAUSE:
        {
            const auto btn = dynamic_cast<UIMultiStateButton*>(seqMenu.getButtonByAction(SEQ_PLAY_PAUSE));
            switch(btn->getState())
            {
                case 0: sequencer->play(); break;
                    
                case 1:
                    sequencer->pause();
                    // auto stop recording on pause
                    if (sequencer->isRecording()) handleUIAction(SEQ_RECORD);
                    break;
            }
        }
            break;
            
        case SEQ_RECORD:
        {
            sequencer->setRecording(!sequencer->isRecording()); // toggle
        }
            break;
            
        case SEQ_METRO:
        {
            sequencer->setTickerEnabled(!sequencer->isTickerEnabled()); // toggle
        }
            break;
            
        default:
            return;
    }
    syncUI();
}

void UIScreenPlay::handleButton(UIButton* button)
{
    handleUIAction(button->getAction());
}
