//
//  Layout.cpp
//  Harp2
//
//  Created by etienne cella on 2015-12-14.
//
//

#include "Layout.hpp"

void Layout::updateLayoutSize()
{
    if (!sizeDirty) return;
    
    // recursion updating childs first
    auto it0 = getForwardChildIterator<Layout>();
    while(!it0.isDone())
    {
        if (it0->isLayoutSizeDirty())
        {
            it0->updateLayoutSize();
        }
        ++it0;
    }
    
    preferredTotalSize.set(.0f, .0f);
    flexibleTotalSize.set(.0f, .0f);
    
    auto it1 = getForwardChildIterator<Layout>();
    while(!it1.isDone())
    {
        flexibleTotalSize += it1->flexibleSize;
        
        switch(type)
        {
            case DEFAULT: break;
            
            case HORIZONTAL:
                preferredTotalSize.x += it1->preferredSize.x;
                preferredTotalSize.y = std::max<float>(it1->preferredSize.y, preferredTotalSize.y);
                break;
            
            case VERTICAL:
                preferredTotalSize.y += it1->preferredSize.y;
                preferredTotalSize.x = std::max<float>(it1->preferredSize.x, preferredTotalSize.x);
                break;
        }
        ++it1;
    }
    
    flexibleTotalSize.x = std::max<float>(1.0f, flexibleTotalSize.x);
    flexibleTotalSize.y = std::max<float>(1.0f, flexibleTotalSize.y);
    
    if (fitContent.horizontal) preferredSize.x = preferredTotalSize.x;
        
    if (fitContent.vertical) preferredSize.y = preferredTotalSize.y;
    
    sizeDirty = false;
}

void Layout::updateLayoutPosition(bool force)
{
    if (positionDirty || force)
    {
        // fit depending on layout type
        switch(type)
        {
                // default: just an anchored element updating its position according to its anchor
                // and parent size
            case NONE: break;
                
            case DEFAULT: resizeDefaultLayout(this); break;
                
            case HORIZONTAL:
            {
                const auto bounds = resizeHorizontalLayoutGroupChilds(this);
                placeHorizontalLayoutGroupChilds(this, bounds);
            }
                break;
                
            case VERTICAL:
            {
                const auto bounds = resizeVerticalLayoutGroupChilds(this);
                placeVerticalLayoutGroupChilds(this, bounds);
            }
                break;
        }
        
        positionDirty = false;
    }
    
    // resursion updating childs later
    auto it = getForwardChildIterator<Layout>();
    while(!it.isDone())
    {
        it->updateLayoutPosition(force);
        ++it;
    }
}

ofVec2f Layout::getPreferredTotalSize() const noexcept { return preferredTotalSize; }
ofVec2f Layout::getFlexibleTotalSize() const noexcept { return flexibleTotalSize; }

ofVec2f Layout::getPreferredSize() const noexcept { return preferredSize; }
ofVec2f Layout::getFlexibleSize() const noexcept { return flexibleSize; }

Layout::Padding Layout::getPadding() const noexcept { return padding; }
bool Layout::getPreserveSizeRatio() const noexcept { return preserveSizeRatio; }
Layout::ContentFit Layout::getFitContent() const noexcept { return fitContent; }
float Layout::getSpaceBetweenChilds() const noexcept { return spaceBetweenChilds; }

Layout::Align Layout::getAlign() const noexcept { return align; }

Layout::LayoutType Layout::getLayoutType() const noexcept { return type; }

Layout::Anchor Layout::getAnchor() const noexcept { return anchor; }

ofVec2f Layout::getAnchoredPosition() const noexcept { return anchoredPosition; }

void Layout::setPreferredSize(const ofVec2f& v) { preferredSize = v; setLayoutSizeDirty(); }

void Layout::setFlexibleSize(const ofVec2f& v) { flexibleSize = v; setLayoutSizeDirty(); }

void Layout::setPadding(const Padding& v) { padding = v; setLayoutSizeDirty(); }

void Layout::setPreserveSizeRatio(bool v) { preserveSizeRatio = v; setLayoutSizeDirty(); }

void Layout::setSpaceBetweenChilds(float v) { spaceBetweenChilds = v; setLayoutSizeDirty(); }

void Layout::setAlign(HorizontalAlign h, VerticalAlign v)
{
    Align a;
    a.horizontal = h;
    a.vertical = v;
    align = a;
    setLayoutSizeDirty();
}

void Layout::setFitContent(bool horizontal, bool vertical)
{
    fitContent.horizontal = horizontal;
    fitContent.vertical = vertical;
    setLayoutSizeDirty();
}

bool Layout::isLayoutSizeDirty() const noexcept { return sizeDirty; }

bool Layout::isLayoutPositionDirty() const noexcept { return positionDirty; }

void Layout::setLayoutType(LayoutType t) { type = t; setLayoutSizeDirty(); }

void Layout::setAnchor(HorizontalAlign h, VerticalAlign v)
{
    Anchor a;
    a.horizontal = h;
    a.vertical = v;
    anchor = a;
    setLayoutPositionDirty();
}

void Layout::setAnchoredPosition(const ofVec2f& p) { anchoredPosition = p; } // should anchoredPosition update mark anything dirty ?

void Layout::setLayoutSizeDirty(bool force)
{
    sizeDirty = true;
    Layout* parent = dynamic_cast<Layout*>(getParent());
    // propagate up the tree as long as containers fit their children
    if (parent != nullptr && (force || parent->getFitContent())) { parent->setLayoutSizeDirty(); }
}

void Layout::setLayoutPositionDirty()
{
    //positionDirty = true; // note that fitDirty does not bubble up
    Layout* parent = dynamic_cast<Layout*>(getParent());
    if (parent != nullptr) parent->positionDirty = true;
}

void Layout::setLayoutDirty()
{
    setLayoutSizeDirty(true);
    setLayoutPositionDirty();
    //return;
    traverse([](Transform2D* t)
    {
        const auto l = dynamic_cast<Layout*>(t);
        l->positionDirty = true;
        //l->sizeDirty = true;
        return true;
    });
}

ofVec2f Layout::resizeHorizontalLayoutGroupChilds(Layout* layout)
{
    const auto rWidth = layout->getSize().x - layout->getPadding().right - layout->getPadding().left -
    (layout->getNumChildren() * layout->getSpaceBetweenChilds());
    const auto rHeight = layout->getSize().y - layout->getPadding().top - layout->getPadding().bottom;
    
    // we don't scale above preferred size
    const ofVec2f scaleRatio
    {
        std::min<float>(1.0, rWidth / std::max<float>(1.0, layout->getPreferredTotalSize().x)),
        std::min<float>(1.0, rHeight / std::max<float>(1.0, layout->getPreferredTotalSize().y))
    };
    
    const auto uniformScaleRatio = std::min<float>(scaleRatio.x, scaleRatio.y);
    const auto availableSpace = std::max<float>(.0f, rWidth - layout->getPreferredTotalSize().x * scaleRatio.x);
    // compute childs sizes
    ofVec2f childrenBounds;
    {
        auto it = layout->getForwardChildIterator<Layout>();
        while(!it.isDone())
        {
            ofVec2f size;
            if (it->getPreserveSizeRatio())
            {
                size.set(it->getPreferredSize() * uniformScaleRatio);
            }
            else
            {
                const ofVec2f scaledSize
                {
                    it->getPreferredSize().x * scaleRatio.x,
                    it->getPreferredSize().y * scaleRatio.y
                };
                const ofVec2f fillSize
                {
                    availableSpace * it->getFlexibleSize().x / layout->getFlexibleTotalSize().x,
                    (rHeight - scaledSize.y) * it->getFlexibleSize().y
                    // in the case of an horizontal layout, vertical space is not shared
                };
                size = scaledSize + fillSize;
            }
            it->setSize(size);
            size = it->getSize();
            childrenBounds.x += size.x;
            childrenBounds.y = std::max<float>(childrenBounds.y, size.y);
            ++it;
        }
    }
    childrenBounds.x += layout->getSpaceBetweenChilds() * (layout->getNumChildren() - 1);
    return childrenBounds;
}

ofVec2f Layout::resizeVerticalLayoutGroupChilds(Layout* layout)
{
    const auto rWidth = layout->getSize().x - layout->getPadding().right - layout->getPadding().left;
    const auto rHeight = layout->getSize().y - layout->getPadding().top - layout->getPadding().bottom - (layout->getNumChildren() * layout->getSpaceBetweenChilds());
    
    // we don't scale above preferred size
    const ofVec2f scaleRatio
    {
        std::min<float>(1.0, rWidth / std::max<float>(1.0, layout->getPreferredTotalSize().x)),
        std::min<float>(1.0, rHeight / std::max<float>(1.0, layout->getPreferredTotalSize().y))
    };
    
    const auto uniformScaleRatio = std::min<float>(scaleRatio.x, scaleRatio.y);
    const auto availableSpace = std::max<float>(.0f, rHeight - layout->getPreferredTotalSize().y * scaleRatio.y);
    
    // compute childs sizes
    ofVec2f childrenBounds;
    {
        auto it = layout->getForwardChildIterator<Layout>();
        while(!it.isDone())
        {
            ofVec2f size;
            if (it->getPreserveSizeRatio())
            {
                size.set(it->getPreferredSize() * uniformScaleRatio);
            }
            else
            {
                const ofVec2f scaledSize
                {
                    it->getPreferredSize().x * scaleRatio.x,
                    it->getPreferredSize().y * scaleRatio.y
                };
                const ofVec2f fillSize
                {
                    (rWidth - scaledSize.x) * it->getFlexibleSize().x,
                    availableSpace * it->getFlexibleSize().y / layout->getFlexibleTotalSize().y
                };
                size = scaledSize + fillSize;
            }
            it->setSize(size);
            size = it->getSize();
            childrenBounds.y += size.y;
            childrenBounds.x = std::max<float>(childrenBounds.x, size.x);
            ++it;
        }
    }
    childrenBounds.y += layout->getSpaceBetweenChilds() * (layout->getNumChildren() - 1);
    return childrenBounds;
}

void Layout::placeHorizontalLayoutGroupChilds(Layout* layout, const ofVec2f& childrenBounds)
{
    float cx = .0f;
    
    switch(layout->getAlign().horizontal)
    {
        case Layout::HorizontalAlign::LEFT:
            cx = layout->getPadding().left;
            break;
            
        case Layout::HorizontalAlign::CENTER:
            cx = layout->getPadding().left + (layout->getSize().x -
                                              layout->getPadding().left - layout->getPadding().right - childrenBounds.x) * .5f;
            break;
            
        case Layout::HorizontalAlign::RIGHT:
            cx = layout->getSize().x - layout->getPadding().right - childrenBounds.x;
            break;
    };
    
    auto it = layout->getForwardChildIterator<Layout>();
    
    while(!it.isDone())
    {
        float y = .0f;
        
        switch(layout->getAlign().vertical)
        {
            case Layout::VerticalAlign::TOP:
                y = layout->getPadding().top;
                break;
                
            case Layout::VerticalAlign::MIDDLE:
                y = layout->getPadding().top + (layout->getSize().y -
                                                layout->getPadding().top - layout->getPadding().bottom - it->getSize().y) * .5f;
                break;
                
            case Layout::VerticalAlign::BOTTOM:
                y = layout->getSize().y - layout->getPadding().bottom - it->getSize().y;
                break;
        };
        
        it->setPosition(ofVec2f(cx, y));
        cx += it->getSize().x + layout->getSpaceBetweenChilds();
        ++it;
    }
}

void Layout::placeVerticalLayoutGroupChilds(Layout* layout, const ofVec2f& childrenBounds)
{
    float cy = .0f;
    
    switch(layout->getAlign().vertical)
    {
        case Layout::VerticalAlign::TOP:
            cy = layout->getPadding().top;
            break;
            
        case Layout::VerticalAlign::MIDDLE:
            cy = layout->getPadding().top + (layout->getSize().y -
                                             layout->getPadding().top - layout->getPadding().bottom - childrenBounds.y) * .5f;
            break;
            
        case Layout::VerticalAlign::BOTTOM:
            cy = layout->getSize().y - layout->getPadding().bottom - childrenBounds.y;
            break;
    };
    
    auto it = layout->getForwardChildIterator<Layout>();
    
    while(!it.isDone())
    {
        float x = .0f;
        
        switch(layout->getAlign().horizontal)
        {
            case Layout::HorizontalAlign::LEFT:
                x = layout->getPadding().left;
                break;
                
            case Layout::HorizontalAlign::CENTER:
                x = layout->getPadding().left + (layout->getSize().x -
                                                 layout->getPadding().left - layout->getPadding().right - it->getSize().x) * .5f;
                break;
                
            case Layout::HorizontalAlign::RIGHT:
                x = layout->getSize().x - layout->getPadding().right - it->getSize().x;
                break;
        };
        
        it->setPosition(ofVec2f(x, cy));
        cy += it->getSize().y + layout->getSpaceBetweenChilds();
        ++it;
    }
}

void Layout::resizeDefaultLayout(Layout* layout)
{
    auto it = layout->getForwardChildIterator<Layout>();
    while(!it.isDone())
    {
        // update size
        it->setSize(ofVec2f(it->getPreferredSize().x + layout->getSize().x * it->getFlexibleSize().x,
                            it->getPreferredSize().y + layout->getSize().y * it->getFlexibleSize().y));
        // update position
        ofVec2f position;
        
        switch(it->getAnchor().horizontal)
        {
            case LEFT:   position.x = it->getAnchoredPosition().x; break;
            case CENTER: position.x = it->getAnchoredPosition().x + layout->getSize().x * .5f; break;
            case RIGHT:  position.x = it->getAnchoredPosition().x + layout->getSize().x; break;
        }
        
        switch(it->getAnchor().vertical)
        {
            case TOP:    position.y = it->getAnchoredPosition().y; break;
            case MIDDLE: position.y = it->getAnchoredPosition().y + layout->getSize().y * .5f; break;
            case BOTTOM: position.y = it->getAnchoredPosition().y + layout->getSize().y; break;
        }
        it->setPosition(position);
        ++it;
    }
}
