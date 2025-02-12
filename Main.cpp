#include "raylib.h"
#include <stdio.h>

#define TILE_SIZE 40
#define TILE_COUNT_X 20
#define TILE_COUNT_Y 15

typedef struct {
    Vector2 position;
    bool collected;
} Item;

typedef struct {
    Vector2 position;
    Texture2D texture;
} Enemy;

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

    Sound collectSound = LoadSound("assets/collect.wav");
    Sound hitSound = LoadSound("assets/hit.wav");

    Vector2 logoPosition = {(float)(GetScreenWidth() - textureW.width) / 2, (float)(GetScreenHeight() - textureW.height) / 2};
    Texture2D currentTexture = textureW;

    Item items[] = {
        {{2 * TILE_SIZE, 2 * TILE_SIZE}, false},
        {{5 * TILE_SIZE, 5 * TILE_SIZE}, false},
        {{8 * TILE_SIZE, 8 * TILE_SIZE}, false}
    };
    int itemCount = sizeof(items) / sizeof(Item);

    Enemy enemy = {{10 * TILE_SIZE, 10 * TILE_SIZE}, textureD};

    Vector2 enemyDirection = {-1.0f, 0.0f};
    int score = 0;

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_A) && logoPosition.x > 0) {
            logoPosition.x -= 2.0f;
            currentTexture = textureA;
        }
        if (IsKeyDown(KEY_D) && logoPosition.x < (TILE_COUNT_X - 1) * TILE_SIZE) {
            logoPosition.x += 2.0f;
            currentTexture = textureD;
        }
        if (IsKeyDown(KEY_S) && logoPosition.y < (TILE_COUNT_Y - 1) * TILE_SIZE) {
            logoPosition.y += 2.0f;
            currentTexture = textureS;
        }
        if (IsKeyDown(KEY_W) && logoPosition.y > 0) {
            logoPosition.y -= 2.0f;
            currentTexture = textureW;
        }

        for (int i = 0; i < itemCount; i++) {
            if (!items[i].collected && CheckCollisionRecs(
                (Rectangle){logoPosition.x, logoPosition.y, textureW.width, textureW.height},
                (Rectangle){items[i].position.x, items[i].position.y, TILE_SIZE, TILE_SIZE})) {
                items[i].collected = true;
                score += 10;
                PlaySound(collectSound);
            }
        }

        enemy.position.x += enemyDirection.x * 2.0f;
        enemy.position.y += enemyDirection.y * 2.0f;

        if (enemy.position.x <= 0 || enemy.position.x >= (TILE_COUNT_X - 1) * TILE_SIZE) enemyDirection.x *= -1.0f;
        if (enemy.position.y <= 0 || enemy.position.y >= (TILE_COUNT_Y - 1) * TILE_SIZE) enemyDirection.y *= -1.0f;

        BeginDrawing();
        ClearBackground(Color{40, 40, 40, 255});

        for (int y = 0; y < TILE_COUNT_Y; y++) {
            for (int x = 0; x < TILE_COUNT_X; x++) {
                Color tileColor = ((x + y) % 2 == 0) ? DARKGRAY : LIGHTGRAY;
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
                DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GRAY);
            }
        }

        for (int i = 0; i < itemCount; i++) {
            if (!items[i].collected) {
                DrawCircle(items[i].position.x + TILE_SIZE / 2, items[i].position.y + TILE_SIZE / 2, 10, YELLOW);
            }
        }

        DrawTexture(currentTexture, logoPosition.x, logoPosition.y, WHITE);
        DrawTexture(enemy.texture, enemy.position.x, enemy.position.y, RED);
        DrawFPS(30, 30);
        DrawText(TextFormat("Score: %i", score), 10, 10, 20, RAYWHITE);

        EndDrawing();
    }

    UnloadTexture(textureW);
    UnloadTexture(textureA);
    UnloadTexture(textureS);
    UnloadTexture(textureD);
    UnloadSound(collectSound);
    UnloadSound(hitSound);
    CloseWindow();
    return 0;
}
