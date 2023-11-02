//
//  Fonts.hpp
//  Harp2
//
//  Created by etienne cella on 2015-11-21.
//
//

#pragma once

#include "ofMain.h"

class Fonts
{
    
public:
    
    static ofTrueTypeFont* getFontH1();
    static ofTrueTypeFont* getFontP();
    static void init();

private:
    
    static ofTrueTypeFont ELGETHY_30;
    static ofTrueTypeFont ELGETHY_72;
    
    
};