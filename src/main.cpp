#include "Grid.hpp"

#include "Logger.hpp"

#include "raylib.h"


int main() 
{
    const int gridSizeX = 64;
    const int gridSizeZ = 64;

    const int tileSize = 12;
    
    InitWindow(1920, 1080, "Tile Grid");
    SetWindowState(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
    SetWindowPosition(0, 40);

    Grid grid;

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        grid.DrawGridDebug();
        grid.DrawGrid();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
