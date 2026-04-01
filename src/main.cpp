#include "Grid.hpp"

#include "utility/Logger.hpp"

#include "raylib.h"


int main() 
{
    InitWindow(1920, 1080, "Tile Grid");
    SetWindowState(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
    SetWindowPosition(0, 25);

    Grid grid;

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        grid.DrawGrid();
        grid.DrawGridDebug();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
