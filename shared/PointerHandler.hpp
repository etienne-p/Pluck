//
//  InteractiveObject2D.hpp
//  Harp2
//
//  Created by etienne cella on 2015-11-19.
//
//

#pragma once

#include <memory>
#include "Transform2D.hpp"

struct TouchEvent
{
    int id;
    bool canceled{false};
    //size_t minDepth{0};
    ofTouchEventArgs::Type type;
    ofVec2f globalPosition;
    ofVec2f localPosition;
    
    static TouchEvent convertEvent(ofTouchEventArgs& touch)
    {
        TouchEvent evt;
        evt.globalPosition.set(touch.x, touch.y);
        evt.localPosition.set(touch.x, touch.y);
        evt.type = touch.type;
        evt.id = touch.id;
        return evt;
    }
};


class PointerHandler : public Transform2D
{
public:
    
    //bool captureEvents{false};
    
    bool inside(const ofVec2f& input) const
    {
        return  (input.x >= 0 && input.x <= getSize().x && input.y >= 0 && input.y <= getSize().y);
    }
    virtual void pointerDownHandler(TouchEvent& touch){};
    virtual void pointerMovedHandler(TouchEvent& touch){};
    virtual void pointerUpHandler(TouchEvent& touch){};
    
    bool isPointerEnabled() const
    {
        return pointerEnabled;
    }
    
    virtual bool setPointerEnabled(bool v, bool recursize = false)
    {
        pointerEnabled = v;
        if (recursize)
        {
            traverse([v](Transform2D* t) // pre post order doesnt matter here
                                 {
                                     static_cast<PointerHandler*>(t)->setPointerEnabled(v);
                                     return true;
                                 });
        }
    }
    
private:
    bool pointerEnabled{true};
    
};