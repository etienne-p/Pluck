//
//  UIRopeControls.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-07.
//
//

#pragma once

#include "UIBaseObject.hpp"
#include "UIMenu.hpp"

class UIRopeControls : public UIBaseObject
{
    UIRopeControls();
    
    std::vector<std::unique_ptr<UIBaseObject>> uiObjects; // for ownership
    
};
