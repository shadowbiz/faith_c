#ifndef __FAITH_H__
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#include "faith_platform.h"

struct MemoryArena
{
    size_t Size;
    size_t Used;
    uint8* Base;
};

#define PushSize(Arena, type) (type*) PushSize_(Arena, sizeof(type))
#define PushArray(Arena, count, type) (type*) PushSize_(Arena, count * sizeof(type))
inline void*
PushSize_(MemoryArena *arena, size_t size)
{
    Assert((arena->Used + size) < arena->Size);

    void* result = arena->Base + arena->Used;
    arena->Used += size;
    return result;
}
#define ZeroStruct(Instance) ZeroSize(sizeof(Instance), &(Instance))
inline void
ZeroSize(size_t Size, void *Ptr)
{
    // TODO: Check this guy for performance
    uint8 *Byte = (uint8 *)Ptr;
    while(Size--)
    {
        *Byte++ = 0;
    }
}

#include "faith_intinsics.h"
#include "faith_world.h"

struct GameState
{
    MemoryArena WorldArena;
    World *World;
};

#define __FAITH_H__
#endif
