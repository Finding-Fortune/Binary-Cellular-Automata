#include "Grid.hpp"

#include "Logger.hpp"
#include "Timer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/bitfield.hpp>
#include <algorithm>
#include <random>
#include <numeric>
#include "FastNoise/FastNoise.h"



uint64_t Grid::SpatialHash(uint64_t x, uint64_t z) 
{
    x += seed;
    z += seed;

    uint64_t h = x + 0x9e3779b97f4a7c15ULL + (z << 6) + (z >> 2);
    
    h ^= z + 0x517cc1b727220a95ULL; 
    h = (h ^ (h >> 30)) * 0xbf58476d1ce4e5b9ULL;
    h = (h ^ (h >> 27)) * 0x94d049bb133111ebULL;
    h = (h ^ (h >> 31));
    return h;
}



Grid::Grid()
{
    stoneTexture = LoadTexture("resources/Stone.png");

    RefreshCaveNoise();
}



void Grid::RefreshCaveNoise()
{
    // Resize grid
    grid.resize(gridLength);
    for (int i = 0; i < gridLength; ++i) 
        grid[i] = std::vector<std::array<uint64_t, 64>>(gridLength);

    double startTime = GetTime();

    // Cellular Automata
    if(renderCACaves)
    {
        InitCaveNoise();

        for(int chunkCoordX = 0; chunkCoordX < gridLength; ++chunkCoordX)
        for(int chunkCoordZ = 0; chunkCoordZ < gridLength; ++chunkCoordZ)
        {
            for (int i = 0; i < CAiterations; ++i)
            {
                CaveCA(i, chunkCoordX, chunkCoordZ);
                // CaveCAIsolated(i, chunkCoordX, chunkCoordZ);
            }
        }
    }
    // Fastnoise2
    else 
    {
        InitCaveNoise();
        
        // static const auto noiseGen = FastNoise::NewFromEncodedNodeTree(
        //     "EwCamZk+GgABEQACAAAAAADgQBAAAACIQR8AFgABAAAACwADAAAAAgAAAAMAAAAEAAAAAAAAAD8BFAD//wAAAAAAAD8AAAAAPwAAAAA/AAAAAD8BFwAAAIC/AACAPz0KF0BSuB5AEwAAAKBABgAAj8J1PACamZk+AAAAAAAA4XoUPw=="
        // );
        auto simplex = FastNoise::New<FastNoise::Simplex>();
        std::vector<float> noiseBuffer(64 * 64);
        static const float frequency = 10.1f;

        for(int chunkCoordX = 0; chunkCoordX < gridLength; ++chunkCoordX)
        for(int chunkCoordZ = 0; chunkCoordZ < gridLength; ++chunkCoordZ)
        {
            std::array<uint64_t, 64>& chunk = grid[chunkCoordX][chunkCoordZ];
            const float xOffset = (float)(chunkCoordX * 64) * frequency;
            const float zOffset = (float)(chunkCoordZ * 64) * frequency;

            simplex->GenUniformGrid2D(
                noiseBuffer.data(),
                xOffset, zOffset, // Offsets
                64, 64,                                              // Counts
                frequency, frequency,                                // Step sizes (X and Y)
                seed                                                 // Seed
            );
            // Map noise to bitmask
            for (uint64_t x = 0; x < 64; ++x) 
            {
                uint64_t columnMask = 0;
                for (uint64_t z = 0; z < 64; ++z) 
                {
                    float noiseVal = noiseBuffer[z * 64 + x];

                    if (noiseVal > 0.1f) {
                        columnMask |= (1ULL << z);
                    }
                }
                chunk[x] = columnMask;
            }
        }
    }

    double endTime = GetTime();
    generationTime = endTime - startTime;
}



void Grid::CaveCAIsolated(const int iteration, const int chunkCoordX, const int chunkCoordZ) 
{
    std::array<uint64_t, 64>& chunk = grid[chunkCoordX][chunkCoordZ];
    uint64_t temp[64] = {0};

    for (int x = 0; x < 64; ++x) 
    {
        const uint64_t L = (x > 0)   ? chunk[x-1] : ~0ULL;  // border = solid walls
        const uint64_t C = chunk[x];
        const uint64_t R = (x < 63)  ? chunk[x+1] : ~0ULL;

        // 8 neighbor masks + self
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

        // Extract >=5 from the 4-bit count
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
        chunk[x] = temp[x];
    }
}




void Grid::CaveCA(const int iteration, const int chunkCoordX, const int chunkCoordZ) 
{
    std::array<uint64_t, 64>& chunk = grid[chunkCoordX][chunkCoordZ];
    uint64_t temp[64] = {0};

    // Horizontal neighbor chunks (X direction)
    const bool hasLeft  = (chunkCoordX > 0);
    const bool hasRight = (chunkCoordX < gridLength - 1);
    const std::array<uint64_t, 64>* leftChunk  = hasLeft  ? &grid[chunkCoordX - 1][chunkCoordZ] : nullptr;
    const std::array<uint64_t, 64>* rightChunk = hasRight ? &grid[chunkCoordX + 1][chunkCoordZ] : nullptr;

    // Vertical neighbor chunks (Z direction)
    const bool hasAbove = (chunkCoordZ > 0);
    const bool hasBelow = (chunkCoordZ < gridLength - 1);
    const std::array<uint64_t, 64>* aboveChunk = hasAbove ? &grid[chunkCoordX][chunkCoordZ - 1] : nullptr;
    const std::array<uint64_t, 64>* belowChunk = hasBelow ? &grid[chunkCoordX][chunkCoordZ + 1] : nullptr;

    for (int x = 0; x < 64; ++x) 
    {
        // Resolve the three columns in this row
        const uint64_t L = (x > 0)   ? chunk[x-1] :
                           (hasLeft  ? (*leftChunk)[63] : ~0ULL);
        const uint64_t C = chunk[x];
        const uint64_t R = (x < 63)  ? chunk[x+1] :
                           (hasRight ? (*rightChunk)[0]  : ~0ULL);

        // Resolve the "above" versions of L, C, R (for Z seam)
        uint64_t above_L = 0, above_C = 0, above_R = 0;
        if (hasAbove)
        {
            int aboveZ = chunkCoordZ - 1;
            above_C = grid[chunkCoordX][aboveZ][x];

            if (x > 0)                    above_L = grid[chunkCoordX][aboveZ][x-1];
            else if (hasLeft)             above_L = grid[chunkCoordX-1][aboveZ][63];

            if (x < 63)                   above_R = grid[chunkCoordX][aboveZ][x+1];
            else if (hasRight)            above_R = grid[chunkCoordX+1][aboveZ][0];
        }

        // Resolve the "below" versions of L, C, R
        uint64_t below_L = 0, below_C = 0, below_R = 0;
        if (hasBelow)
        {
            int belowZ = chunkCoordZ + 1;
            below_C = grid[chunkCoordX][belowZ][x];

            if (x > 0)                    below_L = grid[chunkCoordX][belowZ][x-1];
            else if (hasLeft)             below_L = grid[chunkCoordX-1][belowZ][63];

            if (x < 63)                   below_R = grid[chunkCoordX][belowZ][x+1];
            else if (hasRight)            below_R = grid[chunkCoordX+1][belowZ][0];
        }

        // Vertical edge injects
        const uint64_t upper_inject_L = hasAbove ? (above_L >> 63) : 1ULL;   // 0 or 1 for LSB
        const uint64_t upper_inject_C = hasAbove ? (above_C >> 63) : 1ULL;
        const uint64_t upper_inject_R = hasAbove ? (above_R >> 63) : 1ULL;

        const uint64_t lower_inject_L = hasBelow ? ((below_L & 1ULL) << 63) : (1ULL << 63);
        const uint64_t lower_inject_C = hasBelow ? ((below_C & 1ULL) << 63) : (1ULL << 63);
        const uint64_t lower_inject_R = hasBelow ? ((below_R & 1ULL) << 63) : (1ULL << 63);

        // Build the neighbor bitboards
        const uint64_t up_left   = (L << 1) | upper_inject_L;
        const uint64_t up        = (C << 1) | upper_inject_C;
        const uint64_t up_right  = (R << 1) | upper_inject_R;

        const uint64_t left      = L;
        const uint64_t right     = R;

        const uint64_t down_left = (L >> 1) | lower_inject_L;
        const uint64_t down      = (C >> 1) | lower_inject_C;
        const uint64_t down_right= (R >> 1) | lower_inject_R;

        // Get counts for neighbor comparisons
        uint64_t count[4] = {0};
        const uint64_t neighbors[9] = { up_left, up, up_right, left, C, right, down_left, down, down_right };
        for (int i = 0; i < 9; ++i) 
        {
            uint64_t carry = neighbors[i];
            for (int b = 0; b < 4; ++b) 
            {
                uint64_t sum  = count[b] ^ carry;
                carry         = count[b] & carry;
                count[b]      = sum;
                if (carry == 0) break;
            }
        }

        const uint64_t b0 = count[0];
        const uint64_t b1 = count[1];
        const uint64_t b2 = count[2];
        const uint64_t b3 = count[3];
        temp[x] = b3 | (b2 & (b1 | b0));

        // A few attempts at changing the final result depending on iteration
        // temp[x] = b3 | (b2 & b1 & b0);
        // bool even = (iteration % 2 == 0);
        // uint64_t ge5 = b3 | (b2 & (b1 | b0));
        // uint64_t ge6 = b3 | (b2 & b1);
        // temp[x] = even ? ge5 : ge6;
    }

    for (int x = 0; x < 64; ++x) 
    {
        chunk[x] = temp[x];
    }
}



void Grid::InitCaveNoise() 
{
    for(uint64_t chunkCoordX = 0; chunkCoordX < gridLength; ++chunkCoordX)
    for(uint64_t chunkCoordZ = 0; chunkCoordZ < gridLength; ++chunkCoordZ)
    {
        std::array<uint64_t, 64>& chunk = grid[chunkCoordX][chunkCoordZ];

        for (uint64_t x = 0; x < 64; ++x) 
        {
            const uint64_t worldX = chunkCoordX * 64 + x;
            const uint64_t worldZ = chunkCoordZ * 64;

            const uint64_t noise = SpatialHash(worldX, worldZ);
            chunk[x] = noise;
        }
    }
}



void Grid::DrawGrid() 
{ 
    const int gap = 2;
    const bool tileGap = false;
    
    const int totalTilesAcross = tilesInChunkAxis * gridLength;
    const int tileSize = (GetScreenHeight() - (gridLength * gap)) / totalTilesAcross;
    
    const int chunkPxSize = tilesInChunkAxis * tileSize;
    
    const int totalWidth  = gridLength  * (chunkPxSize + gap) - gap;
    const int totalHeight = gridLength * (chunkPxSize + gap) - gap;
    
    const int startScreenX = (GetScreenWidth()  - totalWidth)  / 2;
    const int startScreenY = (GetScreenHeight() - totalHeight) / 2;

    for(int cz = 0; cz < gridLength; ++cz) 
    for(int cx = 0; cx < gridLength; ++cx) 
    { 
        std::array<uint64_t, 64>& chunk = grid[cx][cz]; 

        // for (int localX = 1; localX < 63; ++localX) 
        // for (int localZ = 1; localZ < 63; ++localZ) 
        for (int localX = 0; localX < 64; ++localX) 
        for (int localZ = 0; localZ < 64; ++localZ) 
        {  
            const bool isWall = glm::bitfieldExtract(chunk[localX], localZ, 1) == 0; 
            
            const int screenX = startScreenX + cx * (chunkPxSize + gap) + localX * tileSize - cx;
            const int screenY = startScreenY + cz * (chunkPxSize + gap) + localZ * tileSize - cz;
            
            const int drawSize = tileGap ? tileSize - 1 : tileSize;
            if(isWall)
            {
                Rectangle source = { 0.0f, 0.0f, (float)stoneTexture.width, (float)stoneTexture.height };
                Rectangle dest = { (float)screenX, (float)screenY, (float)drawSize, (float)drawSize };
                DrawTexturePro(stoneTexture, source, dest, { 0, 0 }, 0.0f, WHITE);
            }
            else DrawRectangle(screenX, screenY, drawSize, drawSize, WHITE); 
        } 
    } 
}


