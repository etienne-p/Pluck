//
//  UIBaseObject.hpp
//  Harp2
//
//  Created by Etienne on 2015-12-01.
//
//

#pragma once

#include "Layout.hpp"

class UIBaseObject : public Layout
{
public:
    
    bool visible{true};
    
    UIBaseObject()
    {
        //layout.setDefault(this);
    }
    
    virtual void draw(){};
    virtual void pause() {};
    virtual void resume() {};
    virtual void update(float dt){};
    /*
    virtual void resize(float width_, float height_)
    {
        setSize(ofVec2f(width_, height_));
    }*/ // default behavior
};
