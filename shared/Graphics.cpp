//
//  Graphics.cpp
//  Harp2
//
//  Created by etienne cella on 2015-11-21.
//
//

#include "Graphics.hpp"

void Graphics::drawAnnularWedge(const ofVec2f& center,
                                float innerRadius,
                                float outerRadius,
                                float startAngle,
                                float angleSweep)
{
    // TODO use a mesh
    
    const int resolution = 22;//std::max<int>(4, 12 * angleSweep);
    
    ofBeginShape();
    
    auto i = 0;
    for (; i < resolution; ++i)
    {
        const auto a = startAngle + angleSweep * ((float)i / (float)(resolution - 1));
        ofVertex( cos(a) * outerRadius + center.x, sin(a) * outerRadius + center.y);
    }
    
    for (i = resolution - 1; i > -1; --i)
    {
        const auto a = startAngle + angleSweep * ((float)i / (float)(resolution - 1));
        ofVertex( cos(a) * innerRadius + center.x, sin(a) * innerRadius + center.y);
    }
    
    ofEndShape();
}