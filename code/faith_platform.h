#ifndef __FAITH_PLATFORM_H__
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else

#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define internal static
#define local_persist static
#define global_variable static

    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;

    typedef int32 bool32;

    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;

    typedef float real32;
    typedef double real64;

#if FAITH_SLOW
#define Assert(expression) \
    if (!(expression))     \
    {                      \
        *(int *)0 = 0;     \
    }
#else
#define Assert(expression)
#endif

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define Kilobytes(bytes) ((bytes)*1024)
#define Megabytes(bytes) (Kilobytes(bytes) * 1024)
#define Gigabytes(bytes) (Megabytes(bytes) * 1024)
#define Terabytes(bytes) (Gigabytes(bytes) * 1024)

#define ToKilobytes(bytes) ((bytes) / 1024)
#define ToMegabytes(bytes) (ToKilobytes(bytes) / 1024)
#define ToGigabytes(bytes) (ToMegabytes(bytes) / 1024)
#define ToTerabytes(bytes) (ToGigabytes(bytes) / 1024)

#define Pi32 3.14159265359f

    inline uint32
    SafeTruncateUInt64(uint64 value)
    {
        Assert(value <= 0xFFFFFFFF);
        uint32 result = (uint32)value;
        return result;
    }

    typedef struct ThreadContext
    {
        int Placeholder;
    } ThreadContext;

#if FAITH_INTERNAL

    typedef struct debug_read_file_result
    {
        uint32 ContentsSize;
        void *Contents;
    } debug_read_file_result;

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(ThreadContext *thread, void *memory)
    typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(ThreadContext *thread, char *filename)
    typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(ThreadContext *thread, char *filename, uint32 memorySize, void *memory)
    typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#endif

    typedef struct GameMemory
    {
        bool32 IsInitialized;

        uint64 PermamentStorageSize;
        void *PermamentStorage;

        uint64 TransientStorageSize;
        void *TransientStorage;

        debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
        debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
        debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
    } GameMemory;

#define GAME_UPDATE(name) void name(ThreadContext *thread, GameMemory *memory)
    typedef GAME_UPDATE(game_update);

#ifdef __cplusplus
}
#endif

#define __FAITH_PLATFORM_H__
#endif
