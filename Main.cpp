#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>

constexpr int TILE_SIZE = 40;
constexpr int TILE_COUNT_X = 20;
constexpr int TILE_COUNT_Y = 15;
constexpr int MAX_PARTICLES = 100;

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, SHOP };
enum class PlayerState { NORMAL, SPEED_BOOST, INVINCIBLE, FROZEN };

class GameObject {
protected:
    Vector2 position;
    Texture2D texture;
public:
    virtual void Update() = 0;
    virtual void Draw() const = 0;
    virtual ~GameObject() = default;
};

class ParticleSystem {
    struct Particle {
        Vector2 position;
        Color color;
        float alpha;
        float size;
        Vector2 velocity;
    };

    std::vector<Particle> particles;
public:
    void Spawn(Vector2 position, int count) {
        for(int i = 0; i < count; i++) {
            particles.push_back({
                position,
                Color{static_cast<unsigned char>(GetRandomValue(200,255)),
                      static_cast<unsigned char>(GetRandomValue(100,200)), 0, 255},
                1.0f,
                static_cast<float>(GetRandomValue(2,8)),
                Vector2{static_cast<float>(GetRandomValue(-50,50))/10.0f,
                        static_cast<float>(GetRandomValue(-50,50))/10.0f}
            });
        }
    }

    void Update() {
        particles.erase(std::remove_if(particles.begin(), particles.end(), 
            [](const Particle& p) { return p.alpha <= 0; }), particles.end());

        for(auto& p : particles) {
            p.position = Vector2Add(p.position, p.velocity);
            p.alpha -= 0.02f;
            p.size *= 0.98f;
        }
    }

    void Draw() const {
        for(const auto& p : particles) {
            DrawCircleV(p.position, p.size, Fade(p.color, p.alpha));
        }
    }
};

class Player : public GameObject {
    Rectangle frameRec;
    int currentFrame = 0;
    int framesCounter = 0;
    PlayerState state = PlayerState::NORMAL;
    float speed = 2.0f;
    float powerUpTimer = 0.0f;
    int coins = 0;
    int score = 0;
    int lives = 3;

public:
    Player(Vector2 startPos, Texture2D tex) {
        position = startPos;
        texture = tex;
        frameRec = {0, 0, 32, 32};
    }

    void Update() override {
        if(IsKeyDown(KEY_D)) position.x += speed;
        if(IsKeyDown(KEY_A)) position.x -= speed;
        if(IsKeyDown(KEY_W)) position.y -= speed;
        if(IsKeyDown(KEY_S)) position.y += speed;

        // Animation logic
        if(++framesCounter >= 8) {
            currentFrame = (currentFrame + 1) % 4;
            frameRec.x = currentFrame * 32;
            framesCounter = 0;
        }

        // Power-up timer
        if(powerUpTimer > 0) powerUpTimer -= GetFrameTime();
    }

    void Draw() const override {
        DrawTextureRec(texture, frameRec, position, WHITE);
    }

    void ApplyPowerUp(PlayerState newState, float duration) {
        state = newState;
        powerUpTimer = duration;
        // Apply effects based on state...
    }

    Vector2 GetPosition() const {
        return position;
    }

    int GetScore() const {
        return score;
    }

    int GetLives() const {
        return lives;
    }

    void Reset() {
        position = {400, 300};
        state = PlayerState::NORMAL;
        speed = 2.0f;
        powerUpTimer = 0.0f;
        coins = 0;
        score = 0;
        lives = 3;
    }
};

class Enemy : public GameObject {
protected:
    Vector2 direction;
    float speed;
public:
    virtual void AIUpdate(Vector2 playerPos) = 0;
};

class PatrolEnemy : public Enemy {
public:
    PatrolEnemy(Vector2 startPos, Texture2D tex) {
        position = startPos;
        texture = tex;
        direction = {1, 0};
        speed = 1.5f;
    }

    void Update() override {
        position.x += direction.x * speed;
        if(position.x < 40 || position.x > 760) direction.x *= -1;
    }

    void AIUpdate(Vector2 playerPos) override {
        // Simple patrol AI
    }

    void Draw() const override {
        DrawTextureV(texture, position, WHITE);
    }
};

class ChasingEnemy : public Enemy {
public:
    ChasingEnemy(Vector2 startPos, Texture2D tex) {
        position = startPos;
        texture = tex;
        speed = 2.0f;
    }

    void Update() override {
        // Movement handled in AIUpdate
    }

    void AIUpdate(Vector2 playerPos) override {
        Vector2 dir = Vector2Normalize(Vector2Subtract(playerPos, position));
        position = Vector2Add(position, Vector2Scale(dir, speed));
    }

    void Draw() const override {
        DrawTextureV(texture, position, RED);
    }
};

class Game {
    GameState state = GameState::MENU;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    ParticleSystem particles;
    Sound collectSound;
    Music bgMusic;
    int highScore = 0;
    float gameTime = 120.0f;

    void LoadGameData() {
        std::ifstream saveFile("save.dat");
        if(saveFile) saveFile >> highScore;
    }

public:
    Game() {
        InitWindow(800, 600, "Ultimate Pacman");
        InitAudioDevice();

        Texture2D playerTex = LoadTexture("assets/4.png");
        player = std::make_unique<Player>(Vector2{400, 300}, playerTex);

        enemies.push_back(std::make_unique<PatrolEnemy>(
            Vector2{200, 200}, LoadTexture("assets/blueghost.png")));
        enemies.push_back(std::make_unique<ChasingEnemy>(
            Vector2{600, 400}, LoadTexture("assets/sefidghost.png")));

        collectSound = LoadSound("assets/collect.wav");
        bgMusic = LoadMusicStream("assets/bg_music.mp3");
        LoadGameData();
    }

    ~Game() {
        UnloadSound(collectSound);
        UnloadMusicStream(bgMusic);
        CloseAudioDevice();
        CloseWindow();
    }

    void Run() {
        SetTargetFPS(60);
        PlayMusicStream(bgMusic);

        while (!WindowShouldClose()) {
            UpdateMusicStream(bgMusic);
            Update();
            Draw();
        }
    }

private:
    void Update() {
        switch(state) {
            case GameState::MENU:
                if(IsKeyPressed(KEY_ENTER)) {
                    state = GameState::PLAYING;
                    gameTime = 120.0f;
                    player->Reset();
                }
                break;

            case GameState::PLAYING:
                player->Update();
                for(auto& enemy : enemies) {
                    enemy->AIUpdate(player->GetPosition());
                    enemy->Update();
                }
                // Collision detection and other logic...
                break;

            // Handle other states...
        }
    }

    void Draw() const {
        BeginDrawing();
        ClearBackground(DARKBLUE);

        // Draw game world
        for(int x = 0; x < TILE_COUNT_X; x++) {
            for(int y = 0; y < TILE_COUNT_Y; y++) {
                if(x == 0 || x == TILE_COUNT_X-1 || y == 0 || y == TILE_COUNT_Y-1) {
                    DrawRectangle(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
                }
            }
        }

        player->Draw();
        for(const auto& enemy : enemies) enemy->Draw();
        particles.Draw();

        // Draw HUD
        DrawText(TextFormat("SCORE: %d", player->GetScore()), 20, 10, 20, YELLOW);
        DrawText(TextFormat("LIVES: %d", player->GetLives()), 200, 10, 20, RED);
        DrawText(TextFormat("TIME: %02d:%02d", 
            static_cast<int>(gameTime)/60, static_cast<int>(gameTime)%60), 
            400, 10, 20, SKYBLUE);

        EndDrawing();
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}
