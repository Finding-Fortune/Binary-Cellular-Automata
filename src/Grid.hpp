#pragma once

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

    void CaveCA(const int chunkCoordX, const int chunkCoordZ);
    void CaveCAIsolated(const int chunkCoordX, const int chunkCoordZ);
    
    void DrawGridDebug();

    static constexpr int tilesInChunkAxis = 64;

    int gridLength = 4;

    std::vector<std::vector<std::array<uint64_t, 64>>> grid;

    int seed = 0;
    int CAiterations = 3;

private:
    uint64_t SpatialHash(uint64_t x, uint64_t z);
};