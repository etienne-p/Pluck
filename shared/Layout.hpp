//
//  Layout.hpp
//  Harp2
//
//  Created by etienne cella on 2015-12-14.
//
//

#pragma once

#include <algorithm>
#include "PointerHandler.hpp"

class Layout : public PointerHandler
{
public:
    
    struct Padding
    {
        float top{.0f};
        float left{.0f};
        float right{.0f};
        float bottom{.0f};
        
        Padding(float top_, float left_, float bottom_, float right_) :
        top(top_), left(left_), bottom(bottom_), right(right_) {}
        
        Padding(){}
    };
    
    enum LayoutType { NONE, DEFAULT, HORIZONTAL, VERTICAL };
    
    enum HorizontalAlign { LEFT, RIGHT, CENTER };
    
    enum VerticalAlign { TOP, BOTTOM, MIDDLE };
    
    struct Align
    {
        HorizontalAlign horizontal{LEFT};
        VerticalAlign vertical{TOP};
    };
    
    struct Anchor
    {
        HorizontalAlign horizontal{LEFT};
        VerticalAlign vertical{TOP};
    };
    
    struct ContentFit
    {
        bool horizontal{false};
        bool vertical{false};
        operator bool() const { return horizontal || vertical; }
    };
    
    void updateLayoutSize();
    void updateLayoutPosition(bool force = false);
    
    ofVec2f getPreferredTotalSize() const noexcept;
    ofVec2f getFlexibleTotalSize() const noexcept;
    
    ofVec2f getPreferredSize() const noexcept;
    ofVec2f getFlexibleSize() const noexcept;
    
    Padding getPadding() const noexcept;
    bool getPreserveSizeRatio() const noexcept;
    ContentFit getFitContent() const noexcept;
    float getSpaceBetweenChilds() const noexcept;
    Align getAlign() const noexcept;
    LayoutType getLayoutType() const noexcept;
    Anchor getAnchor() const noexcept;
    ofVec2f getAnchoredPosition() const noexcept;
    bool isLayoutSizeDirty() const noexcept;
    bool isLayoutPositionDirty() const noexcept;
    
    void setPreferredSize(const ofVec2f& v);
    void setFlexibleSize(const ofVec2f& v);
    void setPadding(const Padding& v);
    void setPreserveSizeRatio(bool v);
    void setSpaceBetweenChilds(float v);
    void setAlign(HorizontalAlign h, VerticalAlign v);
    void setFitContent(bool horizontal, bool vertical);
    void setLayoutType(LayoutType t);
    void setAnchor(HorizontalAlign h, VerticalAlign v);
    void setAnchoredPosition(const ofVec2f& p);
    void setLayoutDirty();
    
protected:
    
    void setLayoutSizeDirty(bool force = false);
    void setLayoutPositionDirty();
private:
    
    ofVec2f preferredTotalSize{.0f, .0f};
    ofVec2f flexibleTotalSize{.0f, .0f};
    ofVec2f anchoredPosition;
    
    Padding padding;
    Anchor anchor;
    Align align;
    bool sizeDirty{true};
    bool positionDirty{true};
    bool preserveSizeRatio{false}; // could be deduced from fillSize?
    ContentFit fitContent; // could be deduced from fillSize?
    LayoutType type{DEFAULT};

    //bool fitContent{true};
    ofVec2f preferredSize; // ~= maxSize, used by layouts to compute space allocated to childs
    ofVec2f flexibleSize; // how much do we fill extra available layout place
    float spaceBetweenChilds{.0f};// claim a share of extra space

    // Utils
    static ofVec2f resizeHorizontalLayoutGroupChilds(Layout* layout);
    static ofVec2f resizeVerticalLayoutGroupChilds(Layout* layout);
    static void placeHorizontalLayoutGroupChilds(Layout* layout, const ofVec2f& childrenBounds);
    static void placeVerticalLayoutGroupChilds(Layout* layout, const ofVec2f& childrenBounds);
    static void resizeDefaultLayout(Layout* layout);
};