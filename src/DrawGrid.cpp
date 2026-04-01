#include "Grid.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/bitfield.hpp>



void Grid::DrawGrid() 
{ 
    const int gap = 2;
    const bool tileGap = false;
    
    const int totalTilesAcross = CHUNK_AXIS_BITS * gridLength;
    const int tileSize = (GetScreenHeight() - (gridLength * gap)) / totalTilesAcross;
    
    const int chunkPxSize = CHUNK_AXIS_BITS * tileSize;
    
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