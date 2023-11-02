//
//  Sequencer.cpp
//  Harp2
//
//  Created by etienne cella on 2015-11-28.
//
//

#pragma once

#include "Sequencer.hpp"
#include "InterpolatedProperty.h"
#include "Graphics.hpp"
#include "UIBaseObject.hpp"
#include "UIButton.hpp"
#include "UIMenu.hpp"
#include "UITextBox.hpp"
#include "Fonts.hpp"
#include "ImageAssets.hpp"
#include "Locator.h"
#include "Settings.h"
#include "UICircSlider.hpp"

// evnts in time
// does not know about what these SequencerEvents mean

struct UISequencerPattern
{
    float offset{.0f};
    //float radiusRatio{.0f};
    float minRadius{.0f}; // used for touch
    float maxRadius{.0f}; // used for touch
    
    bool activeFlag{true};
    bool focusFlag{false};
    
    InterpolatedProperty ipDisplay;
    InterpolatedProperty ipFocus;
    InterpolatedProperty ipActive;
    
    UISequencerPattern()
    {
        ipDisplay = 1.0f;
        ipFocus = .0f;
        ipActive = 1.0f;
    }
};

class UISequencer : public UIBaseObject
{
    
public:
    
    Nano::Signal<void()> onFullScreenChanged;
    
    UISequencer(Sequencer* sequencer_);
    ~UISequencer();
    
    void update(float dt);
    void draw();
    void pointerDownHandler(TouchEvent& touch);
    //void show(const std::function<void()>& callback = 0);
    //void hide();
    void syncUI();
    void resume();
    void pause();

    void setFullScreen(bool value, const std::function<void()>& callback = 0, bool immediate = false);
    bool isFullScreen() const;
    
private:
    
    // drawing notes
    void resizeNotes();
    // will be executed on fullscreen only
    void updateNotes();
    void markNotesDirty();
    bool notesDirty{false};
    std::vector<ofVec3f> notesVertices;
    ofVbo notesParticles;
    
    bool fullScreen{true};
    bool minDirty{false};
    ofVec2f origin;
    float radius;
    
    void drawRect(float x, float y, float w, float h);
    void handleButton(UIButton* button);
    std::vector<UIButton*> buttons;
    
    Sequencer* sequencer;
    InterpolatedProperty ipExpand;
    std::vector<std::unique_ptr<UIBaseObject>> uiObjects; // memory management
    // pattern views matching sequencer patterns
    std::array<UISequencerPattern, Sequencer::NUM_PATTERNS> patterns;
    BackEaseIn easingBack;
    
    UIBaseObject menu;
    std::unique_ptr<UIButton> shrinkButton;
    std::unique_ptr<UIButton> decreaseTimeMulButton;
    std::unique_ptr<UIButton> increaseTimeMulButton;
    std::unique_ptr<UIButton> clearPatternButton;
    std::unique_ptr<UIMultiStateButton> togglePatternButton;
    
    ofShader shaderRadial;
    ofShader shaderOutline;
    ofVboMesh quad;
    ofMesh rectMesh;
    ofFbo fbo;
    ofFbo minFbo;
};
