//
//  Graphics.hpp
//  Harp2
//
//  Created by etienne cella on 2015-11-21.
//
//

#pragma once

#include <algorithm>
#include "ofMain.h"

class Graphics
{

public:
    
static void drawAnnularWedge(const ofVec2f& center,
                             float innerRadius,
                             float outerRadius,
                             float startAngle,
                             float angleSweep);
    
};