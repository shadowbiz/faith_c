#include <cstdlib>

#include "faith.h"

#include "faith_string.cpp"
#include "faith_json.cpp"
#include "faith_random.h"
#include "faith_world.cpp"

internal void
ArenaInitialize(MemoryArena *arena, size_t memorySize, uint8 *storage)
{
    arena->Size = memorySize;
    arena->Base = storage;
    arena->Used = 0;
}

extern "C" GAME_UPDATE(GameUpdate)
{
    RandomInit();

    GameState *gameState = (GameState *)memory->PermamentStorage;
    if (!memory->IsInitialized)
    {
        size_t memorySize = memory->PermamentStorageSize - sizeof(GameState);
        uint8 *storage = (uint8 *)memory->PermamentStorage + sizeof(GameState);

        ArenaInitialize(&gameState->WorldArena, memorySize, storage);

        WorldInitialize(memory, gameState);
        memory->IsInitialized = true;
    }
}
