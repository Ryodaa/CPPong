#include "raylib.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>

static void ResetPoints(void);
static void UpdateFrame(void);
static void PlayerTwoAI(void);
static void RandomizeDirection(void);
static void RandomizeAIDirection(void);
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
int frameCounterAI = 0;
int scoreToWin = 10;
bool startScreen = true;
bool gameIsRunning = false;

enum BallDirectionX {
    Left,
    Right
};
enum BallDirectionY {
    Up,
    Down
};
enum ComputerPlayerDirectionY {
    UpAI,
    DownAI
};

BallDirectionX xDirection;
BallDirectionY yDirection;
ComputerPlayerDirectionY yDirectionAI;

const int screenWidth = 1280;
const int screenHeight = 720;

float playerOneY = 320.0F;
static float playerOneX = 20.0F;
float playerTwoY = 320.0F;
static float playerTwoX = 1240.0F;
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

void RandomizeAIDirection(void) {
    ComputerPlayerDirectionY yDirectionAILocal = ComputerPlayerDirectionY(rand() % 2);
    yDirectionAI = yDirectionAILocal;
}

void RandomizeYSpeed(void) {
    if (verticalOffset == 0) {
        verticalOffset = 1;
    } else {
        verticalOffset += 2;
    }
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
            if (!gameIsRunning) {
                DrawText("PLAY!", (1280 / 2) - 40, (720 / 2) - 40, 20, LIGHTGRAY);
            }
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
        ResetPoints();
    } else if (pointsP2 == scoreToWin) {
        SetDefaults();
        ResetPoints();
    }
}

void IncreaseDifficulty(void) {
    if (gameIsRunning) {
        frameCounter++;
        if (frameCounter >= difficultyInterval) {
            horizontalSpeed++;
            frameCounter = 0;
        }
    }
}

void SetDefaults() {
    ball = ballDefault;
    horizontalSpeed = 15;
    verticalSpeed = 0.5F;
    verticalOffset = 0;
    volleyCounter = 0;
    gameIsRunning = false;
    playerOne.y = 320;
    playerTwo.y = 320;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void ResetPoints() {
    startScreen = true;
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

void PlayerTwoAI() {
    if (gameIsRunning) {

        frameCounterAI += 1;

        if (ball.x < GetScreenWidth() / 3) {                        // Only do this when the ball enters the human players half of the playfield
            if (frameCounterAI <= 30) {                             // Do this for set number of frames
                if (yDirectionAI == UpAI) {                         // Check current direction
                    if (playerTwo.y >= 0) {                         // Check if computer hasn't hit Wall
                        playerTwo.y = playerTwo.y - 5;              // Move computer up
                    } else {                                        // If computer hits wall, change direction
                        yDirectionAI = DownAI;
                        playerTwo.y = playerTwo.y + 5;              // Move computer down once
                    }
                } else if (yDirectionAI == DownAI) {                // Check current direction
                    if (playerTwo.y <= GetScreenHeight() - 80) {    // Check if computer hasn't hit wall
                        playerTwo.y = playerTwo.y + 5;              // Move computer down
                    } else {
                        yDirectionAI = UpAI;                          
                        playerTwo.y = playerTwo.y - 5;              // Move computer up once
                    }
                }
            } else {
                frameCounterAI = 0;
                RandomizeAIDirection();
            }
        } else {                                                    // Only do this when the ball enters the computer players half of the playfield

            float paddleY = playerTwo.y + 40;
            float ballY = ball.y;
            float difference = paddleY - ballY;
            float absoluteDif = abs(difference);
            
            if (playerTwo.y > ball.y - 40) {
                if (absoluteDif >= 150) {
                    playerTwo.y -= 20;
                } else if (absoluteDif >= 100) {
                    playerTwo.y -= 15;
                } else if (absoluteDif >= 50) {
                    playerTwo.y -= 10;
                } else if (absoluteDif >= 20) {
                    playerTwo.y -= 8;
                } else if (absoluteDif >= 10) {
                    playerTwo.y -= 3;
                } else {
                    playerTwo.y -= 2;
                }
            } else if (playerTwo.y < ball.y - 40) {
                if (absoluteDif >= 150) {
                    playerTwo.y += 20;
                } else if (absoluteDif >= 100) {
                    playerTwo.y += 15;
                } else if (absoluteDif >= 50) {
                    playerTwo.y += 10;
                } else if (absoluteDif >= 20) {
                    playerTwo.y += 8;
                } else if (absoluteDif >= 10) {
                    playerTwo.y += 3;
                } else {
                    playerTwo.y += 2;
                }
            }
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
            SetDefaults();    
            pointsP2 += 1;
            ball = ballDefault;
            RandomizeDirection();
            RandomizeYSpeed();
        } else if (ball.x >= 1280) {
            SetDefaults();
            pointsP1 += 1;
            ball = ballDefault;
            RandomizeDirection();
            RandomizeYSpeed();
        }
    }
}

void UpdateFrame(void) {
    DrawGame();
    UpdateGame();
}