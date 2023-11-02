//
//  ImageAssets.hpp
//  Harp2
//
//  Created by etienne cella on 2015-12-21.
//
//

#pragma once

#include <map>
#include <memory>
#include "ofxThreadedImageLoader.h"

class ImageAssets
{
public:
    
    enum Icon
    {
        ADD,
        SUB,
        CLOSE,
        CONTROLS,
        DOWNLOAD,
        DUMP,
        NOTE,
        PAUSE,
        PEN,
        PLAY,
        SETTINGS,
        VOLUME_ON,
        VOLUME_OFF,
        OPEN,
        SAVE,
        CHECK,
        METRO,
        TOUCH
    };
    
    ImageAssets()
    {
        for(auto it = ICONS_PATHS.begin(); it != ICONS_PATHS.end(); it++)
        {
            auto img = std::make_shared<ofImage>();
            img->getTexture().enableMipmap();
            icons.insert(std::make_pair(it->first, img));
            //icons[it->first]->getTexture().enableMipmap();
            loader.loadFromDisk(*icons[it->first], "icons/" + it->second + ".png");
        }
    }
    
    ~ImageAssets() { loader.stopThread(); }
    
    bool isComplete()
    {
        for(auto it = icons.begin(); it != icons.end(); it++)
        {
            if (!it->second->isAllocated() || it->second->getWidth() == 0)
                return false;
        }
        return true;
    }
    
    std::shared_ptr<ofImage> getIcon(Icon icon)
    {
        return icons.find(icon)->second;
    }
    
private:
    
    ofxThreadedImageLoader loader;
    std::map<ImageAssets::Icon, std::shared_ptr<ofImage>> icons;
    static const std::map<ImageAssets::Icon, string> ICONS_PATHS;
};



