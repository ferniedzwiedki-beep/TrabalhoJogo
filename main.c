#include <raylib.h>
#include <stdlib.h>
#include <time.h>

typedef struct Player {
    Vector2 position;
    Vector2 speed;
    int radius;
} Player;

typedef struct Score {
    Vector2 position;
    int score;
} Score;

const int screenWidth = 1600;
const int screenHeight = 900;
const int floor = 750;
int obstaclePosx = screenWidth;
int obstacleSpeed = 0;
int timePassed = 0;
const float gravity = 0.5f;
bool canJump = true;
bool collisionFlag = false;
bool loseFlag = false;
bool obstacleSwitch = false;
Rectangle *obstaclePtr = NULL;
Music musicaFundo;
Sound somInicio;
Sound somGameOver;
Sound somPulo;
Score pontos = {1200, 100, 0};
Score highScore = {100, 100, 0};

void UpdateFrame(Player *p, Rectangle *obstacle);
void ResetGame(Player *p, Rectangle *obstacle);
void DrawFrame(Player *p, Rectangle *obstacle);
void ChangeObstacle(Rectangle *obs_1, Rectangle *obs_2, Rectangle *obs_3);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    Player p = {500.f, 700.f, 10.f, 0, 50};

    Rectangle obstacleSqr = {obstaclePosx, floor - 125, 125, 125};
    Rectangle obstacleRec = {obstaclePosx, floor - 100, 300, 100};
    Rectangle obstacleBird = {obstaclePosx, floor - 125, 200, 50};

    obstaclePtr = &obstacleSqr;
    obstacleSpeed = 25;

    InitWindow(screenWidth, screenHeight, "my game");
    SetRandomSeed(time(NULL));
    InitAudioDevice();
    musicaFundo = LoadMusicStream("sounds/background.mp3");
    somInicio = LoadSound("sounds/start_sound.wav");
    somGameOver = LoadSound("sounds/game_over.wav");
    somPulo = LoadSound("sounds/jumping.wav");
    PlaySound(somInicio);
    PlayMusicStream(musicaFundo);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (!loseFlag) 
            UpdateFrame(&p, obstaclePtr);
        else 
            ResetGame(&p, obstaclePtr);
        if (obstacleSwitch)
            ChangeObstacle(&obstacleSqr, &obstacleRec, &obstacleBird);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            DrawFrame(&p, obstaclePtr);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    //Ending Music
        //----------------------------------------------------------------------------------
        UnloadMusicStream(musicaFundo);
        UnloadSound(somInicio);
        UnloadSound(somGameOver);
        UnloadSound(somPulo);
        CloseAudioDevice();
        //----------------------------------------------------------------------------------


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL obstacleWidthext
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdateFrame(Player *p, Rectangle *obstacle)
{
    obstacle->x = obstaclePosx;
    timePassed += 1;
    pontos.score += 1;
    if (timePassed % 600 == 0) {
        obstacleSpeed += 1;
    }
    if (IsKeyPressed(KEY_UP) && canJump == true) {
        canJump = false; 
        p->speed.y = 250;
        p->position.y -= p->speed.y;   
        p->speed.y = 0;
        PlaySound(somPulo);
    } else {
        p->speed.y += gravity;
        p->position.y += p->speed.y;
    }
    if (IsKeyDown(KEY_DOWN) && canJump == true)
        p->radius = 25;
    else p->radius = 50;
    
    if (p->position.y > floor - p->radius) {
        p->speed.y = 0;
        p->position.y = floor - p->radius;
        canJump = true;
    }
    obstaclePosx -= obstacleSpeed;
    if (obstaclePosx <= 0 - obstacle->width) {
        obstaclePosx = screenWidth;
        obstacleSwitch = true;
    }
    if (CheckCollisionCircleRec(p->position, p->radius, *obstacle)) {
        collisionFlag = true;
        StopMusicStream(musicaFundo);
        PlaySound(somGameOver);
    }
}

void ResetGame(Player *p, Rectangle *obstacle)
{
    if (IsKeyPressed(KEY_R)) {
        collisionFlag = false;
        loseFlag = false;
        obstaclePosx = screenWidth;
        obstacle->x = obstaclePosx;
        obstacleSpeed = 25;
        timePassed = 0;
        if (highScore.score < pontos.score) {
            highScore.score = pontos.score;
        }
        pontos.score = 0;
        p->position.x = 500;
        p->position.y = 700;
        PlayMusicStream(musicaFundo);
    }
}

void DrawFrame(Player *p, Rectangle *obstacle)
{
    ClearBackground(PURPLE);           
    DrawRectangle(0, floor, screenWidth, 200, BLUE);
    DrawCircle(p->position.x, p->position.y, p->radius, WHITE);
    DrawRectangleRec(*obstacle, RED);
    DrawText(TextFormat("SCORE: %d", pontos.score), pontos.position.x, pontos.position.y, 50, PINK);
    DrawText(TextFormat("HIGH SCORE: %d", highScore.score), highScore.position.x, highScore.position.y, 50, PINK);
    if (collisionFlag) {
        DrawText("YOU LOSE!\nPRESS R TO RESTART", 600, 250, 50, PINK);
        loseFlag = true;
    }
}

void ChangeObstacle(Rectangle *obs_1, Rectangle *obs_2, Rectangle *obs_3)
{
    int random = GetRandomValue(0, 90);
    if (random <= 30)
        obstaclePtr = obs_1;
    else if (random > 30 && random <= 60)
        obstaclePtr = obs_2;
    else obstaclePtr = obs_3;
    obstacleSwitch = false;
}