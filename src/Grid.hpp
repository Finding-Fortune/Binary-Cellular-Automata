#pragma once

#include "raylib.h"
#include <cstdint>
#include <array>
#include <vector>



class Grid
{
public:
    Grid();

    void DrawGrid();

    void InitCaveNoise();

    void RefreshCaveNoise();

    void CaveCA(const int iteration, const int chunkCoordX, const int chunkCoordZ);

    void CaveCAIsolated(const int iteration, const int chunkCoordX, const int chunkCoordZ);

    void CaveCAIsolatedShrink(const int iteration, const int chunkCoordX, const int chunkCoordZ);
    
    void DrawGridDebug();

    static constexpr int tilesInChunkAxis = 64;

    int gridLength = 3;

    std::vector<std::vector<std::array<uint64_t, 64>>> grid;

    int seed = 0;
    int CAiterations = 2;

    // If false render fastnoise2 caves
    bool renderCACaves = true;

    double generationTime = 0.0;

private:
    uint64_t SpatialHash(uint64_t x, uint64_t z);

    Texture2D stoneTexture;
};