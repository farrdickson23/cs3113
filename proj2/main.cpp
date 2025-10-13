/**
* Author: Farr Dickson
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "raylib.h"
#include <math.h>
#include "cs3113.h"
// Enums
enum GameStatus { PLAYING, WINNER1, WINNER2 };
enum Players { ONE, TWO };
//enum BallDirection {RIGHT, LEFT};

// Global Constants
constexpr int SCREEN_WIDTH = 1920.f,
              SCREEN_HEIGHT = 1080.f,
              FPS = 60,
              PADDLE_SPEED = 10;
              

constexpr char  ball[] = "ball.png",
                goal[] = "goal.png",
                paddle[] = "paddle.png",
                ball1[] = "ball1.png",
                ball2[] = "ball2.png",
                ball3[] = "ball3.png",
                ball4[] = "ball4.png",
                ball5[] = "ball5.png",
                ball6[] = "ball6.png",
                noBall[] = "noBall.png",
                free1[] = "free1.png",
                free2[] = "free2.png",
                wall[] = "wall.png";

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BALL_SIZE = { 100.f, 100.f },
                  WALL_SIZE = {SCREEN_WIDTH, 100.f},
                  BALL_SPEED = {200.f, 0.f},
                  GOAL_SIZE = {SCREEN_HEIGHT - 80.f, 150.f },
                  GOAL_SIZE_TURNED = {GOAL_SIZE.y, GOAL_SIZE.x},
                  MESSAGE_SIZE = {600.f, 50.f},
                  PADDLE_SIZE = { 80.f, 250.f };

constexpr Color backgroundColor = {128,128,128};

// Global Variables
AppStatus gAppStatus = RUNNING;
Players   playas = TWO;
GameStatus status = PLAYING;


Vector2 gBall1Movement = { 200.0f, 10.0f },
        gBall2Movement = { -200.0f, .0f },
        gBall3Movement = { 150.0f, .0f },
        gPaddle1Pos = { 150.f, SCREEN_HEIGHT/2},
        gPaddle2Pos = { SCREEN_WIDTH - 150.f, SCREEN_HEIGHT/2 },
        gBall1Pos = ORIGIN,
        ball1Scale = BALL_SIZE,
        gBall2Pos = { ORIGIN.x + 50, ORIGIN.y + 50 },
        ball2Scale = { .0f, .0f },
        gBall3Pos = { ORIGIN.x - 50, ORIGIN.y - 50 },
        ball3Scale = { .0f, .0f },
        tWallPos = { ORIGIN.x, 40.f },
        lGoalPos = {65.f, SCREEN_HEIGHT /2 },
        rGoalPos = { SCREEN_WIDTH - 65.f, SCREEN_HEIGHT/2 },
        bWallPos = { ORIGIN.x, SCREEN_HEIGHT - 40.f };

int gPaddle1Movement =  0 ,
    gPaddle2Movement =  0, 
    ballsRendered    =  1;

float   timeCount = 0.f,
        gClock = 0.f,
        gPreviousTicks = 0.0f;

Texture2D bTexture,
          pTexture,
          gTexture,
          mTexture,
          wTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding();
void Normalise();
float GetLength();
void gameReset();
void blockyAnimate();

// Function Definitions

// borowed from you
float GetLength(const Vector2 *vector)
{
    return sqrtf(
        pow(vector->x, 2) + pow(vector->y, 2)
    );
}

// normalizing is for nerds who dont wanna go fast
/*
void Normalise(Vector2 *vector)
{
    float magnitude = GetLength(vector);

    vector->x /= magnitude;
    vector->y /= magnitude;
}*/
// borowed from you
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void blockyAnimate() {// i imagine this is incredibley inefficient so good thing this isnt sterlings class
    if (timeCount < .3) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(ball1);
    } 
    else if (timeCount < .6) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(ball2);
    }
    else if (timeCount < .9) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(ball3);
    }
    else if (timeCount < 1.2) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(ball4);
    }
    else if (timeCount < 1.5) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(ball5);
    }
    else if (timeCount < 1.8) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(ball6);
    }
    else if (timeCount < 5) {
        UnloadTexture(bTexture);
        bTexture = LoadTexture(noBall);
    }
    else if (timeCount > 4 ) {
        gameReset();
    }
    //else 
}

void gameReset() {
    gBall1Pos = ORIGIN;
    gBall2Pos = ORIGIN;
    gBall3Pos = ORIGIN;
    gBall1Movement = { 200.0f, 10.0f };
    gBall2Movement = { -200.0f, .0f };
    gBall3Movement = { 150.0f, -50.0f }; 
    UnloadTexture(bTexture);
    bTexture = LoadTexture(ball);
    status = PLAYING;
    timeCount = 0.f;

}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Clone: Free Blocky");
    bTexture = LoadTexture(ball);
    pTexture = LoadTexture(paddle);
    wTexture = LoadTexture(wall);
    gTexture = LoadTexture(goal);
    mTexture = LoadTexture(free1);
    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    gPaddle1Movement = 0; // reset movement vector
    //if (!(ballsRendered == 3)) {
    gPaddle2Movement = 0;
    //}

    if (IsKeyDown(KEY_W)) gPaddle1Movement = -400; // up
    else if (IsKeyDown(KEY_S)) gPaddle1Movement = 400; // down
    if (playas == TWO) {
        if (IsKeyDown(KEY_I)) gPaddle2Movement = -400; // up
        else if (IsKeyDown(KEY_K)) gPaddle2Movement = 400; // down
    }
    if (IsKeyDown(KEY_T)) {
        if (playas == ONE) {
            playas = TWO;
        }
        else {
            playas = ONE;
        }
    }
    if (IsKeyDown(49)) { // supposedly num1
        ballsRendered = 1;
    } 
    else if (IsKeyDown(50)) { // supposedly num2
        ballsRendered = 2;
        gBall2Pos = ORIGIN;
    } 
    else if (IsKeyDown(51)) { // supposedly num3
        ballsRendered = 3;
        gBall2Pos = ORIGIN;
        gBall3Pos = ORIGIN;
    }
}

void update() {
    float ticks = (float)GetTime();          // step 1
    float deltaTime = ticks - gPreviousTicks; // step 2
    gPreviousTicks = ticks;

    if (status == WINNER1) {
        UnloadTexture(mTexture);
        mTexture = LoadTexture(free1);
        timeCount += deltaTime;
        blockyAnimate();
        //I could return here so you cant move the ball afer you lose but i think its funnier to push around his corpse
    } else if (status == WINNER2) {
        UnloadTexture(mTexture);
        mTexture = LoadTexture(free2);
        timeCount += deltaTime;
        blockyAnimate();
        // same with the return here, is there something wrong with me 
    }

    gBall1Pos.x += gBall1Movement.x * deltaTime;
    gBall1Pos.y += gBall1Movement.y * deltaTime;
    gBall2Pos.x += gBall2Movement.x * deltaTime;
    gBall2Pos.y += gBall2Movement.y * deltaTime;
    gBall3Pos.x += gBall3Movement.x * deltaTime;
    gBall3Pos.y += gBall3Movement.y * deltaTime;

    // right here I need to if playas == one decide how to update CPU for p1
    if (playas == ONE) {
        if (ballsRendered == 1) {
            if (gBall1Pos.y > gPaddle2Pos.y) {
                gPaddle2Movement = 400;
            }
            else if (gBall1Pos.y < gPaddle2Pos.y) {
                gPaddle2Movement = -400;
            }
        }
        else if (ballsRendered == 2) {
            /*  // this wasnt working and im not smart enough to figure out why
            if (gBall1Pos.x > gBall2Pos.x) {
                if (gBall1Pos.y > gPaddle2Pos.y) {
                    gPaddle2Movement = -400;
                }
                else if (gBall1Pos.y < gPaddle2Pos.y) {
                    gPaddle2Movement = 400;
                }
            }
            else if (gBall1Pos.x < gBall2Pos.x) {
                if (gBall2Pos.y > gPaddle2Pos.y) {
                    gPaddle2Movement = -400;
                }
                else if (gBall2Pos.y < gPaddle2Pos.y) {
                    gPaddle2Movement = 400;
                }
            }*/
            gPaddle2Pos.y = SCREEN_HEIGHT /2 + 300 * sin(3*gClock);
            gClock += 1.f * deltaTime;
            if (gClock >= 2 * PI) { gClock = 0; }
        }
        else if (ballsRendered == 3) { // no i will not code ai for tracking 3 balls you cant make me 
            gPaddle2Pos.y = SCREEN_HEIGHT / 2 + 300 * sin(5 * gClock);
            gClock += 1.f * deltaTime;
            if (gClock >= 2 * PI) { gClock = 0; }
        }
    }

    gPaddle1Pos.y += gPaddle1Movement * deltaTime;
    gPaddle2Pos.y += gPaddle2Movement * deltaTime;

    //make sure paddle doesnt go past the top and bottom walls

    if (isColliding(&gPaddle2Pos, &PADDLE_SIZE, &bWallPos, &WALL_SIZE)) {
        gPaddle2Pos.y -= gPaddle2Movement * deltaTime;
    }
    else if (isColliding(&gPaddle2Pos, &PADDLE_SIZE, &tWallPos, &WALL_SIZE)) {
        gPaddle2Pos.y -= gPaddle2Movement * deltaTime;
    }

    if (isColliding(&gPaddle1Pos, &PADDLE_SIZE, &bWallPos, &WALL_SIZE)) {
        gPaddle1Pos.y -= gPaddle1Movement * deltaTime;
    }
    else if (isColliding(&gPaddle1Pos, &PADDLE_SIZE, &tWallPos, &WALL_SIZE)) {
        gPaddle1Pos.y -= gPaddle1Movement * deltaTime;
    }

    // what if ball hits top or bottom

    if (isColliding(&gBall1Pos, &BALL_SIZE, &bWallPos, &WALL_SIZE) ||
        isColliding(&gBall1Pos, &BALL_SIZE, &tWallPos, &WALL_SIZE)) {
        gBall1Pos.y -= gBall1Movement.y * deltaTime;
        gBall1Movement.y = -gBall1Movement.y;
    }
    if (ballsRendered > 1) {
        if (isColliding(&gBall2Pos, &BALL_SIZE, &bWallPos, &WALL_SIZE) ||
            isColliding(&gBall2Pos, &BALL_SIZE, &tWallPos, &WALL_SIZE)) {
            gBall2Pos.y -= gBall2Movement.y * deltaTime;
            gBall2Movement.y = -gBall2Movement.y;
        }
    } if (ballsRendered > 2) {
        if (isColliding(&gBall3Pos, &BALL_SIZE, &bWallPos, &WALL_SIZE) ||
            isColliding(&gBall3Pos, &BALL_SIZE, &tWallPos, &WALL_SIZE)) {
            gBall3Pos.y -= gBall3Movement.y * deltaTime;
            gBall3Movement.y = -gBall3Movement.y;
        }
    }

    // balls and goal collide ?!?!

    if (isColliding(&gBall1Pos, &BALL_SIZE, &lGoalPos, &GOAL_SIZE_TURNED)) {
        gBall1Movement.x = 0.f;
        gBall1Movement.y = 0.f;
        status = WINNER2;
        gBall2Movement.x = 0.f; // L inneficient coding moment
        gBall2Movement.y = 0.f;
        gBall3Movement.x = 0.f;
        gBall3Movement.y = 0.f;

    }
    else if (isColliding(&gBall1Pos, &BALL_SIZE, &rGoalPos, &GOAL_SIZE_TURNED)) {
        gBall1Movement.x = 0.f;
        gBall1Movement.y = 0.f;
        status = WINNER1;
        gBall2Movement.x = 0.f; // L inneficient coding moment
        gBall2Movement.y = 0.f;
        gBall3Movement.x = 0.f;
        gBall3Movement.y = 0.f;
    }
    // this couldve been simplified with entity... oh well
    if (ballsRendered > 1) {
        if (isColliding(&gBall2Pos, &BALL_SIZE, &lGoalPos, &GOAL_SIZE_TURNED)) {
            gBall1Movement.x = 0.f;
            gBall1Movement.y = 0.f;
            status = WINNER2;
            gBall2Movement.x = 0.f; // L inneficient coding moment
            gBall2Movement.y = 0.f;
            gBall3Movement.x = 0.f;
            gBall3Movement.y = 0.f;

        }
        else if (isColliding(&gBall2Pos, &BALL_SIZE, &rGoalPos, &GOAL_SIZE_TURNED)) {
            gBall1Movement.x = 0.f;
            gBall1Movement.y = 0.f;
            status = WINNER1;
            gBall2Movement.x = 0.f; // L inneficient coding moment
            gBall2Movement.y = 0.f;
            gBall3Movement.x = 0.f;
            gBall3Movement.y = 0.f;
        }
    } if (ballsRendered == 3) {
        if (isColliding(&gBall3Pos, &BALL_SIZE, &lGoalPos, &GOAL_SIZE_TURNED)) {
            gBall1Movement.x = 0.f;
            gBall1Movement.y = 0.f;
            status = WINNER2;
            gBall2Movement.x = 0.f; // L inneficient coding moment
            gBall2Movement.y = 0.f;
            gBall3Movement.x = 0.f;
            gBall3Movement.y = 0.f;

        }
        else if (isColliding(&gBall3Pos, &BALL_SIZE, &rGoalPos, &GOAL_SIZE_TURNED)) {
            gBall1Movement.x = 0.f;
            gBall1Movement.y = 0.f;
            status = WINNER1;
            gBall2Movement.x = 0.f; // L inneficient coding moment
            gBall2Movement.y = 0.f;
            gBall3Movement.x = 0.f;
            gBall3Movement.y = 0.f;
        }
    }
    // ball and paddle touch

    if (isColliding(&gBall1Pos, &BALL_SIZE, &gPaddle2Pos, &PADDLE_SIZE)) {
        gBall1Pos.x -= gBall1Movement.x * deltaTime;
        gBall1Movement.x = -gBall1Movement.x - 50.f;
        if ((gBall1Pos.y - gPaddle2Pos.y) > 20.f) {
            gBall1Movement.y += 100;
        } 
        else if ((gBall1Pos.y - gPaddle2Pos.y) < 20.f) {
            gBall1Movement.y -= 100;
        } 
        else if ((gBall1Pos.y - gPaddle2Pos.y) > 10.f) {
            gBall1Movement.y += 50;
        }
        else if ((gBall1Pos.y - gPaddle2Pos.y) < 10.f) {
            gBall1Movement.y -= 50;
        }
    } else if (isColliding(&gBall1Pos, &BALL_SIZE, &gPaddle1Pos, &PADDLE_SIZE)) {
        gBall1Pos.x -= gBall1Movement.x * deltaTime;
        gBall1Movement.x = -gBall1Movement.x + 50.f;
        if ((gBall1Pos.y - gPaddle1Pos.y) > 20.f) {
            gBall1Movement.y += 100;
        }
        else if ((gBall1Pos.y - gPaddle1Pos.y) < 20.f) {
            gBall1Movement.y -= 100;
        } 
        else if ((gBall1Pos.y - gPaddle1Pos.y) > 10.f) {
            gBall1Movement.y += 50;
        }
        else if ((gBall1Pos.y - gPaddle1Pos.y) < 10.f) {
            gBall1Movement.y -= 50;
        }
    } // dont show sterling this or he might strangle me Im too lazy to write code this better
    if (ballsRendered > 1) {
        if (isColliding(&gBall2Pos, &BALL_SIZE, &gPaddle2Pos, &PADDLE_SIZE)) {
            gBall2Pos.x -= gBall2Movement.x * deltaTime;
            gBall2Movement.x = -gBall2Movement.x - 50.f;
            if ((gBall2Pos.y - gPaddle2Pos.y) > 20.f) {
                gBall2Movement.y += 100;
            }
            else if ((gBall2Pos.y - gPaddle2Pos.y) < 20.f) {
                gBall2Movement.y -= 100;
            }
            else if ((gBall2Pos.y - gPaddle2Pos.y) > 10.f) {
                gBall2Movement.y += 50;
            }
            else if ((gBall2Pos.y - gPaddle2Pos.y) < 10.f) {
                gBall2Movement.y -= 50;
            }
        }
        else if (isColliding(&gBall2Pos, &BALL_SIZE, &gPaddle1Pos, &PADDLE_SIZE)) {
            gBall2Pos.x -= gBall2Movement.x * deltaTime;
            gBall2Movement.x = -gBall2Movement.x + 50.f;
            if ((gBall2Pos.y - gPaddle1Pos.y) > 20.f) {
                gBall2Movement.y += 100;
            }
            else if ((gBall2Pos.y - gPaddle1Pos.y) < 20.f) {
                gBall2Movement.y -= 100;
            }
            else if ((gBall2Pos.y - gPaddle1Pos.y) > 10.f) {
                gBall2Movement.y += 50;
            }
            else if ((gBall2Pos.y - gPaddle1Pos.y) < 10.f) {
                gBall2Movement.y -= 50;
            }
        }
    }
    if (ballsRendered == 3) {
        if (isColliding(&gBall3Pos, &BALL_SIZE, &gPaddle2Pos, &PADDLE_SIZE)) {
            gBall3Pos.x -= gBall3Movement.x * deltaTime;
            gBall3Movement.x = -gBall3Movement.x - 50.f;
            if ((gBall3Pos.y - gPaddle2Pos.y) > 20.f) {
                gBall3Movement.y += 100;
            }
            else if ((gBall3Pos.y - gPaddle2Pos.y) < 20.f) {
                gBall3Movement.y -= 100;
            }
            else if ((gBall3Pos.y - gPaddle2Pos.y) > 10.f) {
                gBall3Movement.y += 50;
            }
            else if ((gBall3Pos.y - gPaddle2Pos.y) < 10.f) {
                gBall3Movement.y -= 50;
            }
        }
        if (isColliding(&gBall3Pos, &BALL_SIZE, &gPaddle1Pos, &PADDLE_SIZE)) {
            gBall3Pos.x -= gBall3Movement.x * deltaTime;
            gBall3Movement.x = -gBall3Movement.x + 50.f;
            if ((gBall3Pos.y - gPaddle1Pos.y) > 20.f) {
                gBall3Movement.y += 100;
            }
            else if ((gBall3Pos.y - gPaddle1Pos.y) < 20.f) {
                gBall3Movement.y -= 100;
            }
            else if ((gBall3Pos.y - gPaddle1Pos.y) > 10.f) {
                gBall3Movement.y += 50;
            }
            else if ((gBall3Pos.y - gPaddle1Pos.y) < 10.f) {
                gBall3Movement.y -= 50;
            }
        }
    }
}

void render()
{
    BeginDrawing();

    ClearBackground(backgroundColor);

    // left GOAL

    Rectangle lgTextureArea = {
        // top-left corner
        0.0f, 0.0f,
        static_cast<float>(gTexture.width),
        static_cast<float>(gTexture.height)
        // bottom-right corner (of texture)

    };

    Rectangle lgDestinationArea = {
        lGoalPos.x,
        lGoalPos.y, //static_cast<float>(wTexture.height)
        static_cast<float>(GOAL_SIZE.x),
        static_cast<float>(GOAL_SIZE.y)
    };

    Vector2 lgOrigin = {
        GOAL_SIZE.x / 2.0f, //static_cast<float>(SCREEN_WIDTH)
        GOAL_SIZE.y / 2.0f //static_cast<float>(wTexture.height)
    };

    DrawTexturePro(
        gTexture, // the texture object
        lgTextureArea,  // the UV coordinates
        lgDestinationArea,    // the XY coordinates 
        lgOrigin,    // the texture origin 
        270.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    // right GOAL

    Rectangle rgTextureArea = {
        // top-left corner
        0.0f, 0.0f,
        static_cast<float>(gTexture.width),
        static_cast<float>(gTexture.height)
        // bottom-right corner (of texture)

    };

    Rectangle rgDestinationArea = {
        rGoalPos.x,
        rGoalPos.y, //static_cast<float>(wTexture.height)
        static_cast<float>(GOAL_SIZE.x),
        static_cast<float>(GOAL_SIZE.y)
    };

    Vector2 rgOrigin = {
        GOAL_SIZE.x / 2.0f, //static_cast<float>(SCREEN_WIDTH)
        GOAL_SIZE.y / 2.0f //static_cast<float>(wTexture.height)
    };

    DrawTexturePro(
        gTexture, // the texture object
        rgTextureArea,  // the UV coordinates
        rgDestinationArea,    // the XY coordinates 
        rgOrigin,    // the texture origin 
        90.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    //top wall

    Rectangle twTextureArea = {
        // top-left corner
        0.0f, 0.0f,
        -static_cast<float>(wTexture.width),
        -static_cast<float>(wTexture.height) 
        // bottom-right corner (of texture)
        
    };

    Rectangle twDestinationArea = {
        tWallPos.x,
        tWallPos.y, 
        static_cast<float>(WALL_SIZE.x),
        static_cast<float>(WALL_SIZE.y)
    };

    Vector2 twOrigin = {
        WALL_SIZE.x / 2.0f, 
        WALL_SIZE.y / 2.0f 
    };

    DrawTexturePro(
        wTexture, // the texture object
        twTextureArea,  // the UV coordinates
        twDestinationArea,    // the XY coordinates 
        twOrigin,    // the texture origin 
        0.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    // bottom wall
    Rectangle bwTextureArea = {
        // top-left corner
        0.0f, 0.0f,
        static_cast<float>(wTexture.width),
        static_cast<float>(wTexture.height)
        // bottom-right corner (of texture)

    };

    Rectangle bwDestinationArea = {
        bWallPos.x,
        bWallPos.y, //static_cast<float>(wTexture.height)
        static_cast<float>(WALL_SIZE.x),
        static_cast<float>(WALL_SIZE.y)
    };

    Vector2 bwOrigin = {
        WALL_SIZE.x / 2.0f, //static_cast<float>(SCREEN_WIDTH)
        WALL_SIZE.y / 2.0f //static_cast<float>(wTexture.height)
    };

    DrawTexturePro(
        wTexture, // the texture object
        bwTextureArea,  // the UV coordinates
        bwDestinationArea,    // the XY coordinates 
        bwOrigin,    // the texture origin 
        0.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    // Draw P1

    Rectangle p1TextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(pTexture.width),
        static_cast<float>(pTexture.height)
    };

    Rectangle p1DestinationArea = {
        gPaddle1Pos.x,
        gPaddle1Pos.y,
        static_cast<float>(PADDLE_SIZE.x),
        static_cast<float>(PADDLE_SIZE.y)
    };

    Vector2 p1Origin = {
        static_cast<float>(PADDLE_SIZE.x) / 2.0f,
        static_cast<float>(PADDLE_SIZE.y) / 2.0f
    };

    DrawTexturePro(
        pTexture, // the texture object
        p1TextureArea,  // the UV coordinates
        p1DestinationArea,    // the XY coordinates 
        p1Origin,    // the texture origin 
        0.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    // Draw P2

    Rectangle p2TextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(pTexture.width),
        static_cast<float>(pTexture.height)
    };

    Rectangle p2DestinationArea = {
        gPaddle2Pos.x,
        gPaddle2Pos.y,
        static_cast<float>(PADDLE_SIZE.x),
        static_cast<float>(PADDLE_SIZE.y)
    };

    Vector2 p2Origin = {
        static_cast<float>(PADDLE_SIZE.x) / 2.0f,
        static_cast<float>(PADDLE_SIZE.y) / 2.0f
    };

    DrawTexturePro(
        pTexture, // the texture object
        p2TextureArea,  // the UV coordinates
        p2DestinationArea,    // the XY coordinates 
        p2Origin,    // the texture origin 
        0.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    // Draw Ball

    Rectangle b1TextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(bTexture.width),
        static_cast<float>(bTexture.height)
    };

    Rectangle b1DestinationArea = {
        gBall1Pos.x,
        gBall1Pos.y,
        static_cast<float>(ball1Scale.x),
        static_cast<float>(ball1Scale.y)
    };

    Vector2 ball1Origin = {
        static_cast<float>(ball1Scale.x) / 2.0f,
        static_cast<float>(ball1Scale.y) / 2.0f
    };

    DrawTexturePro(
        bTexture, // the texture object
        b1TextureArea,  // the UV coordinates
        b1DestinationArea,    // the XY coordinates 
        ball1Origin,    // the texture origin 
        0.f,    // rotate
        WHITE        // a sort of colour lens; use WHITE for none
    );

    // this is my last warcrime against coding i promise

    if (ballsRendered > 1) {
        // Draw Ball

        Rectangle b2TextureArea = {
            // top-left corner
            0.0f, 0.0f,

            // bottom-right corner (of texture)
            static_cast<float>(bTexture.width),
            static_cast<float>(bTexture.height)
        };

        Rectangle b2DestinationArea = {
            gBall2Pos.x,
            gBall2Pos.y,
            static_cast<float>(ball1Scale.x),
            static_cast<float>(ball1Scale.y)
        };

        Vector2 ball2Origin = {
            static_cast<float>(ball1Scale.x) / 2.0f,
            static_cast<float>(ball1Scale.y) / 2.0f
        };

        DrawTexturePro(
            bTexture, // the texture object
            b2TextureArea,  // the UV coordinates
            b2DestinationArea,    // the XY coordinates 
            ball2Origin,    // the texture origin 
            0.f,    // rotate
            WHITE        // a sort of colour lens; use WHITE for none
        );
    } if (ballsRendered == 3) {
        // Draw Ball

        Rectangle b3TextureArea = {
            // top-left corner
            0.0f, 0.0f,

            // bottom-right corner (of texture)
            static_cast<float>(bTexture.width),
            static_cast<float>(bTexture.height)
        };

        Rectangle b3DestinationArea = {
            gBall3Pos.x,
            gBall3Pos.y,
            static_cast<float>(ball1Scale.x),
            static_cast<float>(ball1Scale.y)
        };

        Vector2 ball3Origin = {
            static_cast<float>(ball1Scale.x) / 2.0f,
            static_cast<float>(ball1Scale.y) / 2.0f
        };

        DrawTexturePro(
            bTexture, // the texture object
            b3TextureArea,  // the UV coordinates
            b3DestinationArea,    // the XY coordinates 
            ball3Origin,    // the texture origin 
            0.f,    // rotate
            WHITE        // a sort of colour lens; use WHITE for none
        );
    }

    if (status == WINNER1 || status == WINNER2) {
        //draw winner icon 

        Rectangle mTextureArea = {
            // top-left corner
            0.0f, 0.0f,

            // bottom-right corner (of texture)
            static_cast<float>(mTexture.width),
            static_cast<float>(mTexture.height)
        };

        Rectangle mDestinationArea = {
            ORIGIN.x,
            ORIGIN.y,
            static_cast<float>(MESSAGE_SIZE.x),
            static_cast<float>(MESSAGE_SIZE.y)
        };

        Vector2 mOrigin = {
            static_cast<float>(MESSAGE_SIZE.x) / 2.0f,
            static_cast<float>(MESSAGE_SIZE.y) / 2.0f
        };

        DrawTexturePro(
            mTexture, // the texture object
            mTextureArea,  // the UV coordinates
            mDestinationArea,    // the XY coordinates 
            mOrigin,    // the texture origin 
            0.f,    // rotate
            WHITE        // a sort of colour lens; use WHITE for none
        );
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); // Close window and OpenGL context
    UnloadTexture(bTexture);
    UnloadTexture(pTexture);
    UnloadTexture(wTexture);
    UnloadTexture(gTexture);
    UnloadTexture(mTexture);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}