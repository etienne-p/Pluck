//
//  Fonts.cpp
//  Harp2
//
//  Created by etienne cella on 2015-11-21.
//
//

#include "Fonts.hpp"

ofTrueTypeFont Fonts::ELGETHY_30;

ofTrueTypeFont Fonts::ELGETHY_72;

void Fonts::init()
{
    ofTrueTypeFont::setGlobalDpi(72);
    
    ELGETHY_30.load("Moon_Bold.ttf", 30, true, true);
    ELGETHY_30.setLineHeight(34.0f);
    ELGETHY_30.setLetterSpacing(1.035);
    
    ELGETHY_72.load("Moon_Bold.ttf", 52, true, true);
    //VERDANA_72.setLineHeight(80.0f);
    //VERDANA_72.setLetterSpacing(1.035);
}

ofTrueTypeFont* Fonts::getFontH1()
{
    return &ELGETHY_72;
}

ofTrueTypeFont* Fonts::getFontP()
{
    return &ELGETHY_30;
}

