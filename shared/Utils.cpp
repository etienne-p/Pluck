//
//  Utils.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-24.
//
//

#include "Utils.hpp"

// could run at compile time
// no constexpr as the return type is not literal
std::string Utils::Platform::getShaderPath(std::string path)
{
    #ifdef TARGET_OPENGLES
        return "shaders_gles/" + path;
    #else
        return "shaders/" + path;
    #endif
}

bool Utils::Math::findIntersection(const ofVec2f& p0, const ofVec2f& p1, const ofVec2f& p2, const ofVec2f& p3, ofVec2f& result)
{
    const auto s10_x = p1.x - p0.x;
    const auto s10_y = p1.y - p0.y;
    const auto s32_x = p3.x - p2.x;
    const auto s32_y = p3.y - p2.y;
    const auto denom = s10_x * s32_y - s32_x * s10_y;
    
    if (denom == 0) return false;
    
    bool denom_is_positive = denom > 0;
    
    const auto s02_x = p0.x - p2.x;
    const auto s02_y = p0.y - p2.y;
    const auto s_numer = s10_x * s02_y - s10_y * s02_x;
    
    if ((s_numer < 0) == denom_is_positive) return false;
    
    const auto t_numer = s32_x * s02_y - s32_y * s02_x;
    
    if ((t_numer < 0) == denom_is_positive) return false;
    
    if (((s_numer > denom) == denom_is_positive) || ((t_numer > denom) == denom_is_positive)) return false;
    
    const auto t = t_numer / denom;
    result.set(p0.x + (t * s10_x), p0.y + (t * s10_y));
    return true;
}

float Utils::Math::pointToSegmentDistance(float x, float y, float x1, float y1, float x2, float y2)
{
    const auto A = x - x1;
    const auto B = y - y1;
    const auto C = x2 - x1;
    const auto D = y2 - y1;
    
    const auto dot = A * C + B * D;
    const auto len_sq = C * C + D * D;
    const auto param = dot / len_sq;
    
    float xx, yy;
    
    if (param < 0 || (x1 == x2 && y1 == y2))
    {
        xx = x1;
        yy = y1;
    }
    else if (param > 1)
    {
        xx = x2;
        yy = y2;
    }
    else
    {
        xx = x1 + param * C;
        yy = y1 + param * D;
    }
    
    const auto dx = x - xx;
    const auto dy = y - yy;
    return sqrt(dx * dx + dy * dy);
}

float Utils::Math::pointToSegmentDistance(const ofVec2f& p, const ofVec2f& p1, const ofVec2f& p2)
{
    return pointToSegmentDistance(p.x, p.y, p1.x, p1.y, p2.x, p2.y);
}

Utils::Math::Note Utils::Math::snapToNote(float freq)
{
    // distance to A4 reference in semitones
    int semitones = round((12.0f / log(2.0f)) * log (freq / 440.0f));
    int octave = round(semitones / 12.0f);
    int dSemi = semitones + 9 - octave * 12;
    
    while (dSemi < 0){
        dSemi += 12;
        octave -= 1;
    }
    while (dSemi > 11) {
        dSemi -= 12;
        octave += 1;
    }
    
    Note rv;
    rv.frequency = pow(2.0f, semitones / 12.0f) * 440.0f;
    rv.label = NOTES[dSemi] + ofToString(octave + 4);
    return rv;
}

float Utils::Math::lengthTofreq(float length)
{
    //ofLogNotice(ofToString(length)); //calibration
    auto ease = CircEaseIn();
    auto ratio = 1.0f - ofClamp(length / 76.0f, .0f, 1.0f);
    return 50.0f + 480.0f * ease(EasingArgs(ratio, .0f, 1.0f, 1.0f));
}

bool Utils::Math::annularWedgeContainsPoint(
                                            ofVec2f center,
                                            float minRadius, float maxRadius,
                                            float startAngle, float sweepAngle,
                                            ofVec2f point)
{
    const auto d = point - center;
    const auto dl = d.length();
    if (dl >= minRadius && dl <= maxRadius)
    {
        auto aa = std::fmod(startAngle, 2.0f * PI);
        if (aa > PI) aa -= 2.0f * PI;
        else if (aa < -PI) aa += 2.0f * PI;
        
        auto ab = std::fmod(startAngle + sweepAngle, 2.0f * PI);
        if (ab > PI) ab -= 2.0f * PI;
        else if (ab < -PI) ab += 2.0f * PI;

        const auto a = atan2(d.y, d.x);
        if (a >= std::min<float>(aa, ab) && a <= std::max<float>(aa, ab)) return true;
        return false;
    }
    return false;
}

ofVec3f Utils::Math::getNormal(vector<ofVec3f> pverts)
{
    ofVec3f t0 = pverts.at(1)-pverts.at(0);
    ofVec3f t1 = pverts.at(2)-pverts.at(0);
    ofVec3f normal = t0.getCrossed(t1);
    normal.normalize();
    return  normal;
}

// https://github.com/Akira-Hayasaka/ofxRayTriangleIntersection/blob/master/src/ofxRayTriangleIntersection.h
Utils::Math::IntersectInfo Utils::Math::intersectRayTri(ofVec3f rayStart, ofVec3f rayDir, vector<ofVec3f> tri)
{
    IntersectInfo result;
    
    rayDir.normalize();
    
    ofVec3f triNorm = getNormal(tri);
    float vn = rayDir.dot(triNorm);
    
    ofVec3f aa = rayStart - tri.at(0);
    float xpn = aa.dot(triNorm);
    float distance = -xpn / vn;
    
    if (distance < 0) return result; // behind ray origin. fail
    
    ofPoint hitPos(rayDir.x * distance + rayStart.x,
                   rayDir.y * distance + rayStart.y,
                   rayDir.z * distance + rayStart.z);
    
    ofVec3f hit00 = hitPos - tri.at(0);
    ofVec3f hit01 = tri.at(1) - tri.at(0);
    ofVec3f cross0 = hit00.cross(hit01);
    if (cross0.dot(triNorm) > 0.000000001) return result;; // not in tri. fail
    
    ofVec3f hit10 = hitPos - tri.at(1);
    ofVec3f hit11 = tri.at(2) - tri.at(1);
    ofVec3f cross1 = hit10.cross(hit11);
    if (cross1.dot(triNorm) > 0.000000001) return result;; // not in tri. fail
    
    ofVec3f hit20 = hitPos - tri.at(2);
    ofVec3f hit21 = tri.at(0) - tri.at(2);
    ofVec3f cross2 = hit20.cross(hit21);
    if (cross2.dot(triNorm) > 0.000000001) return result;; // not in tri. fail
    
    // intersect!
    result.bIntersect = true;
    result.intersectPos = hitPos;
    result.distance = distance;
    return result;
}


