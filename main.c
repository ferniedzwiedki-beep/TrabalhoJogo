#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define MIN_OBS_SPEED 25
#define DEFAULT_PLAYER_POS_X 500
#define DEFAULT_PLAYER_POS_Y 700
#define PLAYER_RADIUS 50
#define PLAYER_SPRITE_WIDTH_HEIGHT 225
#define GROUND_END -200

typedef struct Player {
  Vector2 position;
  Vector2 speed;
  int radius;
} Player;

typedef struct Score {
  Vector2 position;
  int score;
} Score;

typedef struct Obstacle {
  Rectangle* recPtr;
  Texture2D* TexturePtr;
} Obstacle;

const int screenWidth = 1600;
const int screenHeight = 900;
const int ground = screenHeight - 150;
const int groundSpeed = 5;
int groundPosx = 0;
int obstaclePosx = screenWidth;
int obstacleSpeed = MIN_OBS_SPEED;
int timePassed = 0;
const float gravity = 0.5f;
bool gameStart = false;
bool canJump = true;
bool isCrouched = false;
bool collisionFlag = false;
bool loseFlag = false;
bool obstacleSwitch = false;
Obstacle obstacle;
Music musicaFundo;
Sound somInicio;
Sound somGameOver;
Sound somPulo;
Texture2D playerTextureStanding;
Texture2D playerTextureCrouch;
Texture2D playerTextureFalling;
Texture2D playerTextureJumping;
Texture2D playerTextureMov;
Texture2D playerTextureCrouchMov;
Texture2D groundTexture;
Texture2D background;
Texture2D obstacleBirdTexture;
Texture2D obstacleSqrTexture;
Texture2D obstacleRecTexture;
Image playerImageStanding;
Image playerImageCrouch;
Image playerImageFalling;
Image playerImageJumping;
Image playerImageMov;
Image playerImageCrouchMov;
Image groundImage;
Image obstacleBirdImage;
Image obstacleSqrImage;
Image obstacleRecImage;
Score pontos = {1200, 100, 0};
Score highScore = {100, 100, 0};

void UpdateFrame(Player* p, Rectangle* obstacle);
void ResetGame(Player* p, Rectangle* obstacle);
void DrawFrame(Player* p, Obstacle obs);
void ChangeObstacle(Rectangle* obs_1, Rectangle* obs_2, Rectangle* obs_3);
void GetTexture(Image sprite, int newWidth, int newHeight, Texture* texture,
                char* fileName);
void InitAudio();
void InitGame();
void InitTextures();
void UnloadAudio();
void UnloadImages();
void UnloadTextures();

int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  Player p = {DEFAULT_PLAYER_POS_X, DEFAULT_PLAYER_POS_Y, 0, 0, PLAYER_RADIUS};

  Rectangle obstacleSqr = {obstaclePosx, ground - 125, 125, 125};
  Rectangle obstacleRec = {obstaclePosx, ground - 100, 300, 100};
  Rectangle obstacleBird = {obstaclePosx, ground - 150, 250, 75};
  obstacle.recPtr = &obstacleSqr;
  obstacle.TexturePtr = &obstacleSqrTexture;

  InitWindow(screenWidth, screenHeight, "my game");
  SetRandomSeed(time(NULL));
  InitTextures();
  InitAudioDevice();
  InitAudio();

  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    // Update
    //---------------------------------------------------------------------------------
    if (!gameStart) {
      InitGame();
    } else if (!loseFlag) {
      UpdateFrame(&p, obstacle.recPtr);
    } else
      ResetGame(&p, obstacle.recPtr);
    if (obstacleSwitch)
      ChangeObstacle(&obstacleSqr, &obstacleRec, &obstacleBird);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    DrawFrame(&p, obstacle);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadAudio();
  UnloadTextures();
  UnloadImages();
  CloseAudioDevice();
  CloseWindow();  // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void InitGame() {
  if (IsKeyPressed(KEY_SPACE)) {
    gameStart = true;
    PlaySound(somInicio);
    PlayMusicStream(musicaFundo);
  }
}

void InitAudio() {
  musicaFundo = LoadMusicStream("audio/background.mp3");
  somInicio = LoadSound("audio/start_sound.wav");
  somGameOver = LoadSound("audio/game_over.wav");
  somPulo = LoadSound("audio/jumping.wav");
}

void GetTexture(Image sprite, int newWidth, int newHeight, Texture* texture,
                char* fileName) {
  sprite = LoadImage(fileName);
  ImageResize(&sprite, newWidth, newHeight);
  *texture = LoadTextureFromImage(sprite);
}

void InitTextures() {
  GetTexture(playerImageStanding, PLAYER_SPRITE_WIDTH_HEIGHT,
             PLAYER_SPRITE_WIDTH_HEIGHT, &playerTextureStanding,
             "resources/sprite.png");
  GetTexture(playerImageJumping, PLAYER_SPRITE_WIDTH_HEIGHT,
             PLAYER_SPRITE_WIDTH_HEIGHT, &playerTextureJumping,
             "resources/sprite_pulando.png");
  GetTexture(playerImageCrouch, PLAYER_SPRITE_WIDTH_HEIGHT,
             PLAYER_SPRITE_WIDTH_HEIGHT, &playerTextureCrouch,
             "resources/sprite_agachado.png");
  GetTexture(playerImageCrouchMov, PLAYER_SPRITE_WIDTH_HEIGHT,
             PLAYER_SPRITE_WIDTH_HEIGHT, &playerTextureCrouchMov,
             "resources/sprite_agachado_movimento.png");
  GetTexture(playerImageMov, PLAYER_SPRITE_WIDTH_HEIGHT,
             PLAYER_SPRITE_WIDTH_HEIGHT, &playerTextureMov,
             "resources/sprite_movimento.png");
  GetTexture(playerImageFalling, PLAYER_SPRITE_WIDTH_HEIGHT,
             PLAYER_SPRITE_WIDTH_HEIGHT, &playerTextureFalling,
             "resources/sprite_caindo.png");
  GetTexture(groundImage, 2000, 150, &groundTexture,
             "resources/sprite_chao.jpeg");
  GetTexture(obstacleBirdImage, 250, 150, &obstacleBirdTexture,
             "resources/berdly.png");
  obstacleSqrTexture = LoadTexture("resources/arvore.png");
  GetTexture(obstacleRecImage, 400, 300, &obstacleRecTexture,
             "resources/duas_arvores.png");
  background = LoadTexture("resources/sprite_background.jpeg");
}

void UnloadAudio() {
  UnloadMusicStream(musicaFundo);
  UnloadSound(somInicio);
  UnloadSound(somGameOver);
  UnloadSound(somPulo);
}
void UnloadImages() {
  UnloadImage(playerImageStanding);
  UnloadImage(playerImageCrouch);
  UnloadImage(playerImageCrouchMov);
  UnloadImage(playerImageJumping);
  UnloadImage(playerImageFalling);
  UnloadImage(playerImageMov);
  UnloadImage(obstacleBirdImage);
  UnloadImage(obstacleSqrImage);
  UnloadImage(obstacleRecImage);
}
void UnloadTextures() {
  UnloadTexture(playerTextureStanding);
  UnloadTexture(playerTextureCrouch);
  UnloadTexture(playerTextureCrouchMov);
  UnloadTexture(playerTextureJumping);
  UnloadTexture(playerTextureFalling);
  UnloadTexture(playerTextureMov);
  UnloadTexture(groundTexture);
  UnloadTexture(background);
  UnloadTexture(obstacleBirdTexture);
  UnloadTexture(obstacleSqrTexture);
  UnloadTexture(obstacleRecTexture);
}

void UpdateFrame(Player* p, Rectangle* obstacle) {
  UpdateMusicStream(musicaFundo);
  groundPosx -= groundSpeed;
  if (groundPosx <= GROUND_END) groundPosx = 0;
  obstacle->x = obstaclePosx;
  timePassed += 1;
  pontos.score += 1;
  if (timePassed % 300 == 0) {
    obstacleSpeed += 2;
  }
  if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE)) && canJump == true) {
    canJump = false;
    p->speed.y = 250;
    p->position.y -= p->speed.y;
    p->speed.y = 0;
    PlaySound(somPulo);
  } else {
    p->speed.y += gravity;
    p->position.y += p->speed.y;
  }
  if (IsKeyDown(KEY_DOWN) && canJump == true) {
    p->radius = PLAYER_RADIUS / 2;
    isCrouched = true;
  } else {
    p->radius = PLAYER_RADIUS;
    isCrouched = false;
  }
  if (p->position.y > ground - p->radius) {
    p->speed.y = 0;
    p->position.y = ground - p->radius;
    canJump = true;
  }
  obstaclePosx -= obstacleSpeed;
  if (obstaclePosx <= -75 - obstacle->width) {
    obstaclePosx = screenWidth;
    obstacleSwitch = true;
  }
  if (CheckCollisionCircleRec(p->position, p->radius, *obstacle)) {
    collisionFlag = true;
    StopMusicStream(musicaFundo);
    PlaySound(somGameOver);
  }
}

void ResetGame(Player* p, Rectangle* obstacle) {
  if (IsKeyPressed(KEY_R)) {
    collisionFlag = false;
    loseFlag = false;
    obstacleSwitch = true;
    obstaclePosx = screenWidth;
    obstacle->x = obstaclePosx;
    obstacleSpeed = MIN_OBS_SPEED;
    timePassed = 0;
    if (highScore.score < pontos.score) {
      highScore.score = pontos.score;
    }
    pontos.score = 0;
    p->position.x = DEFAULT_PLAYER_POS_X;
    p->position.y = DEFAULT_PLAYER_POS_Y;
    PlayMusicStream(musicaFundo);
  }
}

void DrawFrame(Player* p, Obstacle obs) {
  ClearBackground(SKYBLUE);
  DrawTexture(background, 0, 0, WHITE);
  DrawTexture(groundTexture, groundPosx + GROUND_END, ground, WHITE);
  if (!gameStart)
    DrawText("PRESS SPACE TO START!", 500, 250, 50, PINK);
  else {
    if (!isCrouched) {
      if (canJump) {
        if (timePassed % 8 == 0) {
          DrawTexture(playerTextureMov, p->position.x - p->radius * 2,
                      p->position.y - p->radius * 2.5, WHITE);
        } else
          DrawTexture(playerTextureStanding, p->position.x - p->radius * 2,
                      p->position.y - p->radius * 2.5, WHITE);
      } else if (p->speed.y < 5)
        DrawTexture(playerTextureJumping, p->position.x - p->radius * 2,
                    p->position.y - p->radius * 2.5, WHITE);
      else if (p->speed.y < 10)
        DrawTexture(playerTextureMov, p->position.x - p->radius * 2,
                    p->position.y - p->radius * 2.5, WHITE);
      else
        DrawTexture(playerTextureFalling, p->position.x - p->radius * 2,
                    p->position.y - p->radius * 2.5, WHITE);
    } else if (timePassed % 6) {
      DrawTexture(playerTextureCrouchMov, p->position.x - p->radius * 3.5,
                  p->position.y - p->radius * 6.25, WHITE);
    } else
      DrawTexture(playerTextureCrouch, p->position.x - p->radius * 3.5,
                  p->position.y - p->radius * 6.25, WHITE);
    if (obs.TexturePtr == &obstacleBirdTexture) {
      DrawTexture(*obs.TexturePtr, obs.recPtr->x, obs.recPtr->y - 20, WHITE);
    } else if (obs.TexturePtr == &obstacleSqrTexture) {
      DrawTexture(*obs.TexturePtr, obs.recPtr->x - 25, obs.recPtr->y - 75,
                  WHITE);
    } else {
      DrawTexture(*obs.TexturePtr, obs.recPtr->x, obs.recPtr->y - 200, WHITE);
    }
    DrawText(TextFormat("SCORE: %d", pontos.score), pontos.position.x,
             pontos.position.y, 50, BLACK);
    DrawText(TextFormat("HIGH SCORE: %d", highScore.score),
             highScore.position.x, highScore.position.y, 50, BLACK);
  }
  if (collisionFlag) {
    DrawText("YOU LOSE!\nPRESS R TO RESTART", 600, 250, 50, PINK);
    loseFlag = true;
  }
}

void ChangeObstacle(Rectangle* obsSqr, Rectangle* obsRec, Rectangle* obsBird) {
  int random = GetRandomValue(0, 90);
  if (random <= 30) {
    obstacle.recPtr = obsSqr;
    obstacle.TexturePtr = &obstacleSqrTexture;
  } else if (random > 30 && random <= 60) {
    obstacle.recPtr = obsRec;
    obstacle.TexturePtr = &obstacleRecTexture;
  } else {
    obstacle.recPtr = obsBird;
    obstacle.TexturePtr = &obstacleBirdTexture;
  }
  obstacleSwitch = false;
}