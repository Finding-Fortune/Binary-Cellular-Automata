#pragma once

#include <cstdint>
#include <array>



class Grid
{
public:
    Grid();

    void DrawGrid();

    void InitCaveNoise();

    void RefreshCaveNoise();

    void CaveCA(const int chunkCoordX, const int chunkCoordZ);
    
    void DrawGridDebug();

    static constexpr int gridWidth = 2;
    static constexpr int gridDepth = 2;

    std::array<std::array<std::array<uint64_t, 64>, gridDepth>, gridWidth> grid;

    int seed = 0;
    int CAiterations = 3;

private:
    uint64_t SpatialHash(uint64_t x, uint64_t z);
};