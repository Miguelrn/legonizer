#include <stdio.h>
#include "raylib.h"

void hello_world(void){
    printf("Hello, World!\n");
}

void load_lenna(void){
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Pixeler");

    Texture2D lenna = LoadTexture("assets/lenna.png");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(lenna, 100, 50, WHITE);

        EndDrawing();
    }

    UnloadTexture(lenna);

    CloseWindow();
}