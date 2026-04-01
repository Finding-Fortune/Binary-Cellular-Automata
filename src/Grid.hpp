#pragma once

#include "raylib.h"
#include <cstdint>
#include <array>
#include <vector>

#define CHUNK_AXIS_BITS 64


class Grid
{
public:
    Grid();

    void DrawGrid();

    void DrawGridDebug();

private:
    void InitCaveNoise();

    void RefreshCaves();

    void CaveCA(const int iteration, const int chunkCoordX, const int chunkCoordZ);

    void CaveCAIsolated(const int iteration, const int chunkCoordX, const int chunkCoordZ);

    void GenerateCACaves();

    void GenerateFN2Caves();

    uint64_t SpatialHash(const uint64_t x, const uint64_t z);

    int gridLength = 3;

    std::vector<std::vector<std::array<uint64_t, 64>>> grid;

    int seed = 0;
    int CAiterations = 3;

    // If false render fastnoise2 caves
    bool renderCACaves = true;

    double generationTime = 0.0;

    Texture2D stoneTexture;
};