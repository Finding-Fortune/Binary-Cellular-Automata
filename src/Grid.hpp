#pragma once

#include <cstdint>



class Grid
{
public:
    Grid();

    void DrawGrid();

    void InitCaveNoise();

    void RefreshCaveNoise();

    void CaveCA();
    
    void DrawGridDebug();

    uint64_t columns[64];

    int CAiterations = 3;
};