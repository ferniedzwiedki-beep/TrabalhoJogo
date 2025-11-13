#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct player {
    Vector2 position;
    Vector2 speed;
    int radius;
} player;

const int screenWidth = 1600;
const int screenHeight = 900;
const int floor = 750;
int obstaclePosx = screenWidth;
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

void UpdateFrame(player *p, Rectangle *obstacle);
void ResetGame(player *p, Rectangle *obstacle);
void DrawFrame(player *p, Rectangle *obstacle);
void ChangeObstacle(Rectangle *obs_1, Rectangle *obs_2, Rectangle *obs_3);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    player p = {800.f, 700.f, 10.f, 0, 50};

    Rectangle obstacleSqr = {obstaclePosx, 600, 150, 150};
    Rectangle obstacleRec = {obstaclePosx, floor - 100, 300, 100};
    Rectangle obstacleBird = {obstaclePosx, floor - 125, 200, 50};

    obstaclePtr = &obstacleSqr;

    InitWindow(screenWidth, screenHeight, "my game");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Music

    InitAudioDevice();
    musicaFundo = LoadMusicStream("sounds/background.mp3");
    somInicio = LoadSound("sounds/start_sound.wav");
    somGameOver = LoadSound("sounds/game_over.wav");
    somPulo = LoadSound("sounds/jumping.wav");
    PlaySound(somInicio);
    PlayMusicStream(musicaFundo);


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //Music
        UpdateMusicStream(musicaFundo);
        //----------------------------------------------------------------------------------

        // Update
        //----------------------------------------------------------------------------------
        if (!loseFlag)
            UpdateFrame(&p, obstaclePtr);
        else
            ResetGame(&p, obstaclePtr);
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

void UpdateFrame(player *p, Rectangle *obstacle)
{
    obstacle->x = obstaclePosx;
    if (IsKeyDown(KEY_RIGHT)) {
        if (p->position.x < screenWidth - p->radius) {
            p->position.x += p->speed.x;
        }
    }
    if (IsKeyDown(KEY_LEFT)) {
        if (p->position.x > p->radius) {
            p->position.x -= p->speed.x;
        }
    }
    if (IsKeyPressed(KEY_UP) && canJump == true) {
            p->speed.y = 250;
            p->position.y -= p->speed.y;
            canJump = false;
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
    obstaclePosx -= 10;
    if (obstaclePosx <= 0 - obstacle->width) {
        obstaclePosx = screenWidth;
        obstacleSwitch = true;
    }
    if (CheckCollisionCircleRec(p->position, p->radius, *obstacle)){
        if(!collisionFlag){
            collisionFlag = true;
            StopMusicStream(musicaFundo);
            PlaySound(somGameOver);
        }
    }
}

void ResetGame(player *p, Rectangle *obstacle)
{
    if (IsKeyPressed(KEY_R)) {
        collisionFlag = false;
        loseFlag = false;
        obstaclePosx = screenWidth;
        obstacle->x = obstaclePosx;
        p->position.x = 800;
        p->position.y = 700;
        PlayMusicStream(musicaFundo);
    }
}

void DrawFrame(player *p, Rectangle *obstacle)
{
    ClearBackground(PURPLE);
    DrawRectangle(0, floor, screenWidth, 200, BLUE);
    DrawCircle(p->position.x, p->position.y, p->radius, WHITE);
    DrawRectangleRec(*obstacle, RED);
    if (collisionFlag) {
        DrawText("YOU LOSE!\nPRESS R TO RESTART", 600, 250, 50, PINK);
        loseFlag = true;
    }
}

void ChangeObstacle(Rectangle *obs_1, Rectangle *obs_2, Rectangle *obs_3)
{
    int random = 0;
    SetRandomSeed((int) GetTime());
    if (obstacleSwitch) {
        random = GetRandomValue(0, 90);
        if (random <= 30) {
            obstaclePtr = obs_1;
        } else if (random > 30 && random <= 60) {
            obstaclePtr = obs_2;
        } else obstaclePtr = obs_3;
        obstacleSwitch = false;
    }
}
