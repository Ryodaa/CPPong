#include "raylib.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <random>

static void UpdateFrame(void);
static void PlayerTwoAI(void);
static void RandomizeDirection(void);
static void RandomizeYSpeed(void);
static void MovePaddle(void);
static void BallMovement(void);
static void DrawGame(void);
static void UpdateGame(void);
static void CheckGameStatus(void);
static void SetDefaults(void);
static void IncreaseDifficulty(void);

const Vector2 ballDefault = {625, 370};
Vector2 ball = ballDefault;
float horizontalSpeed = 15;
float verticalSpeed;
float verticalOffset;

static float paddleSpeed = 15;
static float paddleLength = 80;
static float paddleThickness = 20;

int volleyCounter = 0;
int difficultyInterval = 300;
int frameCounter = 0;
int scoreToWin = 10;
bool startScreen = true;
bool gameIsRunning = false;

enum BallDirectionX
{
    Left,
    Right
};
enum BallDirectionY
{
    Up,
    Down
};

BallDirectionX xDirection;
BallDirectionY yDirection;

const int screenWidth = 1280;
const int screenHeight = 720;

float playerOneY = 320;
static float playerOneX = 20;
float playerTwoY = 320;
static float playerTwoX = 1240;
Rectangle playerOne = {playerOneX, playerOneY, paddleThickness, paddleLength};
Rectangle playerTwo = {playerTwoX, playerTwoY, paddleThickness, paddleLength};

int pointsP1 = 0;
int pointsP2 = 0;

int main(void) {
    InitWindow(screenWidth, screenHeight, "Pong");

    SetTargetFPS(60);       
    DrawGame();
    RandomizeDirection();
    RandomizeYSpeed();

    while (!WindowShouldClose()) {
        UpdateFrame();
    }

    CloseWindow();

    return 0;
}

void RandomizeDirection(void) {
    BallDirectionX xDirectionLocal = BallDirectionX(rand() % 2);
    BallDirectionY yDirectionLocal = BallDirectionY(rand() % 2);
    xDirection = xDirectionLocal;
    yDirection = yDirectionLocal;
}

void RandomizeYSpeed(void) {
    verticalOffset += 2;
    std::srand(std::time(0));
    int randomInt = std::rand() % 50 + verticalOffset;
    float randomFloat = randomInt / 10.0F;
    verticalSpeed = randomFloat;

}

void DrawGame(void) {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (startScreen) {
            DrawText("PONG", (1280 / 2) - 40, (720 / 2) - 40, 20, LIGHTGRAY);
            DrawText("PRESS ANY BUTTON TO PLAY", (1280 / 2) - 170, (720 / 2) + 40, 20, LIGHTGRAY);
        } else {
            const char* pointsP1Char = std::to_string(pointsP1).c_str();
            const char* pointsP2Char = std::to_string(pointsP2).c_str();
            const char* volleyCounterChar = std::to_string(volleyCounter).c_str();
            DrawText(pointsP1Char, 50, 10, 50, LIGHTGRAY);
            DrawText(pointsP2Char, 1200, 10, 50, LIGHTGRAY);
            DrawText(volleyCounterChar, 680, 650, 30, LIGHTGRAY);
            DrawText("Volley: ", 550, 653, 20, LIGHTGRAY);
        }
        
        DrawRectangleRec(playerOne, BLACK);
        DrawRectangleRec(playerTwo, BLACK);
        DrawCircleV(ball, 10.0f, BLACK);

    EndDrawing();
}

void CheckGameStatus(void) {
    if (pointsP1 == scoreToWin) {
        SetDefaults();
    } else if (pointsP2 == scoreToWin) {
        SetDefaults();
    }
}

void IncreaseDifficulty(void) {
    frameCounter++;
    if (frameCounter >= difficultyInterval) {
        horizontalSpeed++;
        frameCounter = 0;
    }
}

void SetDefaults() {
    ball = ballDefault;
    horizontalSpeed = 15;
    verticalSpeed = 0.5F;
    verticalOffset = 0;

    volleyCounter = 0;
    startScreen = true;
    gameIsRunning = false;

    playerOne.y = 320;
    playerTwo.y = 320;

    pointsP1 = 0;
    pointsP2 = 0;
}

void UpdateGame() {
    CheckGameStatus();
    BallMovement();
    MovePaddle();
    PlayerTwoAI();
    IncreaseDifficulty();
}

void MovePaddle() {
    if (IsKeyDown(KEY_UP)) {
        startScreen = false;
        gameIsRunning = true;
        if (playerOne.y <= 0) {
            playerOne.y = 0;
        } else {
            playerOne.y -= paddleSpeed;
        }
    } else if (IsKeyDown(KEY_DOWN)) {
        startScreen = false;
        gameIsRunning = true;
        if (playerOne.y >= screenHeight - paddleLength) {
            playerOne.y = screenHeight - paddleLength;
        } else {
            playerOne.y += paddleSpeed;
        }
    }
}

void BallMovement() {
    if (gameIsRunning) {
        bool hasCollidedP1 = CheckCollisionCircleRec(ball, 10.0F, playerOne);
        bool hasCollidedP2 = CheckCollisionCircleRec(ball, 10.0F, playerTwo);
        if (hasCollidedP1) {
            xDirection = Right;
            volleyCounter++;
            RandomizeYSpeed();
        }
        if (hasCollidedP2) {
            xDirection = Left;
            volleyCounter++;
            RandomizeYSpeed();
        }

        if (xDirection == Right) {
            if (yDirection == Up) {
                ball.x += horizontalSpeed;
                ball.y -= verticalSpeed;
            } else if (yDirection == Down) {
                ball.x += horizontalSpeed;
                ball.y += verticalSpeed;
            }
        } else if (xDirection == Left) {
            if (yDirection == Up) {
                ball.x -= horizontalSpeed;
                ball.y -= verticalSpeed;
            } else if (yDirection == Down) {
                ball.x -= horizontalSpeed;
                ball.y += verticalSpeed;
            }
        }

        if (ball.y <= 10) {
            yDirection = Down;
        } else  if (ball.y >= 710) {
            yDirection = Up;
        }

        if (ball.x <= 10) {
            volleyCounter = 0;
            horizontalSpeed = 15;
            pointsP2 += 1;
            gameIsRunning = false;
            ball = ballDefault;
            RandomizeDirection();
            RandomizeYSpeed();
        } else if (ball.x >= 1280) {
            volleyCounter = 0;
            horizontalSpeed = 15;
            pointsP1 += 1;
            gameIsRunning = false;
            ball = ballDefault;
            RandomizeDirection();
            RandomizeYSpeed();
        }
    }
}

void PlayerTwoAI() {
    playerTwo.y = ball.y - 40;
}

void UpdateFrame(void) {
    DrawGame();
    UpdateGame();
}