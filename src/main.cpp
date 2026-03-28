#include "raylib.h"



void DrawGrid()
{
    const int gridSizeX = 64;
    const int gridSizeZ = 64;
    const int tileSize = GetScreenHeight() / gridSizeZ;
    const int totalWidth = gridSizeX * tileSize;
    const int totalHeight = gridSizeZ * tileSize;
    const int offsetX = (GetScreenWidth() - totalWidth) / 2;
    const int offsetY = (GetScreenHeight() - totalHeight) / 2 - 5;

    for (int x = 0; x < gridSizeX; x++)
    for (int z = 0; z < gridSizeZ; z++) 
    {
        // Checkerboard: white if (x + y) is even, black otherwise
        Color tileColor = ((x + z) % 2 == 0) ? WHITE : BLACK;
        
        DrawRectangle(offsetX + x * tileSize, offsetY + z * tileSize, tileSize, tileSize, tileColor);
    }
}



int main() 
{
    const int gridSizeX = 64;
    const int gridSizeZ = 64;

    const int tileSize = 12;
    
    InitWindow(1920, 1080, "Tile Grid");
    SetWindowState(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
    SetWindowPosition(0, 40);

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;                    // Exit the loop cleanly
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawGrid();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
