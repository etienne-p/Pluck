//
//  ofAppCore.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-16.
//
//

#include "ofAppCore.hpp"

ofAppCore::ofAppCore()
{
    ofAddListener(ofEvents().touchDown, this, &ofAppCore::handleTouchEvent, 0);
    ofAddListener(ofEvents().touchMoved, this, &ofAppCore::handleTouchEvent, 0);
    ofAddListener(ofEvents().touchUp, this, &ofAppCore::handleTouchEvent, 0);
}

ofAppCore::~ofAppCore()
{
    ofRemoveListener(ofEvents().touchDown, this, &ofAppCore::handleTouchEvent, 0);
    ofRemoveListener(ofEvents().touchMoved, this, &ofAppCore::handleTouchEvent, 0);
    ofRemoveListener(ofEvents().touchUp, this, &ofAppCore::handleTouchEvent, 0);
    
    sequencer->tick.disconnect<ofAppCore, &ofAppCore::handleTick>(this);
}

void ofAppCore::setup()
{
    ofDisableArbTex();
    ofDisableSmoothing();
    ofDisableDepthTest();
    ofDisableLighting();
    ofDisableBlendMode();
    //ofSetVerticalSync(true);
    
    //uiSettings.setup();
    
    fbo.allocate(Settings::THUMBNAIL_SIZE, Settings::THUMBNAIL_SIZE, GL_RGB);
    
    Fonts::init(); // Force Fonts initialization
    imageAssets = Locator::create<ImageAssets>();
    sequencer = Locator::create<Sequencer>();
    sequencer->tick.connect<ofAppCore, &ofAppCore::handleTick>(this);
    camera = Locator::create<ofCamera>();
    ropesController = Locator::create<RopesController>(&audioEngine);
    library = Locator::create<Library>([this](const string& path){ saveScreenshot(path); });

    ropeShader.load(Utils::Platform::getShaderPath("rope.vert"),
                    Utils::Platform::getShaderPath("rope.frag"));
    
    soundVis.setup();
    ui = std::make_unique<UI>();
    audioEngine.reset(Settings::BUFFER_SIZE); // see ofSoundStream setup
    camera->setupPerspective(false, 60, 1, 500);
}

void ofAppCore::handleTick()
{
    audioEngine.tick();
}

void ofAppCore::update()
{
    while(touches.size() > 0)
    {
        auto e = touches.top();
        touches.pop();
        ui->traverseBackward([&e](Transform2D* t)
                     {
                         const auto p = static_cast<PointerHandler*>(t);
                         
                         if (!p->isPointerEnabled() || e.canceled)// && p->getDepth() > e.minDepth)
                         {
                             return false;
                         }
                         
                         e.localPosition = t->globalToLocal(e.globalPosition);
                         if(p->inside(e.localPosition))
                         {
                             switch(e.type)
                             {
                                 case ofTouchEventArgs::down: p->pointerDownHandler(e);  break;
                                 case ofTouchEventArgs::move: p->pointerMovedHandler(e); break;
                                 case ofTouchEventArgs::up:   p->pointerUpHandler(e);    break;
                                 default: return false; // prevent propagation of unhandled events
                             }
                         }
                         
                         return !e.canceled;
                     });
    }
    
    const auto dt = ofGetLastFrameTime();
    sequencer->update(dt);
    
    ui->traverse([&dt](Transform2D* t) // pre post order doesnt matter here
                 {
                     static_cast<UIBaseObject*>(t)->update(dt);
                     return true;
                 });
    
    ui->updateLayoutSize();
    ui->updateLayoutPosition();
    
    //uiSettings.update();
    
    soundVis.update(dt, ropesController->getPluckCountElapsed() > 0);

    // Audio
    /*
    int bufferSize = (ringBuffer.getWriteAvail() / 2) * 2; // prevent odd length
    float * tmpBuffer = new float[bufferSize];
    audioEngine.processAudio(tmpBuffer, bufferSize / 2, 2);
    ringBuffer.write(tmpBuffer, bufferSize);
    delete[] tmpBuffer;
     */
}

void ofAppCore::draw()
{
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe
    draw3DWorld(ofGetViewportWidth(), ofGetViewportHeight());
    drawUI();
#if DEBUG
    ofDrawBitmapString("DEBUG VERSION, FRAMERATE:" + ofToString(ofGetFrameRate()),
                       12, ofGetWindowHeight() - 12);
#endif

#ifdef draw_fingers
    ofSetColor(ofColor::white);
    auto icon = imageAssets->getIcon(ImageAssets::TOUCH);
    const auto ratio = (float)ofGetWindowHeight() / 1600.0f;
    auto w = (float)icon->getWidth() * ratio;
    auto h = (float)icon->getHeight() * ratio;
    for (auto& finger : fingers)
    {
        icon->draw(finger.second.x - w * .5, finger.second.y - h * .5, w, h);
    }
#endif
}

void ofAppCore::draw3DWorld(float width, float height)
{
    ofBackground(Settings::getDarkColor());
    
    //ofDisableBlendMode();
    ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_SCREEN);
    ofSetColor(ofColor::white);
    ofPushMatrix();
    ofTranslate(0, -(width - height) * .5f);
    //ofScale(ui->getSize().x * ui->getScale().x, ui->getSize().y * ui->getScale().y);
    ofSetColor(Settings::getMediumColor());
    soundVis.draw(width, width);
    ofPopMatrix();
    ofDisableBlendMode();
    
    ropeShader.begin();
    camera->begin();
    //ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_MULTIPLY);
    for (const auto& rope : ropesController->getRopes())
    {
        ofSetColor(rope->getColor());
        const auto borderColor = Settings::getDarkColor().lerp(ofColor::white, (float)rope->ipSelected);
        ropeShader.setUniform4f("borderColor", borderColor);
        rope->draw();
    }
    //ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);
    camera->end();
    ropeShader.end();
}

void ofAppCore::drawUI()
{
    ofSetColor(Settings::getLightColor());
    ofEnableAlphaBlending();
    ui->traverse([](Transform2D* t)
                 {
                     auto o = dynamic_cast<UIBaseObject*>(t);
                     if (o->visible)
                     {
                         ofPushMatrix();
                         ofMultMatrix(o->getGlobalTransformMatrix());
                         o->draw();
                         ofPopMatrix();
                         return true;
                     }
                     else return false;
                 });
    //ofDisableAlphaBlending();
}

void ofAppCore::keyPressed(int key)
{
    switch(key)
    {
        case 's': library->saveCurrentScene("demo"); break;
        default: return;
    }
}

bool ofAppCore::handleTouchEvent(ofTouchEventArgs &touch)
{
#ifdef draw_fingers
    switch (touch.type)
    {
        case ofTouchEventArgs::Type::cancel:
        case ofTouchEventArgs::Type::up:
        {
            auto it = fingers.find(touch.id);
            if (it != fingers.end()) fingers.erase(it);
                
        }
            break;
            
        case ofTouchEventArgs::Type::down:
            fingers[touch.id] = ofVec2f();
            
        case ofTouchEventArgs::Type::move:
            fingers[touch.id].set(touch.x, touch.y);
            break;
            
        default: break;
    }
#endif
    
    auto e = TouchEvent::convertEvent(touch);
    touches.push(e);
    // unhandled events should be interrupted here
}

void ofAppCore::windowResized(int w, int h)
{
    const auto screenWidth = (float)w;
    const auto screenHeight = (float)h;
    
    //const auto scaleFactor = screenHeight / Settings::REF_UI_SCREEN_HEIGHT;
    const auto scaleFactor = screenWidth / Settings::REF_UI_SCREEN_WIDTH;
    ui->setScale(ofVec2f(scaleFactor, scaleFactor));
    /*
    const auto width = Settings::REF_UI_SCREEN_HEIGHT * screenWidth / screenHeight;
    const auto height = Settings::REF_UI_SCREEN_HEIGHT;
    */
    const auto width = Settings::REF_UI_SCREEN_WIDTH;
    const auto height = Settings::REF_UI_SCREEN_WIDTH * screenHeight / screenWidth;
    
    ui->setSize(ofVec2f(width, height));
    ui->updateLayoutSize(); // sizes changes will bubble up the hierarchy
    ui->updateLayoutPosition(true);
}

void ofAppCore::saveScreenshot(const std::string& path)
{
    fbo.begin();
    ofPushMatrix();
    const auto screenWidth = ofGetViewportWidth();
    const auto screenHeight = ofGetViewportHeight();
    const auto ratio = (float)screenWidth / (float)screenHeight;
    const auto scale = min((float)Settings::THUMBNAIL_SIZE / (float)screenWidth, (float)Settings::THUMBNAIL_SIZE / (float)screenHeight);
    if (ratio > 1.0f) ofTranslate(.0f, (screenWidth - screenHeight) * .5f * scale);
    else ofTranslate((screenHeight - screenWidth) * .5f * scale, .0f);
    ofScale(scale, scale);
    draw3DWorld(Settings::THUMBNAIL_SIZE, Settings::THUMBNAIL_SIZE);
    ofPopMatrix();
    fbo.end();
    
    ofPixels pixels;
    pixels.allocate(Settings::THUMBNAIL_SIZE, Settings::THUMBNAIL_SIZE, ofImageType::OF_IMAGE_COLOR);
    fbo.readToPixels(pixels);
    
    ofImage img;
    img.allocate(Settings::THUMBNAIL_SIZE, Settings::THUMBNAIL_SIZE, ofImageType::OF_IMAGE_COLOR);
    img.setFromPixels(pixels);
    img.save(path);
}

void ofAppCore::resume()
{
    ofLogNotice("ofApp") << "resume appcore";
    fbo.allocate(Settings::THUMBNAIL_SIZE, Settings::THUMBNAIL_SIZE, GL_RGB);
    //soundVis.resume();
    ui->traverse([](Transform2D* t)
                 {
                     auto o = dynamic_cast<UIBaseObject*>(t);
                     if (o != nullptr) o->resume();
                     return true;
                 });
}

void ofAppCore::pause()
{
    ofLogNotice("ofApp") << "pause appcore";
    fbo.clear();
    //soundVis.pause();
    ui->traverse([](Transform2D* t)
                 {
                     auto o = dynamic_cast<UIBaseObject*>(t);
                     if (o != nullptr) o->pause();
                     return true;
                 });
}

void ofAppCore::audioRequested(float * output, int bufferSize, int nChannels)
{
    audioEngine.processAudio(output, bufferSize, 2);
    
    /*
    if (bufferSize * nChannels > ringBuffer.getReadAvail())
    {
        ofLogNotice("Audio buffer underflow!");
    }
    ringBuffer.read(output, min(bufferSize * nChannels, ringBuffer.getReadAvail()));
     */
}