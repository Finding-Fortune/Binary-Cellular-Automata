#include "Grid.hpp"

#include "utility/Logger.hpp"
#include "utility/Timer.hpp"



void Grid::DrawGridDebug()
{
    for(int dx = -2; dx <= 2; dx++)
    for(int dy = -2; dy <= 2; dy++)
    {
        int baseX = 10 + dx;
        int baseY = 35 + dy;
        int drawIndex = 0;
        const int fontSize = 36;
        const int lineSpacing = 40;

        if(dx || dy)
        {
            DrawText(FMT("Avg Gen Time: {}us", STRP((generationTime), 2)),                        baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            DrawText(FMT("Runs per Test: {}", testRuns),                                          baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            DrawText(FMT("Base Seed: {}. (o, p)", seed),                                          baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            DrawText(FMT("Grid Length: {}. (k, l)", gridLength),                                  baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            DrawText(FMT("Total Chunks: {}", gridArea),                                           baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            DrawText(FMT("Points of Noise: {}", gridArea * CHUNK_AREA_BITS),                      baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            DrawText(FMT("Drawing {} caves (u)", ToSTR(caveToGenerate)),                          baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
            if(caveToGenerate == Caves::CA) DrawText(FMT("Iterations: {}. (-, =)", CAiterations), baseX, baseY + drawIndex++ * lineSpacing, fontSize, BLACK);
        }

        baseX = 10;
        baseY = 35;
        drawIndex = 0;

        DrawText(FMT("Avg Gen Time: {}us", STRP((generationTime), 2)),                        baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        DrawText(FMT("Runs per Test: {}", testRuns),                                          baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        DrawText(FMT("Base Seed: {}. (o, p)", seed),                                          baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        DrawText(FMT("Grid Length: {}. (k, l)", gridLength),                                  baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        DrawText(FMT("Total Chunks: {}", gridArea),                                           baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        DrawText(FMT("Points of Noise: {}", gridArea * CHUNK_AREA_BITS),                      baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        DrawText(FMT("Drawing {} caves (u)", ToSTR(caveToGenerate)),                          baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
        if(caveToGenerate == Caves::CA) DrawText(FMT("Iterations: {}. (-, =)", CAiterations), baseX, baseY + drawIndex++ * lineSpacing, fontSize, RAYWHITE);
    }

    CheckSettingsChange();
}



void Grid::CheckSettingsChange()
{
    static Timer SettingsChangeTimer(0.25);

    // Switch cave generation
    if (IsKeyPressed(KEY_U))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            caveToGenerate = static_cast<Caves>((static_cast<int>(caveToGenerate) + 1) % static_cast<int>(Caves::Total_Caves));
            RefreshCaves();
            return;
        }
    }
    // Increase, decrease grid length
    else if (IsKeyPressed(KEY_L))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            gridLength++;
            gridArea = gridLength * gridLength;
            RefreshCaves();
            return;
        }
    }
    else if (IsKeyPressed(KEY_K))
    {
        if(SettingsChangeTimer.HasElapsed())
            if (gridLength >= 2)
            {
                gridLength--;
                gridArea = gridLength * gridLength;
                RefreshCaves();
                return;
            }
    }

    // Increase, decrease seed
    else if (IsKeyPressed(KEY_P))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            seed++;
            RefreshCaves();
            return;
        }
    }
    else if (IsKeyPressed(KEY_O))
    {
        if(SettingsChangeTimer.HasElapsed())
        {
            seed--;
            RefreshCaves();
            return;
        }
    }

    // Increase, decrease iterations
    else if (IsKeyPressed(KEY_EQUAL))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            CAiterations++;
            RefreshCaves();
            return;
        }
    }
    else if (IsKeyPressed(KEY_MINUS))
    {
        if(SettingsChangeTimer.HasElapsed())
            if (CAiterations >= 1)
            {
                CAiterations--;
                RefreshCaves();
                return;
            }
    }
}



constexpr const char* Grid::ToSTR(const Caves cave)
{
    switch (cave)
    {
        case Caves::FN2:         return "FastNoise2";
        case Caves::CA:          return "CA";
        case Caves::Total_Caves: return "Total_Caves";
        default:                 return "Unknown";
    }
}
