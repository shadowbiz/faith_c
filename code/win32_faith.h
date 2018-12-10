#ifndef __WIN32_FAITH_H__
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#include "faith_string.h"

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

struct Win32GameCode
{
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;
    game_update *UpdateAndRender;

    bool32 IsValid;
};

struct Win32State
{
    uint64 TotalSize;
    void *GameMemoryBlock;

    HANDLE RecordingHandle;
    int InputRecordingIndex;

    HANDLE PlaybackHandle;
    int InputPlayingIndex;

    char EXEFileName[WIN32_STATE_FILE_NAME_COUNT];
    char *OnePastLastEXEFileNameSlash;
};

#define __WIN32_FAITH_H__
#endif
