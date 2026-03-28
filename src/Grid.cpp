#include "Grid.hpp"

#include "Logger.hpp"
#include "Timer.hpp"

#include "raylib.h"
#include <glm/glm.hpp>
#include <glm/gtc/bitfield.hpp>



static uint64_t SpatialHash(uint64_t x) 
{
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = (x ^ (x >> 31));
    return x;
}



Grid::Grid()
{
    RefreshCaveNoise();
}



void Grid::RefreshCaveNoise()
{
    InitCaveNoise();

    for (int i = 0; i < CAiterations; ++i)
    {
        CaveCA();
    }
}



void Grid::CaveCA() 
{
    uint64_t temp[64] = {0};

    for (int x = 0; x < 64; ++x) 
    {
        const uint64_t L = (x > 0)   ? columns[x-1] : ~0ULL;  // border = solid walls
        const uint64_t C = columns[x];
        const uint64_t R = (x < 63)  ? columns[x+1] : ~0ULL;

        // 8 neighbor masks + self (standard Moore neighborhood, 9 cells total).
        // Shifts handle vertical neighbors; border walls are injected with OR.
        const uint64_t upper_border = 1ULL;           // LSB (y=0) up-neighbor = wall
        const uint64_t lower_border = 1ULL << 63;     // MSB (y=63) down-neighbor = wall

        const uint64_t up_left   = (L << 1) | upper_border;
        const uint64_t up        = (C << 1) | upper_border;
        const uint64_t up_right  = (R << 1) | upper_border;
        const uint64_t left      = L;
        const uint64_t right     = R;
        const uint64_t down_left = (L >> 1) | lower_border;
        const uint64_t down      = (C >> 1) | lower_border;
        const uint64_t down_right= (R >> 1) | lower_border;

        // 4-bit counter planes (b0 = LSB ... b3 = 8's place). Enough for 0-15.
        uint64_t count[4] = {0};

        // Add each of the 9 neighbor bits into the parallel counter using ripple-carry
        // (pure & ^ |, sequential only inside the 4-bit width — exactly the "sequential"
        // part you mentioned, but still fully bit-parallel across all 64 rows).
        const uint64_t neighbors[9] = { up_left, up, up_right, left, C, right, down_left, down, down_right };
        for (int i = 0; i < 9; ++i) 
        {
            uint64_t carry = neighbors[i];
            for (int b = 0; b < 4; ++b) 
            {
                uint64_t sum  = count[b] ^ carry;
                carry         = count[b] & carry;
                count[b]      = sum;
                if (carry == 0) break;   // early-out when no more carry
            }
        }

        // Extract >=5 from the 4-bit count (exact match to the classic rule).
        const uint64_t b0 = count[0];  // 1's
        const uint64_t b1 = count[1];  // 2's
        const uint64_t b2 = count[2];  // 4's
        const uint64_t b3 = count[3];  // 8's
        const uint64_t ge5 = b3 | (b2 & (b1 | b0));

        temp[x] = ge5;
    }

    // Write back (or use pointer swap in real engine code).
    for (int x = 0; x < 64; ++x) 
    {
        columns[x] = temp[x];
    }
}



void Grid::InitCaveNoise() 
{
    for (int x = 0; x < 64; ++x) 
    {
        columns[x] = SpatialHash(x);
    }
}



void Grid::DrawGridDebug()
{
    DrawText(FMT("CA Iterations: {}", CAiterations), 10, 35, 36, RAYWHITE);

    static Timer CAChangeTimer(0.5);

    // Increase, decrease iterations
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
    {
        if(CAChangeTimer.HasElapsed()) 
        {
            CAiterations++;
            RefreshCaveNoise();
        }
    }

    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
    {
        if(CAChangeTimer.HasElapsed())
            if (CAiterations >= 2)
            {
                CAiterations--;
                RefreshCaveNoise();
            }
    }
}



void Grid::DrawGrid()
{
    const int gridSizeX = 64;
    const int gridSizeZ = 64;
    const int tileSize = GetScreenHeight() / gridSizeZ;
    const int totalWidth = gridSizeX * tileSize;
    const int totalHeight = gridSizeZ * tileSize;
    const int offsetX = (GetScreenWidth() - totalWidth) / 2;
    const int offsetY = (GetScreenHeight() - totalHeight) / 2 - 5;

    for (int x = 0; x < gridSizeX; x++)
    for (int z = 0; z < gridSizeZ; z++) 
    {
        // Checkerboard: white if (x + y) is even, black otherwise
        const uint64_t result = glm::bitfieldExtract(columns[x], z, 1);
        Color tileColor = (result == 1) ? WHITE : BLACK;
        
        DrawRectangle(offsetX + x * tileSize, offsetY + z * tileSize, tileSize, tileSize, tileColor);
    }
}


