#pragma once

#include "raylib.h"
#include <cstdint>
#include <array>
#include <vector>

#define CHUNK_AXIS_BITS 64
#define CHUNK_AREA_BITS CHUNK_AXIS_BITS * CHUNK_AXIS_BITS

enum class Caves;


class Grid
{
public:
    Grid();

    void DrawGrid();

    void DrawGridDebug();

    void CheckSettingsChange();

private:
    void InitCaveNoise();

    void RefreshCaves();

    void CaveCA(const int iteration, const int chunkCoordX, const int chunkCoordZ);

    void CaveCAIsolated(const int iteration, const int chunkCoordX, const int chunkCoordZ);

    void GenerateCACaves();

    void GenerateFN2Caves();

    uint64_t SpatialHash(const uint64_t x, const uint64_t z);

    size_t GetChunkIndex(const int chunkCoordX, const int chunkCoordZ);

    int gridLength = 3;
    int gridArea = gridLength * gridLength;

    // std::vector<std::vector<std::array<uint64_t, 64>>> grid;
    std::vector<std::array<uint64_t, 64>> grid;

    int seed = 0;
    int CAiterations = 3;

    // Which cave we want to generate
    enum class Caves
    {
        FN2         = 0,
        CA          = 1,
        Total_Caves = 2
    };
    Caves caveToGenerate = Caves::CA;

    static constexpr const char* ToSTR(const Caves cave);

    double generationTime = 0.0;

    const Texture2D stoneTexture;
};