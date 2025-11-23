#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define OBS_OFFSET -75
#define MIN_OBS_SPEED 25
#define DEFAULT_PLAYER_POS_X 500
#define DEFAULT_PLAYER_POS_Y 700
#define PLAYER_RADIUS 50
#define JUMP_HEIGHT 250
#define PLAYER_SPRITE_WIDTH_HEIGHT 225
#define DEFAULT_SPRITE_OFFSET_X 2
#define DEFAULT_SPRITE_OFFSET_Y 2.5f
#define SPRITE_CROUCH_OFFSET_X 3.5f
#define SPRITE_CROUCH_OFFSET_Y 6.25f
#define GROUND_END -200

typedef struct PlayerState {
  bool isGrounded;
  bool isCrouched;
} PlayerState;

typedef struct Player {
  Vector2 position;
  Vector2 speed;
  int radius;
  PlayerState state;
} Player;

typedef struct Score {
  Vector2 position;
  int score;
} Score;

typedef struct Obstacle {
  Rectangle* recPtr;
  Texture2D* texturePtr;
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
Texture2D backgroundTexture;
Texture2D obstacleBirdTexture;
Texture2D obstacleSqrTexture;
Texture2D obstacleRecTexture;
Score pontos = {1200, 100, 0};
Score highScore = {100, 100, 0};

void UpdateFrame(Player* p, Rectangle* obstacle);
void ResetGame(Player* p, Rectangle* obstacle);
void DrawFrame(Player* p, Obstacle obs);
void HandleInput(Player* p);
void UpdateObstacle(Rectangle* obstacle);
void UpdateScore();
void UpdateGravity(Player* p);
void UpdateGround();
void ResetPlayer(Player* p);
void ResetObstacle(Rectangle* obstacle);
void ResetScore();
void DrawScenery();
void DrawPlayer(Player* p);
void DrawObstacle(Obstacle obs);
void DrawHUD();
void HandleObstacleCollision(Player* p, Rectangle* obstacle);
void ChangeObstacle(Rectangle* obs_1, Rectangle* obs_2, Rectangle* obs_3);
void GetTexture(int newWidth, int newHeight, Texture* sprite, char* fileName);
void InitAudio();
void InitGame();
void InitTextures();
void UnloadAudio();
void UnloadTextures();

int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  Player p = {DEFAULT_PLAYER_POS_X,
              DEFAULT_PLAYER_POS_Y,
              0,
              0,
              PLAYER_RADIUS,
              true,
              false};

  Rectangle obstacleSqr = {obstaclePosx, ground - 125, 125, 125};
  Rectangle obstacleRec = {obstaclePosx, ground - 100, 300, 100};
  Rectangle obstacleBird = {obstaclePosx, ground - 150, 250, 75};
  obstacle.recPtr = &obstacleSqr;
  obstacle.texturePtr = &obstacleSqrTexture;

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
  CloseAudioDevice();
  CloseWindow();
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

void GetTexture(int newWidth, int newHeight, Texture* sprite, char* fileName) {
  Image img = LoadImage(fileName);
  ImageResize(&img, newWidth, newHeight);
  *sprite = LoadTextureFromImage(img);
  UnloadImage(img);
}

void InitTextures() {
  GetTexture(PLAYER_SPRITE_WIDTH_HEIGHT, PLAYER_SPRITE_WIDTH_HEIGHT,
             &playerTextureStanding, "resources/sprite.png");
  GetTexture(PLAYER_SPRITE_WIDTH_HEIGHT, PLAYER_SPRITE_WIDTH_HEIGHT,
             &playerTextureJumping, "resources/sprite_pulando.png");
  GetTexture(PLAYER_SPRITE_WIDTH_HEIGHT, PLAYER_SPRITE_WIDTH_HEIGHT,
             &playerTextureCrouch, "resources/sprite_agachado.png");
  GetTexture(PLAYER_SPRITE_WIDTH_HEIGHT, PLAYER_SPRITE_WIDTH_HEIGHT,
             &playerTextureCrouchMov,
             "resources/sprite_agachado_movimento.png");
  GetTexture(PLAYER_SPRITE_WIDTH_HEIGHT, PLAYER_SPRITE_WIDTH_HEIGHT,
             &playerTextureMov, "resources/sprite_movimento.png");
  GetTexture(PLAYER_SPRITE_WIDTH_HEIGHT, PLAYER_SPRITE_WIDTH_HEIGHT,
             &playerTextureFalling, "resources/sprite_caindo.png");
  GetTexture(2000, 150, &groundTexture, "resources/sprite_chao.jpeg");
  GetTexture(250, 150, &obstacleBirdTexture, "resources/berdly.png");
  GetTexture(200, 200, &obstacleSqrTexture, "resources/arvore.png");
  GetTexture(400, 300, &obstacleRecTexture, "resources/duas_arvores.png");
  GetTexture(screenWidth, screenHeight, &backgroundTexture,
             "resources/sprite_background.jpeg");
}

void UpdateFrame(Player* p, Rectangle* obstacle) {
  UpdateGround();
  UpdateMusicStream(musicaFundo);
  UpdateScore();
  UpdateObstacle(obstacle);
  HandleInput(p);
  UpdateGravity(p);
  HandleObstacleCollision(p, obstacle);
}

void HandleInput(Player* p) {
  if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE)) &&
      p->state.isGrounded == true) {
    p->state.isGrounded = false;
    p->speed.y = JUMP_HEIGHT;
    p->position.y -= p->speed.y;
    p->speed.y = 0;
    PlaySound(somPulo);
  }
  if (IsKeyDown(KEY_DOWN) && p->state.isGrounded == true) {
    p->radius = PLAYER_RADIUS / 2;
    p->state.isCrouched = true;
  } else {
    p->radius = PLAYER_RADIUS;
    p->state.isCrouched = false;
  }
}

void UpdateGravity(Player* p) {
  p->speed.y += gravity;
  p->position.y += p->speed.y;
  if (p->position.y > ground - p->radius) {
    p->speed.y = 0;
    p->position.y = ground - p->radius;
    p->state.isGrounded = true;
  }
}

void UpdateGround() {
  groundPosx -= groundSpeed;
  if (groundPosx <= GROUND_END) groundPosx = 0;
}

void UpdateObstacle(Rectangle* obstacle) {
  obstacle->x = obstaclePosx;
  obstaclePosx -= obstacleSpeed;
  if (obstaclePosx <= OBS_OFFSET - obstacle->width) {
    obstaclePosx = screenWidth;
    obstacleSwitch = true;
  }
  if (timePassed % 300 == 0) {
    if (obstacleSpeed < MIN_OBS_SPEED * 2) obstacleSpeed += 2;
  }
}

void UpdateScore() {
  timePassed += 1;
  pontos.score += 1;
}

void HandleObstacleCollision(Player* p, Rectangle* obstacle) {
  if (CheckCollisionCircleRec(p->position, p->radius, *obstacle)) {
    loseFlag = true;
    StopMusicStream(musicaFundo);
    PlaySound(somGameOver);
  }
}

void ResetGame(Player* p, Rectangle* obstacle) {
  if (IsKeyPressed(KEY_R)) {
    ResetPlayer(p);
    ResetObstacle(obstacle);
    ResetScore();
    PlayMusicStream(musicaFundo);
    loseFlag = false;
  }
}

void ResetPlayer(Player* p) {
  p->position.x = DEFAULT_PLAYER_POS_X;
  p->position.y = DEFAULT_PLAYER_POS_Y;
}

void ResetObstacle(Rectangle* obstacle) {
  obstacleSwitch = true;
  obstaclePosx = screenWidth;
  obstacle->x = obstaclePosx;
  obstacleSpeed = MIN_OBS_SPEED;
}
void ResetScore() {
  timePassed = 0;
  if (highScore.score < pontos.score) {
    highScore.score = pontos.score;
  }
  pontos.score = 0;
}

void ChangeObstacle(Rectangle* obsSqr, Rectangle* obsRec, Rectangle* obsBird) {
  int random = GetRandomValue(0, 90);
  if (random <= 30) {
    obstacle.recPtr = obsSqr;
    obstacle.texturePtr = &obstacleSqrTexture;
  } else if (random > 30 && random <= 60) {
    obstacle.recPtr = obsRec;
    obstacle.texturePtr = &obstacleRecTexture;
  } else {
    obstacle.recPtr = obsBird;
    obstacle.texturePtr = &obstacleBirdTexture;
  }
  obstacleSwitch = false;
}

void DrawFrame(Player* p, Obstacle obs) {
  DrawScenery();
  DrawHUD();
  if (gameStart) {
    DrawPlayer(p);
    DrawObstacle(obs);
  }
}

void DrawScenery() {
  ClearBackground(SKYBLUE);
  DrawTexture(backgroundTexture, 0, 0, WHITE);
  DrawTexture(groundTexture, groundPosx + GROUND_END, ground, WHITE);
}

void DrawPlayer(Player* p) {
  if (p->state.isCrouched) {
    if (timePassed % 6) {
      DrawTexture(playerTextureCrouchMov,
                  p->position.x - p->radius * SPRITE_CROUCH_OFFSET_X,
                  p->position.y - p->radius * SPRITE_CROUCH_OFFSET_Y, WHITE);
    } else
      DrawTexture(playerTextureCrouch,
                  p->position.x - p->radius * SPRITE_CROUCH_OFFSET_X,
                  p->position.y - p->radius * SPRITE_CROUCH_OFFSET_Y, WHITE);
  } else if (p->state.isGrounded) {
    if (timePassed % 8 == 0) {
      DrawTexture(playerTextureMov,
                  p->position.x - p->radius * DEFAULT_SPRITE_OFFSET_X,
                  p->position.y - p->radius * DEFAULT_SPRITE_OFFSET_Y, WHITE);
    } else
      DrawTexture(playerTextureStanding,
                  p->position.x - p->radius * DEFAULT_SPRITE_OFFSET_X,
                  p->position.y - p->radius * DEFAULT_SPRITE_OFFSET_Y, WHITE);
  } else if (p->speed.y < 5)
    DrawTexture(playerTextureJumping,
                p->position.x - p->radius * DEFAULT_SPRITE_OFFSET_X,
                p->position.y - p->radius * DEFAULT_SPRITE_OFFSET_Y, WHITE);
  else if (p->speed.y < 10)
    DrawTexture(playerTextureMov,
                p->position.x - p->radius * DEFAULT_SPRITE_OFFSET_X,
                p->position.y - p->radius * DEFAULT_SPRITE_OFFSET_Y, WHITE);
  else
    DrawTexture(playerTextureFalling,
                p->position.x - p->radius * DEFAULT_SPRITE_OFFSET_X,
                p->position.y - p->radius * DEFAULT_SPRITE_OFFSET_Y, WHITE);
}

void DrawObstacle(Obstacle obs) {
  if (obs.texturePtr == &obstacleBirdTexture) {
    DrawTexture(*obs.texturePtr, obs.recPtr->x, obs.recPtr->y - 20, WHITE);
  } else if (obs.texturePtr == &obstacleSqrTexture) {
    DrawTexture(*obs.texturePtr, obs.recPtr->x - 25, obs.recPtr->y - 75, WHITE);
  } else {
    DrawTexture(*obs.texturePtr, obs.recPtr->x, obs.recPtr->y - 200, WHITE);
  }
}

void DrawHUD() {
  if (gameStart) {
    DrawText(TextFormat("SCORE: %d", pontos.score), pontos.position.x,
             pontos.position.y, 50, BLACK);
    DrawText(TextFormat("HIGH SCORE: %d", highScore.score),
             highScore.position.x, highScore.position.y, 50, BLACK);
  } else
    DrawText("PRESS SPACE TO START!", 500, 250, 50, PINK);
  if (loseFlag) {
    DrawText("YOU LOSE!\nPRESS R TO RESTART", 600, 250, 50, PINK);
  }
}

void UnloadAudio() {
  UnloadMusicStream(musicaFundo);
  UnloadSound(somInicio);
  UnloadSound(somGameOver);
  UnloadSound(somPulo);
}

void UnloadTextures() {
  UnloadTexture(playerTextureStanding);
  UnloadTexture(playerTextureCrouch);
  UnloadTexture(playerTextureCrouchMov);
  UnloadTexture(playerTextureJumping);
  UnloadTexture(playerTextureFalling);
  UnloadTexture(playerTextureMov);
  UnloadTexture(groundTexture);
  UnloadTexture(backgroundTexture);
  UnloadTexture(obstacleBirdTexture);
  UnloadTexture(obstacleSqrTexture);
  UnloadTexture(obstacleRecTexture);
}
