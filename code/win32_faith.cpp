
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Udov $
   $Notice: (C) Copyright 2015 by Shadowbiz, Inc. All Rights Reserved. $
   ======================================================================== */

#include "faith_platform.h"

#include <windows.h>
#include <stdio.h>
#include <malloc.h>

#include "faith_string.cpp"
#include "win32_faith.h"

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
    debug_read_file_result result = {};

    HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            uint32 fileSize32 = SafeTruncateUInt64(fileSize.QuadPart);
            result.Contents = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result.Contents)
            {
                DWORD bytesRead;
                if (ReadFile(fileHandle, result.Contents, fileSize32, &bytesRead, 0) &&
                    (fileSize32 == bytesRead))
                {
                    result.ContentsSize = fileSize32;
                }
                else
                {
                    // TODO: Log
                    DEBUGPlatformFreeFileMemory(thread, result.Contents);
                    result.Contents = 0;
                }
            }
            else
            {
                // TODO: Log
            }
        }
        else
        {
            // TODO: Log
        }
        CloseHandle(fileHandle);
    }
    else
    {
        // TODO: Log
    }
    return result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
    bool32 result = false;

    HANDLE fileHandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0))
        {
            result = (bytesWritten == memorySize);
        }
        else
        {
            // TODO: Log
        }

        CloseHandle(fileHandle);
    }
    else
    {
        // TODO: Log
    }

    return result;
}

inline FILETIME
Win32GetLastWriteTime(char *filename)
{
    FILETIME lastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    {
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
}

internal void
Win32GetEXEFileName(Win32State *state)
{
    DWORD sizeOfFilename = GetModuleFileNameA(0, state->EXEFileName, sizeof(state->EXEFileName));
    state->OnePastLastEXEFileNameSlash = state->EXEFileName;
    for (char *scan = state->EXEFileName; *scan; ++scan)
    {
        if (*scan == '\\')
        {
            state->OnePastLastEXEFileNameSlash = scan + 1;
        }
    }
}

internal Win32GameCode
Win32LoadGameCode(char *sourceDLLName, char *tempDLLName)
{
    Win32GameCode result = {};

    result.DLLLastWriteTime = Win32GetLastWriteTime(sourceDLLName);
    CopyFile(sourceDLLName, tempDLLName, FALSE);

    result.GameCodeDLL = LoadLibraryA(tempDLLName);
    if (result.GameCodeDLL)
    {
        result.UpdateAndRender = (game_update *)GetProcAddress(result.GameCodeDLL, "GameUpdate");
        result.IsValid = (result.UpdateAndRender != 0);
    }

    if (!result.IsValid)
    {
        result.UpdateAndRender = 0;
    }

    return result;
}

internal void
Win32BuildEXEPathFileName(Win32State *state, char *fileName,
                          int destCount, char *dest)
{
    StringsCat(state->OnePastLastEXEFileNameSlash - state->EXEFileName, state->EXEFileName,
               StringLength(fileName), fileName,
               destCount, dest);
}

internal void
Win32UnloadGameCode(Win32GameCode *gameCode)
{
    if (gameCode->GameCodeDLL)
    {
        FreeLibrary(gameCode->GameCodeDLL);
        gameCode->GameCodeDLL = 0;
    }

    gameCode->IsValid = false;
    gameCode->UpdateAndRender = 0;
}

struct Win32ThreadInfo
{
    int LogicalThreadIndex;
};

int main(int argc, char **argv)
{
    Win32State win32State = {};

    Win32GetEXEFileName(&win32State);

    char sourceGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&win32State, "faith.dll",
                              sizeof(sourceGameCodeDLLFullPath), sourceGameCodeDLLFullPath);

    char tempGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&win32State, "faith_temp.dll",
                              sizeof(tempGameCodeDLLFullPath), tempGameCodeDLLFullPath);

#if FAITH_INTERNAL
    LPVOID baseAddress = (LPVOID)Terabytes((uint64)2);
#else
    LPVOID baseAddress = 0;
#endif

    GameMemory gameMemory = {};
    gameMemory.PermamentStorageSize = Megabytes(8);
    gameMemory.TransientStorageSize = Megabytes(8);
    gameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
    gameMemory.DEBUGPlatformReadEntireFile = DEBUGPlatformReadEntireFile;
    gameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;

    uint64 totalMemorySize = gameMemory.PermamentStorageSize + gameMemory.TransientStorageSize;

    gameMemory.PermamentStorage =
        VirtualAlloc(baseAddress, totalMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    gameMemory.TransientStorage =
        ((uint8 *)gameMemory.PermamentStorage + gameMemory.PermamentStorageSize);

    ThreadContext threadContext = {};

    Win32GameCode game = Win32LoadGameCode(sourceGameCodeDLLFullPath,
                                           tempGameCodeDLLFullPath);

    game.UpdateAndRender(&threadContext, &gameMemory);

    getchar();

    return 0;
}
