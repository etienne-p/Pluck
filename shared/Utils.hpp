//
//  Utils.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-24.
//
//

#pragma once

#include "ofMain.h"
#include <array>
#include "Easing.h"

namespace Utils
{
    
namespace Platform
{
    std::string getShaderPath(std::string path);
}

namespace Math
{
    bool findIntersection(const ofVec2f& p0, const ofVec2f& p1, const ofVec2f& p2, const ofVec2f& p3, ofVec2f& result);
    
    float pointToSegmentDistance(float x, float y, float x1, float y1, float x2, float y2);
    
    float pointToSegmentDistance(const ofVec2f& p,  const ofVec2f& p1, const ofVec2f& p2);
    
    const std::array<string, 12> NOTES = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
    
    struct Note { string label; float frequency; };

    Note snapToNote(float frequency);
    
    float lengthTofreq(float length);
    
    bool annularWedgeContainsPoint(ofVec2f center,
                                   float minRadius, float maxRadius,
                                   float startAngle, float sweepAngle,
                                   ofVec2f point);
    
    // https://github.com/Akira-Hayasaka/ofxRayTriangleIntersection/blob/master/src/ofxRayTriangleIntersection.h
    class IntersectInfo
    {
    public:
        IntersectInfo()
        {
            bIntersect = false;
        }
        
        bool bIntersect;
        ofPoint intersectPos;
        float distance;
    };
    
    ofVec3f getNormal(vector<ofVec3f> pverts);

    IntersectInfo intersectRayTri(ofVec3f rayStart, ofVec3f rayDir, vector<ofVec3f> tri);

}

}