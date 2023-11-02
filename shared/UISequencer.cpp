#include "UISequencer.hpp"

UISequencer::UISequencer(Sequencer* sequencer_)
{
    sequencer = sequencer_;
    sequencer->notesChanged.connect<UISequencer, &UISequencer::markNotesDirty>(this);
    markNotesDirty();
    
    resume();
    
    shaderRadial.load(Utils::Platform::getShaderPath("ui_seq.vert"),
                      Utils::Platform::getShaderPath("ui_seq_radial.frag"));
    
    shaderOutline.load(Utils::Platform::getShaderPath("ui_seq.vert"),
                       Utils::Platform::getShaderPath("ui_seq_outline.frag"));
    
    
    auto imageAssets = Locator::get<ImageAssets>();
    
    ipExpand = 1.0f;
    
    decreaseTimeMulButton = std::make_unique<UIButton>(PATTERN_DECREASE_TIME_MUL,
                                                       imageAssets->getIcon(ImageAssets::Icon::ADD));
    increaseTimeMulButton = std::make_unique<UIButton>(PATTERN_INCREASE_TIME_MUL,
                                                       imageAssets->getIcon(ImageAssets::Icon::SUB));
    clearPatternButton = std::make_unique<UIButton>(PATTERN_CLEAR,
                                                    imageAssets->getIcon(ImageAssets::Icon::DUMP));
    vector<std::shared_ptr<ofImage>> ics =
    {
        imageAssets->getIcon(ImageAssets::Icon::VOLUME_ON),
        imageAssets->getIcon(ImageAssets::Icon::VOLUME_OFF)
    };
    togglePatternButton = std::make_unique<UIMultiStateButton>(PATTERN_ACTIVE, ics);
    
    shrinkButton = std::make_unique<UIButton>(SEQ_SHRINK,
                                              imageAssets->getIcon(ImageAssets::Icon::CLOSE));
    
    shrinkButton->setPreferredSize(ofVec2f(140, 140));
    shrinkButton->setPreserveSizeRatio(true);
    shrinkButton->setPivot(ofVec2f(.5f, .5f));
    shrinkButton->onPointerDown.connect<UISequencer, &UISequencer::handleButton>(this);
    addChild(shrinkButton.get());
    
    //buttons.push_back(decreaseTimeMulButton.get());
    //buttons.push_back(increaseTimeMulButton.get());
    buttons.push_back(clearPatternButton.get());
    buttons.push_back(togglePatternButton.get());
    
    for (auto btn : buttons)
    {
        btn->setPreferredSize(ofVec2f(140, 140));
        btn->setPreserveSizeRatio(true);
        btn->setPivot(ofVec2f(.5f, .5f));
        //btn->color = Settings::getMediumColor();
        //btn->hide();
        addChild(btn);
        btn->onPointerDown.connect<UISequencer, &UISequencer::handleButton>(this);
    }
    
}

void UISequencer::pause()
{
    ofLogNotice("ofApp") << "pause UISequencer";

    quad.clear();
    rectMesh.clear();
    fbo.clear();
    minFbo.clear();
}

void UISequencer::resume()
{
    ofLogNotice("ofApp") << "resume UISequencer";

    quad.addVertex(ofVec2f(-1, -1));
    quad.addTexCoord(ofVec2f(-1, -1));
    quad.addVertex(ofVec2f(1, -1));
    quad.addTexCoord(ofVec2f(1, -1));
    quad.addVertex(ofVec2f(1, 1));
    quad.addTexCoord(ofVec2f(1, 1));
    quad.addVertex(ofVec2f(-1, 1));
    quad.addTexCoord(ofVec2f(-1, 1));
    quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    
    rectMesh.getVertices().resize(4);
    rectMesh.getTexCoords().resize(4);
    rectMesh.getTexCoords()[0].set(-1, -1);
    rectMesh.getTexCoords()[1].set(1, -1);
    rectMesh.getTexCoords()[2].set(1, 1);
    rectMesh.getTexCoords()[3].set(-1, 1);
    rectMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    
    ofFbo::Settings settings;
    settings.width = 1024;
    settings.height = 256;
    settings.useStencil = false;
    settings.useDepth = false;
    settings.internalformat = GL_RGBA;
    
    fbo.allocate(settings);
    fbo.getTexture().setTextureMinMagFilter(GL_LINEAR,GL_LINEAR);
    fbo.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    //fbo.getTexture().disableMipmap();
    
    settings.width = 512;
    settings.height = 128;
    
    minFbo.allocate(settings);
    minFbo.getTexture().setTextureMinMagFilter(GL_LINEAR,GL_LINEAR);
    minFbo.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    //minFbo.getTexture().disableMipmap();
    
    resizeNotes();
}

UISequencer::~UISequencer()
{
    sequencer->notesChanged.disconnect<UISequencer, &UISequencer::markNotesDirty>(this);
    shrinkButton->onPointerDown.disconnect<UISequencer, &UISequencer::handleButton>(this);
    for (auto btn : buttons)
    {
        btn->onPointerDown.disconnect<UISequencer, &UISequencer::handleButton>(this);
    }
}

void UISequencer::setFullScreen(bool value, const std::function<void()>& callback, bool immediate)
{
    fullScreen = value;
    
    if (immediate)
    {
        const auto val = fullScreen ? 1.0 : .0f;
        ipExpand = val;
        decreaseTimeMulButton->ipDisplay = val;
        increaseTimeMulButton->ipDisplay = val;
        togglePatternButton->ipDisplay = val;
        clearPatternButton->ipDisplay = val;
    }
    else
    {
        if (fullScreen)
        {
            ipExpand = .0f;
            ipExpand.to(1.0f, .4f, CircEaseOut(), callback);
            decreaseTimeMulButton->show(.2f);
            increaseTimeMulButton->show(.22f);
            togglePatternButton->show(.24f);
            clearPatternButton->show(.26f);
        }
        else
        {
            decreaseTimeMulButton->hide(.06f);
            increaseTimeMulButton->hide(.04f);
            togglePatternButton->hide(.02f);
            clearPatternButton->hide();
            ipExpand.to(.0f, .6f, Linear(), callback);
        }
    }
    onFullScreenChanged.emit();
}

bool UISequencer::isFullScreen() const { return fullScreen; }

void UISequencer::update(float dt)
{
    ipExpand.update(dt);
    origin = (getSize() * .5f) + ofVec2f(0, (getSize().y * .5f - 160.0f) * (1.0f - ipExpand));
    radius = 120.0f + (std::min<float>(getSize().x, getSize().y) * .48f - 120.0f) * ipExpand;
    for(auto& p : patterns)
    {
        p.ipActive.update(dt);
        p.ipDisplay.update(dt);
        p.ipFocus.update(dt);
    }
    if (notesDirty)
    {
        resizeNotes();
        notesDirty = false;
    }
    if (ipExpand > .0f) updateNotes();
    
}

void UISequencer::drawRect(float x, float y, float w, float h)
{
    constexpr float z = 0; // tmp
    rectMesh.getVertices()[0].set(x, y, z);
    rectMesh.getVertices()[1].set(x + w, y, z);
    rectMesh.getVertices()[2].set(x + w, y + h, z);
    rectMesh.getVertices()[3].set(x, y + h, z);
    
    shaderOutline.setUniform2f("rectSize", ofVec2f(w, h));
    
    rectMesh.draw();
}

void UISequencer::draw()
{
    if (ipExpand > .0f)
    {
        auto c = ofColor::black;
        c.a = (.8f * ipExpand) * 255;
        ofSetColor(c);
        ofDrawRectangle(ofVec2f::zero(), getSize().x, getSize().y);
    }
    
    // place buttonsipDisplay
    shrinkButton->setPosition(origin - shrinkButton->getSize() * .5f);
    shrinkButton->ipDisplay = (float)ipExpand;
    auto angle = - PI * .25f;
    for (auto btn : buttons)
    {
        btn->setPosition(origin +
                         ofVec2f(cos(angle), sin(angle)) * radius -
                         btn->getSize() * .5f);
        angle += PI / 10.0f;
    }
    
    //if (radius < .1f) return; // prevent tesselation error;
    
    const auto expandRatio = ipExpand;
    const auto marginRatio = .0f;
    const auto notesRadiusFactor = std::max(.0f, (expandRatio - .5f) * 2.0f);

    //const auto totalRatio = (marginRati8o * (patterns.size() - 1) + patterns.size());
    const auto ratioToSizeFactor = .84f;
    const auto ratioToSize = .8f * ratioToSizeFactor / patterns.size();
    
    auto r = 1.0f - ratioToSizeFactor;
    auto i = -1;
    
    const auto mat = ofGetCurrentViewMatrix();
    //ofLoadIdentityMatrix();
    auto cFbo = expandRatio > 0 ? &fbo : &minFbo;
    const auto C_FBO_WIDTH = cFbo->getWidth();
    const auto C_FBO_HEIGHT = cFbo->getHeight();
    cFbo->begin();
    //ofBackground(ofColor::gray);
    
    ofClear(0, 0, 0, 0);
    
    //ofSetColor(ofColor::red);
    //ofDrawRectangle(20, 20, 245, 45);
    
    float lineWidth = .0f;
    shaderOutline.begin();
    shaderOutline.setUniform1f("bumpFactor", 1.0f - ipExpand * .7f);
    //lineWidth = 3.0f + 6.0f * patterns[i].ipFocus;
    
   // auto c = j % 2 == 0 ? Settings::getMediumColor() : Settings::getLightColor();
    //c.a = (.3f + patterns[i].ipActive * .7f) * 255.0f;
    ofSetColor(Settings::getLightColor());
    
    
    for(auto& p : sequencer->getPatterns())
    {
        ++i;
        
        //const auto margin = .0f;
        constexpr auto sweep = 1.0f;
        const auto sequencerEventRadius = r + ratioToSize * .5f;
        const auto offset = sequencer->isPlaying() ?
            p.timeStampRelativeToBpm:
            (patterns[i].offset * .9f);
        
        patterns[i].offset = offset;
        
        shaderOutline.setUniform1f("lineWidthY", (1.6f + 6.0f * patterns[i].ipFocus) * ipExpand);
        shaderOutline.setUniform1f("lineWidthX", 6.0f); // TODO
        //shaderOutline.setUniform1f("backgroundOpacity",
        //                           (1.0f - ipExpand) * 1.0f + .4f +
        //                           patterns[i].ipActive * .3f);
        
        shaderOutline.setUniform1f("backgroundOpacity", .6f + patterns[i].ipActive * .3f);
        
        auto baseGlobalOpacity = patterns[i].ipActive * .2f + .8f;
        
        auto c = Settings::getMediumColor();
        c.lerp(Settings::getLightColor(), patterns[i].ipFocus);
        ofSetColor(c);
        
        const auto margin = 1.0f - .12f - .12f * (1.0f - ipExpand);// + .16f * ipExpand;
        const auto size = ratioToSize * margin;
        
        
        //for(int j = 0; j < p.bars; ++j)
        {
            const auto start = fmodf(offset, 1.0f);
            
            //shaderOutline.setUniform1f("globalOpacity", baseGlobalOpacity);
           
            patterns[i].minRadius = radius * r;
            patterns[i].maxRadius = radius * (r + ratioToSize * margin);
            
            float remainingSweep = start + sweep - 1.0f;
            
            //auto c = j % 2 == 0 ? Settings::getMediumColor() : Settings::getLightColor();
            //c.a = (.3f + patterns[i].ipActive * .7f) * 255.0f;
            //ofSetColor(c);
            
            //drawRect
            drawRect(C_FBO_WIDTH * start,
                            r * C_FBO_HEIGHT,
                            sweep * C_FBO_WIDTH,
                            size * C_FBO_HEIGHT);
            
            if (remainingSweep > 0)
            {
                //ofDrawRectangle();
                
                drawRect((start - 1.0f) * C_FBO_WIDTH,
                                r * C_FBO_HEIGHT,
                                sweep * C_FBO_WIDTH + lineWidth,
                                size * C_FBO_HEIGHT);
            }
            
        }
        
        r += ratioToSize;
    }
    
    shaderOutline.end();
    
    cFbo->end();
    //fbo.getTexture().generateMipmap();
    //ofLoadMatrix(mat);
    
    ofSetColor(ofColor::white);
    
    //fbo.draw(0, 0);
    
    shaderRadial.begin();
    shaderRadial.setUniformTexture("tex0", cFbo->getTexture(), 0);
    
    ofPushMatrix();
    //ofTranslate(-radius, -radius);
    ofTranslate(origin);

    ofScale(radius, radius);
    //
    //ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_MULTIPLY);
    quad.draw();
    //ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);
    ofPopMatrix();
    
    shaderRadial.end();
    
    // draw notes on fullscreen
    if (ipExpand > 0 && notesVertices.size() > 0)
    {
        ofSetColor(Settings::getLightColor() * .3f);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        notesParticles.draw(GL_TRIANGLES, 0, notesVertices.size());
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }

}

void UISequencer::pointerDownHandler(TouchEvent& touch)
{
    const auto r = (touch.localPosition - origin).length();
    //touch.canceled = r < radius;
    if (ipExpand == 1.0f)
    {
        int i = 0;
        for(auto& p : patterns)
        {
            if (p.minRadius <= r && p.maxRadius >= r)
            {
                sequencer->selectPattern(i);
                touch.canceled = true;
                return;
            }
            ++i;
        }
    } else if (ipExpand == .0f)
    {
        if (r < radius) setFullScreen(true);
    }
}

void UISequencer::handleButton(UIButton* button)
{
    switch(button->getAction())
    {
        case SEQ_SHRINK:
            setFullScreen(false);
            return;
        default: break;
    }
    sequencer->handleUIAction(button->getAction());
}

void UISequencer::syncUI()
{
    const auto selectedPattern = sequencer->getPatterns()[sequencer->getSelectedPattern()];
    increaseTimeMulButton->setPointerEnabled(selectedPattern.timeMul < Sequencer::MAX_TIME_MUL);
    decreaseTimeMulButton->setPointerEnabled(selectedPattern.timeMul > Sequencer::MIN_TIME_MUL);
    togglePatternButton->setState(selectedPattern.active ? 0 : 1);
    int i = 0;
    for(const auto& p : sequencer->getPatterns())
    {
        if (patterns[i].activeFlag != p.active)
        {
            patterns[i].activeFlag = p.active;
            patterns[i].ipActive.to(p.active ? 1.0f : .0f, .2f, Linear());
        }
        const auto focused = (sequencer->getSelectedPattern() == i);
        if (patterns[i].focusFlag != focused)
        {
            patterns[i].focusFlag = focused;
            patterns[i].ipFocus.to(focused ? 1.0f : .0f, .2f, Linear());
        }
        ++i;
    }
}

void UISequencer::resizeNotes()
{
    // compute total notes
    auto totalNotes = 0;
    for (auto& p : sequencer->getPatterns()) totalNotes += p.events.size();
    notesVertices.resize(totalNotes * 3);
    notesParticles.clear();
    notesParticles.setVertexData(&notesVertices[0], notesVertices.size(), GL_DYNAMIC_DRAW);
}

// we need to know the dimensions of the buffer we'll be drawing to
// we could have assumed that it's always "fbo", never "minFbo"
void UISequencer::updateNotes()
{
    const auto forward = ofVec3f(0, 0, 1);
    auto patternIndex = -1;
    auto verticeIndex = -1;
    auto interpolator = CircEaseOut();
    
    for (auto& p : sequencer->getPatterns())
    {
        ++patternIndex;
        
        const auto patternWidth = patterns[patternIndex].maxRadius - patterns[patternIndex].minRadius;
        const auto notesRadius = patterns[patternIndex].minRadius + .5f * patternWidth;
        
        for (auto& e : p.events)
        {
            const auto t = e.timeRelativeToBpm;
            // + patterns[patternIndex].offset, 1.0f);
            const auto dt = fmod(patterns[patternIndex].offset - t + 1.0f, 1.0f);
            constexpr auto interpDur = .2f;
            const auto interpolation = p.active ? interpolator(EasingArgs(std::min<float>(dt, interpDur), 1.0f, -1.0f, interpDur)) : .0f;
            const auto noteHeight = (interpolation * .7f + .3f) * (e.velocity * .5f + .5f) * .38f;
            const auto a = (t * 2.0f - .5f) * PI;
            const auto dir = ofVec3f(cos(a), sin(a), .0f);
            const auto ePos = origin + dir * notesRadius;
            notesVertices[++verticeIndex].set(ePos + dir * patternWidth * noteHeight * .5f);
            notesVertices[++verticeIndex].set(ePos + (dir * patternWidth * .24f).rotateRad(PI * .5f, forward));
            notesVertices[++verticeIndex].set(ePos + dir * patternWidth * noteHeight * -.5f);
        }
    }
    notesParticles.updateVertexData(&notesVertices[0], notesVertices.size());
}

void UISequencer::markNotesDirty() { notesDirty = true; }

