#include "Grid.hpp"

#include "Logger.hpp"
#include "Timer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/bitfield.hpp>



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

    InitCaveNoise();

    for(int chunkCoordX = 0; chunkCoordX < gridLength; ++chunkCoordX)
    for(int chunkCoordZ = 0; chunkCoordZ < gridLength; ++chunkCoordZ)
    {
        for (int i = 0; i < CAiterations; ++i)
        {
            CaveCA(chunkCoordX, chunkCoordZ);
        }
    }
}



void Grid::CaveCA(const int chunkCoordX, const int chunkCoordZ) 
{
    std::array<uint64_t, 64>& chunk = grid[chunkCoordX][chunkCoordZ];
    uint64_t temp[64] = {0};

    // ── Horizontal neighbor chunks (X direction) ──
    const bool hasLeft  = (chunkCoordX > 0);
    const bool hasRight = (chunkCoordX < gridLength - 1);
    const std::array<uint64_t, 64>* leftChunk  = hasLeft  ? &grid[chunkCoordX - 1][chunkCoordZ] : nullptr;
    const std::array<uint64_t, 64>* rightChunk = hasRight ? &grid[chunkCoordX + 1][chunkCoordZ] : nullptr;

    // ── Vertical neighbor chunks (Z direction) ──
    const bool hasAbove = (chunkCoordZ > 0);
    const bool hasBelow = (chunkCoordZ < gridLength - 1);
    const std::array<uint64_t, 64>* aboveChunk = hasAbove ? &grid[chunkCoordX][chunkCoordZ - 1] : nullptr;
    const std::array<uint64_t, 64>* belowChunk = hasBelow ? &grid[chunkCoordX][chunkCoordZ + 1] : nullptr;

    for (int x = 0; x < 64; ++x) 
    {
        // ── Resolve the three columns in this row (seamless in X) ──
        const uint64_t L = (x > 0)   ? chunk[x-1] :
                           (hasLeft  ? (*leftChunk)[63] : ~0ULL);
        const uint64_t C = chunk[x];
        const uint64_t R = (x < 63)  ? chunk[x+1] :
                           (hasRight ? (*rightChunk)[0]  : ~0ULL);

        // ── Resolve the "above" versions of L, C, R (for Z seam) ──
        uint64_t above_L = 0, above_C = 0, above_R = 0;
        if (hasAbove)
        {
            int aboveZ = chunkCoordZ - 1;
            above_C = grid[chunkCoordX][aboveZ][x];

            if (x > 0)                    above_L = grid[chunkCoordX][aboveZ][x-1];
            else if (hasLeft)             above_L = grid[chunkCoordX-1][aboveZ][63];
            // else outer border → stays 0 (wall handled later)

            if (x < 63)                   above_R = grid[chunkCoordX][aboveZ][x+1];
            else if (hasRight)            above_R = grid[chunkCoordX+1][aboveZ][0];
        }

        // ── Resolve the "below" versions of L, C, R ──
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

        // ── Vertical edge injects (replaces the old fixed borders) ──
        const uint64_t upper_inject_L = hasAbove ? (above_L >> 63) : 1ULL;   // 0 or 1 for LSB
        const uint64_t upper_inject_C = hasAbove ? (above_C >> 63) : 1ULL;
        const uint64_t upper_inject_R = hasAbove ? (above_R >> 63) : 1ULL;

        const uint64_t lower_inject_L = hasBelow ? ((below_L & 1ULL) << 63) : (1ULL << 63);
        const uint64_t lower_inject_C = hasBelow ? ((below_C & 1ULL) << 63) : (1ULL << 63);
        const uint64_t lower_inject_R = hasBelow ? ((below_R & 1ULL) << 63) : (1ULL << 63);

        // ── Build the 9 neighbor bitboards (now fully seamless in both axes) ──
        const uint64_t up_left   = (L << 1) | upper_inject_L;
        const uint64_t up        = (C << 1) | upper_inject_C;
        const uint64_t up_right  = (R << 1) | upper_inject_R;

        const uint64_t left      = L;
        const uint64_t right     = R;

        const uint64_t down_left = (L >> 1) | lower_inject_L;
        const uint64_t down      = (C >> 1) | lower_inject_C;
        const uint64_t down_right= (R >> 1) | lower_inject_R;

        // ── The rest is unchanged (4-bit parallel counter + >=5 rule) ──
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
        temp[x] = b3 | (b2 & b1);
        // temp[x] = ~temp[x];
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
            // uint64_t hash = SpatialHash(x + chunkCoordX * 64, chunkCoordZ * 64);
            // chunk[x] = hash;

            uint64_t h1 = SpatialHash(x + chunkCoordX * 64, chunkCoordZ * 64);
            uint64_t h2 = SpatialHash(x + 410 + chunkCoordX * 64, x + 420 - chunkCoordZ * 64);
            uint64_t h3 = SpatialHash(x - 430 + chunkCoordX * 64, x - 940 - chunkCoordZ * 64);

            chunk[x] = h1 | (h2 & h3);
        }
    }
}



void Grid::DrawGridDebug()
{
    DrawText("Walls are Black", 10, 35, 36, RAYWHITE);
    DrawText(FMT("CA Iterations: {}. (-, =)", CAiterations), 10, 75, 36, RAYWHITE);
    DrawText(FMT("Seed: {}. (o, p)", seed), 10, 115, 36, RAYWHITE);
    DrawText(FMT("Grid Length: {}. (l, k)", gridLength), 10, 155, 36, RAYWHITE);

    static Timer SettingsChangeTimer(0.25);

    // Increase, decrease grid length
    if (IsKeyPressed(KEY_L))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            gridLength++;
            RefreshCaveNoise();
            return;
        }
    }
    else if (IsKeyPressed(KEY_K))
    {
        if(SettingsChangeTimer.HasElapsed())
            if (gridLength >= 2)
            {
                gridLength--;
                RefreshCaveNoise();
                return;
            }
    }

    // Increase, decrease seed
    else if (IsKeyPressed(KEY_P))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            seed++;
            RefreshCaveNoise();
            return;
        }
    }
    else if (IsKeyPressed(KEY_O))
    {
        if(SettingsChangeTimer.HasElapsed())
        {
            seed--;
            RefreshCaveNoise();
            return;
        }
    }

    // Increase, decrease iterations
    else if (IsKeyPressed(KEY_EQUAL))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            CAiterations++;
            RefreshCaveNoise();
            return;
        }
    }
    else if (IsKeyPressed(KEY_MINUS))
    {
        if(SettingsChangeTimer.HasElapsed())
            if (CAiterations >= 1)
            {
                CAiterations--;
                RefreshCaveNoise();
                return;
            }
    }
}



void Grid::DrawGrid() 
{ 
    const int tilesInChunkAxis = 64; 
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

        for (int localX = 0; localX < tilesInChunkAxis; ++localX) 
        for (int localZ = 0; localZ < tilesInChunkAxis; ++localZ) 
        { 
            const bool isWall = glm::bitfieldExtract(chunk[localX], localZ, 1) == 1; 
            Color tileColor = isWall ? BLACK : WHITE; 
            
            int screenX = startScreenX + cx * (chunkPxSize + gap) + localX * tileSize;
            int screenY = startScreenY + cz * (chunkPxSize + gap) + localZ * tileSize;
            
            int drawSize = tileGap ? tileSize - 1 : tileSize;
            if(isWall)
            {
                Rectangle source = { 0.0f, 0.0f, (float)stoneTexture.width, (float)stoneTexture.height };
                Rectangle dest = { (float)screenX, (float)screenY, (float)drawSize, (float)drawSize };
                DrawTexturePro(stoneTexture, source, dest, { 0, 0 }, 0.0f, WHITE);
            }
            else DrawRectangle(screenX, screenY, drawSize, drawSize, tileColor); 
        } 
    } 
}


