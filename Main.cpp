#include "raylib.h"
#include <stdio.h>

#define TILE_SIZE 40
#define TILE_COUNT_X 20
#define TILE_COUNT_Y 15

int main() {
    InitWindow(800, 600, "Pacman");

    SetTargetFPS(60);

    Image spriteW = LoadImage("assets/pacman_w.png");
    Image spriteA = LoadImage("assets/pacman_a.png");
    Image spriteS = LoadImage("assets/pacman_s.png");
    Image spriteD = LoadImage("assets/pacman_d.png");

    Texture2D textureW = LoadTextureFromImage(spriteW);
    Texture2D textureA = LoadTextureFromImage(spriteA);
    Texture2D textureS = LoadTextureFromImage(spriteS);
    Texture2D textureD = LoadTextureFromImage(spriteD);

    UnloadImage(spriteW);
    UnloadImage(spriteA);
    UnloadImage(spriteS);
    UnloadImage(spriteD);

    //Vector2 logoPosition = { (float)(GetScreenWidth() - textureW.width) / 2,(float)(GetScreenHeight() - textureW.height) / 2 };
    Vector2 logoPosition = {40.0f,40.0f};
    Texture2D currentTexture = textureW;

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_A) && logoPosition.x > 0) {
            logoPosition.x -= TILE_SIZE;
            currentTexture = textureA;
        }
        if (IsKeyDown(KEY_D) && logoPosition.x < (TILE_COUNT_X - 1) * TILE_SIZE) {
            logoPosition.x += TILE_SIZE;
            currentTexture = textureD;
        }
        if (IsKeyDown(KEY_S) && logoPosition.y < (TILE_COUNT_Y - 1) * TILE_SIZE) {
            logoPosition.y += TILE_SIZE;
            currentTexture = textureS;
        }
        if (IsKeyDown(KEY_W) && logoPosition.y > 0) {
            logoPosition.y -= TILE_SIZE;
            currentTexture = textureW;
        }

        BeginDrawing();
        ClearBackground(Color{40, 40, 40, 255});

        for (int y = 0; y < TILE_COUNT_Y; y++) {
            for (int x = 0; x < TILE_COUNT_X; x++) {
                DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GRAY);
            }
        }

        DrawTexture(currentTexture, logoPosition.x, logoPosition.y, WHITE);
        DrawFPS(30, 30);

        EndDrawing();
    }

    UnloadTexture(textureW);
    UnloadTexture(textureA);
    UnloadTexture(textureS);
    UnloadTexture(textureD);
    CloseWindow();
    return 0;
}
