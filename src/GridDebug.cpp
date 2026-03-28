#include "Grid.hpp"

#include "Logger.hpp"
#include "Timer.hpp"



void Grid::DrawGridDebug()
{
    DrawText(FMT("CA Iterations: {}. (-, =)", CAiterations), 10, 35, 36, RAYWHITE);
    DrawText(FMT("Seed: {}. (o, p)", seed), 10, 75, 36, RAYWHITE);
    DrawText(FMT("Grid Length: {}. (l, k)", gridLength), 10, 105, 36, RAYWHITE);
    DrawText(FMT("Drawing {} caves", (renderCACaves == true ? "CA" : "FastNoise2")), 10, 145, 36, RAYWHITE);
    DrawText(FMT("Generation Time: {}", STRP(generationTime, 2)), 10, 185, 36, RAYWHITE);

    static Timer SettingsChangeTimer(0.25);

    // Switch cave generation
    if (IsKeyPressed(KEY_U))
    {
        if(SettingsChangeTimer.HasElapsed()) 
        {
            renderCACaves = !renderCACaves;
            RefreshCaveNoise();
            return;
        }
    }
    // Increase, decrease grid length
    else if (IsKeyPressed(KEY_L))
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