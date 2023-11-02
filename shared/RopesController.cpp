//
//  RopesController.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-24.
//
//

#include "RopesController.hpp"

CreateRopeControl::CreateRopeControl()
{
    setLayoutType(LayoutType::NONE);
    ipDisplay = 1.0f;
    textBox.setFont(Fonts::getFontH1());
    addChild(&textBox);
}

void CreateRopeControl::update(float dt)
{
    ipDisplay.update(dt);
    textBox.ipDisplay = ipDisplay;
}

void CreateRopeControl::draw()
{
    auto c = ofColor::white;
    c.a = 40.0f;
    ofSetColor(c);
    constexpr auto touchRadius = 60.0f;
    const float ratio = ipDisplay;
    //Graphics::drawAnnularWedge(from, max(.0f, ratio * touchRadius * .2f), ratio * touchRadius, 0, PI * 2.0f);
    //Graphics::drawAnnularWedge(to, max(.0f, ratio * touchRadius * .2f), ratio * touchRadius, 0, PI * 2.0f);
    ofDrawCircle(to, ratio * touchRadius);
    ofDrawCircle(from, ratio * touchRadius);
    auto d = (to - from);
    d.normalize();
    d *= ratio * touchRadius;
    const auto lFrom = from + d;
    const auto lTo = to - d;
    ofDrawLine(lFrom, lFrom + (lTo - lFrom) * min(1.0f, ratio));
}

MovingRopeControl::MovingRopeControl()
{
    ipDisplay = 1.0f;
    textBox.setFont(Fonts::getFontH1());
    addChild(&textBox);
    textBox.setAnchoredPosition(ofVec2f(88.0f, -40.0f));
}

void MovingRopeControl::update(float dt)
{
    ipDisplay.update(dt);
    textBox.ipDisplay = ipDisplay;
}

void MovingRopeControl::draw()
{
    auto c = ofColor::white;
    c.a = 40.0f;
    ofSetColor(c);
    constexpr auto touchRadius = 60.0f;
    ofDrawCircle(0, 0, ipDisplay * touchRadius);
}

RopesController::RopesController(AudioEngine* audioEngine_)
{
    audioEngine = audioEngine_;
    
    camera = Locator::get<ofCamera>();
    assert(camera != nullptr);
    sequencer = Locator::get<Sequencer>();
    
    tri.resize(3);
    
    //for (auto& v : touchVelocities) v = .0f; // useful?
    
    //soundControls.setFlexibleSize(ofVec2f(1, 0));
    soundControls.setPreferredSize(ofVec2f(400.0f, 220.0f));
    //soundControls.setPivot(ofVec2f(.5f, ));
    soundControls.setAnchoredPosition(ofVec2f(-200.0f, -320.0f));
    //soundControls.setAlign(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    //soundControls.setAnchor(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::BOTTOM);
    soundControls.setAnchor(Layout::HorizontalAlign::CENTER, Layout::VerticalAlign::MIDDLE);
    soundControls.visible = false;
    soundControls.setPointerEnabled(false);
}

RopesController::~RopesController() { setState(NONE); }

// we do not want to expose soundControls, they really belong here
// we just have to cope with design considerations
void RopesController::setSoundControlsParent(Transform2D* parent)
{
    parent->addChild(&soundControls);
}

void RopesController::reset()
{
    for (auto& crc : createRopeControls) crc.second->destroyed = true;
    for (auto& mrc : movingRopeControls) mrc.second->destroyed = true;
    update(.0f); // to prevent pool handling code duplication
}

void RopesController::clear()
{
    for (auto& rope : ropes) rope->destroy();
    reset(); // will trigger an update
}

ofXml RopesController::toXml()
{
    ofXml xml;
    xml.addChild("strings");
    xml.setTo("strings");
    for (const auto& rope : ropes)
    {
        ofXml sxml;
        sxml.addChild("string");
        sxml.setTo("string");
        sxml.addValue("id", rope->getId());
        sxml.addValue("anchor_a", rope->getAnchorPosition(0));
        sxml.addValue("anchor_b", rope->getAnchorPosition(1));
        sxml.addValue("sustain", rope->audioSettings.getSustain());
        sxml.addValue("brilliance", rope->audioSettings.getBrilliance());
        sxml.addValue("frequency", rope->audioSettings.getRawFrequency());
        sxml.reset();
        xml.addXml(sxml);
    };
    return xml;
}

void RopesController::fromXml(ofXml& xml)
{
    clear();
    
    xml.reset();
    xml.setTo("scene");
    xml.setTo("strings");
    if (!xml.exists("string")) return;
    xml.setTo("string[0]");
    
    do
    {
        auto id = xml.getValue<int>("id");
        maxId = std::max(maxId, id + 1);
        auto rope = std::make_unique<Rope>(id);
        rope->moveAnchor(xml.getValue<ofVec3f>("anchor_a"), 0);
        rope->moveAnchor(xml.getValue<ofVec3f>("anchor_b"), 1);
        rope->audioSettings.setBrilliance(xml.getValue<float>("brilliance"));
        rope->audioSettings.setSustain(xml.getValue<float>("sustain"));
        rope->audioSettings.setRawFrequency(xml.getValue<float>("frequency"));
        rope->reset();
        rope->anim = 1.0f;
        ropesById[rope->getId()] = rope.get();
        
        ropes.push_back(std::move(rope));
    }
    while( xml.setToSibling() ); // go to the next STROKE
}

void RopesController::pointerDownHandler(TouchEvent& touch)
{
    touchesInfo[touch.id].reset();
    touchesInfo[touch.id].position = screenToWorld(touch.globalPosition);
    
    switch(state)
    {
        case NONE:     return;
        case EDIT:     editPointerDownHandler(touch); return;
        case PLAY:     playPointerDownHandler(touch); return;
        case DELETE:   deletePointerHandler(touch);   return;
        case SETTINGS: settingsPointerDownHandler(touch); return;
    }
}

void RopesController::pointerMovedHandler(TouchEvent& touch)
{
    // evaluate speed in world coordinates to prevent variations on screen resize
    const auto worldPosition = screenToWorld(touch.globalPosition);
    touchesInfo[touch.id].distance += (worldPosition - touchesInfo[touch.id].position).length();
    touchesInfo[touch.id].position.set(worldPosition);
    
    switch(state)
    {
        case NONE:   return;
        case EDIT:   editPointerMovedHandler(touch); break;
        case PLAY:   playPointerMovedHandler(touch); break;
        case DELETE: deletePointerHandler(touch);    break;
        case SETTINGS: settingsPointerMovedHandler(touch); return;
        default: return;
    }
}

void RopesController::pointerUpHandler(TouchEvent& touch)
{
    switch(state)
    {
        case NONE:   return;
        case EDIT:   editPointerUpHandler(touch); break;
        case PLAY:   playPointerUpHandler(touch); break;
        case DELETE: deletePointerHandler(touch); break;
        default: return;
    }
}

void RopesController::setState(State state_)
{
    if (state == state_) return;
    
    for (auto& p : movingRopeControls)
    {
        p.second->ipDisplay.to(.0f, .5f, CircEaseIn(), [&p]{ p.second->destroyed = true; });
    }
    
    for (auto& p : createRopeControls)
    {
        p.second->ipDisplay.to(.0f, .5f, CircEaseIn(), [&p]{ p.second->destroyed = true; });
    }
    
    if (state == SETTINGS)
    {
        exitSettingsMode();
        hideSoundControls();
    }
    
    state = state_;
    if (state == SETTINGS)
    {
        showSoundControls();
    }
    onStateChanged.emit(state);
}

RopesController::State RopesController::getState() { return state; }

void RopesController::update(float dt)
{
    for (auto& i : touchesInfo)
    {
        i.pushInfo(.018f * i.distance, dt);
        i.distance = .0f;
        break;
    }
    
    for (auto& rope : ropes) { rope->update(dt); }
    
    if (soundControlsActivated)
    {
        for (auto& rope : ropes)
        {
            if (rope->isSelected()) soundControls.writeSettings(rope->audioSettings);
        }
    }

    auto ptsItr = movingRopeControls.begin();
    while (ptsItr != movingRopeControls.end())
    {
        if ((*ptsItr).second->destroyed)
        {
            removeChild((*ptsItr).second);
            (*ptsItr).second->destroyed = false;
            movingRopeControlsPool.returnItem((*ptsItr).second);
            ptsItr = movingRopeControls.erase(ptsItr);
        }
        else ++ptsItr;
    }
    
    auto segItr = createRopeControls.begin();
    while (segItr != createRopeControls.end())
    {
        if ((*segItr).second->destroyed)
        {
            removeChild((*segItr).second);
            (*segItr).second->destroyed = false;
            createRopeControlsPool.returnItem((*segItr).second);
            segItr = createRopeControls.erase(segItr);
        }
        else ++segItr;
    }
    
    // remove destroyed ropes
    
    auto iter = std::begin(ropesById);
    for(; iter != std::end(ropesById); )
    {
        if(iter->second->isDestroyed())
        {
            //availableIds.push(iter->first); // free id
            ropesById.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
    
    ropes.erase(std::remove_if(std::begin(ropes), std::end(ropes),
                                  [](const std::unique_ptr<Rope>& mUPtr)
                                  {
                                      return mUPtr->isDestroyed();
                                  }),
                   std::end(ropes));
    
    // handle played notes
    if (sequencer->isPlaying())
    {
        for (const auto& e : sequencer->getFrameEvents())
        {
            // make sure that the rope has not been destroyed before plucking it
            if (ropesById.count(e.id))
            {
                ropesById[e.id]->pluck(e.velocity);
                audioEngine->pluck(ropesById[e.id]->audioSettings, e.velocity, e.id);
                pluckCount++;
            }
        }
    }
    
}

/*
void RopesController::draw()
{
    auto x = 12.0f, y = 12.0f, w = 12.0f, h = 48.0f;
    for(auto& i : touchesInfo)
    {
        ofSetColor(ofColor::white);
        ofDrawRectangle(x, y, w, h);
        ofSetColor(ofColor::red);
        ofDrawRectangle(x, y, w, h * i.getVelocity());
        x += w * 1.2f;
    }
}
 */

void RopesController::editPointerDownHandler(TouchEvent& touch)
{
    auto i = -1;
    for (auto& rope : ropes)
    {
        ++i;
        int dragExtremity = -1;
        
        if ((worldToScreen(rope->getAnchorPosition(0)) - touch.globalPosition).length() < 20.0f)
        {
            dragExtremity = 0;
        }
        else if((worldToScreen(rope->getAnchorPosition(1)) - touch.globalPosition).length() < 20.0f)
        {
            dragExtremity = 1;
        }
        
        if (dragExtremity == -1) continue;
        
        MovingRopeControl* movingRopeControl = movingRopeControlsPool.getItem();
        if (movingRopeControl != nullptr)
        {
            addChild(movingRopeControl);
            movingRopeControl->setPosition(touch.localPosition);
            movingRopeControl->dragExtremity = dragExtremity;
            movingRopeControl->ropeId = rope->getId();
            movingRopeControl->ipDisplay = .0f;
            movingRopeControl->ipDisplay.to(1.0f, .5f, BackEaseOut());
            movingRopeControls[touch.id ] = movingRopeControl;
            rope->pluck();
            editPointerMovedHandler(touch); // force control proper positioning
        }
        touch.canceled = true;
        return;
    };
    
    CreateRopeControl* createRopeControl = createRopeControlsPool.getItem();
    if (createRopeControl != nullptr)
    {
        createRopeControl->setScale(ofVec2f::one());
        addChild(createRopeControl);
        createRopeControl->from = touch.localPosition;
        createRopeControl->to = touch.localPosition;
        createRopeControl->ipDisplay = .0f;
        createRopeControl->ipDisplay.to(1.0f, .5f, BackEaseOut());
        createRopeControls[touch.id] = createRopeControl;
        touch.canceled = true;
        editPointerMovedHandler(touch);
    }
}

void RopesController::editPointerMovedHandler(TouchEvent& touch)
{
    if (movingRopeControls.find(touch.id) != movingRopeControls.end())
    {
        movingRopeControls[touch.id]->setPosition(touch.localPosition);
        auto rope = ropesById[movingRopeControls[touch.id]->ropeId];
        rope->moveAnchor(screenToWorld(localToGlobal(movingRopeControls[touch.id]->getPosition())),
                         movingRopeControls[touch.id]->dragExtremity);
        rope->syncAudioSettings();
        const auto note = rope->audioSettings.getNote();
        movingRopeControls[touch.id]->textBox.setText(note.label);
        touch.canceled = true;
    }
    
    if (createRopeControls.find(touch.id) != createRopeControls.end())
    {
        createRopeControls[touch.id]->to = touch.localPosition;
        
        const auto a = screenToWorld(localToGlobal(createRopeControls[touch.id]->from));
        const auto b = screenToWorld(localToGlobal(createRopeControls[touch.id]->to));
        const auto note = Utils::Math::snapToNote(Utils::Math::lengthTofreq((b - a).length()));
        createRopeControls[touch.id]->textBox.setText(note.label);
        createRopeControls[touch.id]->textBox.setPosition(touch.localPosition + ofVec2f(88.0f, -40.0f));
        touch.canceled = true;
    }
}

void RopesController::editPointerUpHandler(TouchEvent& touch)
{
    if (movingRopeControls.find(touch.id) != movingRopeControls.end())
    {
        auto mc = movingRopeControls[touch.id];
        mc->ipDisplay.to(.0f, .5f, CircEaseIn(), [mc]{ mc->destroyed = true; });
        
        // Change key to free it for new touch
        const auto it = movingRopeControls.find(touch.id);
        auto value = it->second;
        movingRopeControls.erase(it);
        int newkey = 100; // touches are far from reaching that
        while(movingRopeControls.count(newkey) == 1) ++newkey; // find free key
        movingRopeControls[newkey] = value;
    }
    
    if (createRopeControls.find(touch.id) != createRopeControls.end())
    {
        //const ofVec2f touchPos(touch.globalPosition.x, touch.globalPosition.y);
        if ((createRopeControls[touch.id]->from - createRopeControls[touch.id]->to).length() > 40.0f)
        {
            auto rope = std::make_unique<Rope>(getUId());
            rope->moveAnchor(screenToWorld(localToGlobal(createRopeControls[touch.id]->from)), 0);
            rope->moveAnchor(screenToWorld(localToGlobal(createRopeControls[touch.id]->to)), 1);
            rope->syncAudioSettings();
            rope->reset();
            rope->anim = .0f;
            rope->anim.to(1.0f, .3f, BackEaseOut());
            rope->pluck();
            ropesById[rope->getId()] = rope.get();
            ropes.push_back(std::move(rope));
        }
        
        auto cc = createRopeControls[touch.id];
        //cc->destroyed = true;
        cc->ipDisplay.to(.0f, .5f, CircEaseIn(), [cc]{ cc->destroyed = true; });
        
        // Change key to free it for new touch
        const auto it = createRopeControls.find(touch.id); // find free key
        auto value = it->second;
        createRopeControls.erase(it);
        int newkey = 100; // touches are far from reaching that
        while(createRopeControls.count(newkey) == 1) ++newkey;
        createRopeControls[newkey] = value;
    }
}

void RopesController::playPointerDownHandler(TouchEvent& touch)
{
    previousPositions[touch.id] = ofVec2f(touch.globalPosition.x, touch.globalPosition.y);
}

void RopesController::playPointerMovedHandler(TouchEvent& touch)
{
    if (previousPositions.find(touch.id) == previousPositions.end()) return;
    
    //const ofVec2f pos(touch.globalPosition.x, touch.globalPosition.y);
    
    tri[0] = camera->getPosition();
    tri[1] = screenToWorld(touch.globalPosition, 1.0f);
    tri[2] = screenToWorld(previousPositions[touch.id], 1.0f);
    
    for (auto& rope : ropes)
    {
        const auto dir = rope->getAnchorPosition(1) - rope->getAnchorPosition(0);
        
        const auto info = Utils::Math::intersectRayTri(rope->getAnchorPosition(0), dir, tri);
        
        if (info.bIntersect && info.distance <= dir.length())
        {
            const auto intersectionPoint = rope->getAnchorPosition(0) + dir * info.distance / dir.length();
            const auto radius = (intersectionPoint - camera->getPosition()).length();
            // project rays on the ropes bounding sphere
            const auto a = (tri[1] - camera->getPosition());//.normalize();
            const auto b = (tri[2] - camera->getPosition());//.normalize();
            const auto force = (b - a).normalize() * touchesInfo[touch.id].getVelocity();
            rope->pluck(intersectionPoint, force * 10.0f);
            sequencer->pushSequencerEvent(rope->getId(), touchesInfo[touch.id].getVelocity());
            audioEngine->pluck(rope->audioSettings, touchesInfo[touch.id].getVelocity(), rope->getId());
            //onPluck.emit(rope.get(), touchesInfo[touch.id].getVelocity());
            pluckCount++;
        }
    };
    
    previousPositions[touch.id] = touch.globalPosition;
}

void RopesController::playPointerUpHandler(TouchEvent& touch)
{
    if (previousPositions.find(touch.id) != previousPositions.end()) previousPositions.erase(touch.id);
}

void RopesController::deletePointerHandler(TouchEvent& touch)
{
    auto deleteCount = 0;
    for (auto& rope : ropes)
    {
        if ((isTouchingRope(touch.globalPosition, rope.get())) && !rope->destroyScheduled)
        {
            deleteCount++;
            rope->destroyScheduled = true;
            auto r = rope.get();
            rope->anim.to(.0f, .3f, Linear(), [r]{ r->destroy(); });
        }
    };
    
    if (deleteCount > 0) removeDeletedEvents();
}

void RopesController::settingsPointerDownHandler(TouchEvent &touch)
{
    exitSettingsMode(); // reset selection
    settingsPointerMovedHandler(touch);
}

void RopesController::settingsPointerMovedHandler(TouchEvent &touch)
{
    int i = -1;
    for (auto& rope : ropes)
    {
        ++i;
        if (isTouchingRope(touch.globalPosition, rope.get()))
        {
            if (rope->isSelected())
            {
                continue;
            }
            else
            {
                selectedCount++;
                rope->setSelected(true);
                if (selectedCount == 1)
                {
                    audioSettings.copy(rope->audioSettings);
                    soundControls.readSettings(audioSettings);
                }
            }
        }
    };
}

bool RopesController::isTouchingRope(ofVec2f touchScreenPosition, Rope* rope)
{
    const auto touch = screenToWorld(ofVec2f(touchScreenPosition.x, touchScreenPosition.y));
    
    return Utils::Math::pointToSegmentDistance(
        touch,
        rope->getAnchorPosition(0),
        rope->getAnchorPosition(1)) < rope->lineWidth * .5f;
}

void RopesController::exitSettingsMode()
{
    selectedCount = 0;
    for (auto& rope : ropes) rope->setSelected(false);
}

void RopesController::removeDeletedEvents()
{
    vector<int> ids;
    //ids.reserve(ropes.size());
    for (auto& r : ropes) if(!r->destroyScheduled) ids.push_back(r->getId());
    sequencer->filterEvents(ids);
}

void RopesController::showSoundControls()
{
    // TODO: use show(); method;
    
    //soundControls.readSettings(rope->audioSettings);
    soundControls.visible = true;
    soundControls.show([this]{ soundControls.setPointerEnabled(true); });
    soundControlsActivated = true;
}

void RopesController::hideSoundControls()
{
    soundControlsActivated = false;
    soundControls.setPointerEnabled(false);
    // TODO: use hide() method?
    soundControls.hide([this]{ soundControls.visible = false; });
}

ofVec2f RopesController::worldToScreen(const ofVec3f& input)
{
    return camera->worldToScreen(input);
}

ofVec3f RopesController::screenToWorld(const ofVec2f& input, float depth)
{
    return camera->screenToWorld(ofVec3f(input.x, input.y, depth));
}

const std::vector<std::unique_ptr<Rope>>& RopesController::getRopes() const { return ropes; };


int RopesController::getUId()
{
    return ++maxId;
}

int RopesController::getPluckCountElapsed()
{
    auto tmp = pluckCount;
    pluckCount = 0;
    return tmp;
}

void RopesController::pause()
{
    ofLogNotice("ofApp") << "pause RopesController";
    for (auto& rope : ropes) rope->pause();
}

void RopesController::resume()
{
    ofLogNotice("ofApp") << "resume RopesController";
    reset();
    for (auto& rope : ropes) rope->resume();
}
