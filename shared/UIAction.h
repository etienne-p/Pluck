//
//  UIAction.h
//  Harp2
//
//  Created by etienne cella on 2015-12-12.
//
//

#ifndef UIAction_h
#define UIAction_h

enum UIAction
{
    EXIT,
    SAVE,
    KEYBOARD,
    VALIDATE,
    DELETE,
    
    MODE_EDIT,
    MODE_PLAY,
    MODE_DELETE,
    MODE_SETTINGS,
    
    SEQ_PLAY_PAUSE,
    SEQ_SHRINK,
    SEQ_RECORD,
    SEQ_SEQ,
    SEQ_METRO,

    PATTERN_DECREASE_TIME_MUL,
    PATTERN_INCREASE_TIME_MUL,
    PATTERN_CLEAR,
    PATTERN_ACTIVE,
    
    SCENE_OPEN,
    SCENE_DELETE,
    SCENE_SAVE
};


#endif /* UIAction_h */
