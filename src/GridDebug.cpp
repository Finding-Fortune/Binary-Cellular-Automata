#include "Grid.hpp"

#include "Logger.hpp"
#include "Timer.hpp"



void Grid::DrawGridDebug()
{
    for(int dx= -2; dx <= 2; dx++)
    for(int dy= -2; dy <= 2; dy++)
    {
        if(dx || dy)
        {
            DrawText(FMT("CA Iterations: {}. (-, =)", CAiterations), 10  +dx, 35 + dy, 36, BLACK);
            DrawText(FMT("Seed: {}. (o, p)", seed), 10 + dx, 75 + dy, 36, BLACK);
            DrawText(FMT("Grid Length: {}. (k, l)", gridLength), 10 + dx, 105 + dy, 36, BLACK);
            DrawText(FMT("Drawing {} caves", (renderCACaves == true ? "CA" : "FastNoise2")), 10 + dx, 145 + dy, 36, BLACK);
            DrawText(FMT("Generation Time: {}ms", STRP((generationTime * 1000.0), 2)), 10 + dx, 185 + dy, 36, BLACK);
        }

        DrawText(FMT("CA Iterations: {}. (-, =)", CAiterations), 10, 35, 36, RAYWHITE);
        DrawText(FMT("Seed: {}. (o, p)", seed), 10, 75, 36, RAYWHITE);
        DrawText(FMT("Grid Length: {}. (k, l)", gridLength), 10, 105, 36, RAYWHITE);
        DrawText(FMT("Drawing {} caves", (renderCACaves == true ? "CA" : "FastNoise2")), 10, 145, 36, RAYWHITE);
        DrawText(FMT("Generation Time: {}ms", STRP((generationTime * 1000.0), 2)), 10, 185, 36, RAYWHITE);
    }

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