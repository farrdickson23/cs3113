/**
* Author: Farr Dickson
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"
#include <math.h>
#include <vector>


// Global Constants
constexpr int SCREEN_WIDTH  = 1920,
              SCREEN_HEIGHT = 1080,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#0B2630";
constexpr Vector2 ORIGIN            = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  GOAL_LOC = { SCREEN_WIDTH - 50 , ORIGIN.y - 300 },
                  ALIEN_START = { ORIGIN.x - 700.0f, ORIGIN.y - 400.0f },
                  SPRITE_SHEET_SIZE = { 358.f, 358.f };

constexpr int   NUMBER_OF_GOALS         = 1,
                OG_TRV_DIST = static_cast<int>(sqrt(pow(GOAL_LOC.x - ALIEN_START.x, 2) + pow(GOAL_LOC.y - ALIEN_START.y, 2))),
                NUMBER_OF_RIDGES        = 35;
constexpr float GOAL_LENGTH             = 50.0f,
                END_TIME                = 7.f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 1500.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 1000.0f; // little less than screen height // i dont think ill use this

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gEndTimer        = 0.f,
      miniTimer        = 0.f,
      miniTimerR       = 0.f,
      gTimeAccumulator = 0.0f;


int timer = 0;
int sTimer = 0;

Entity *gAlien = nullptr;
Entity *crashingAlien = nullptr;
Entity *gGoals   = nullptr;
Entity *gRidges  = nullptr;
Entity *exhaust = nullptr;
std::vector<int> starX(30);
std::vector<int> starY(30);

bool gIsPlatformActive = true;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

/* // draw reminders 

// Text drawing functions
void DrawText(const char *text, int posX, int posY, int fontSize, Color color);       // Draw text (using default font)

void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint); // Draw text using font and additional parameters

void DrawTextPro(Font font, const char *text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint); // Draw text using Font and pro parameters (rotation)

*/

void renderUI() {

    DrawRectangle(10, 10, 250, 120, DARKGRAY); // top left UI

    DrawText("STATUS:", 20, 20, 20, WHITE);
    if(gAlien->flewAway()) 
        DrawText("UNKNOWN", 120, 20, 20, YELLOW);
    else if (gAlien->getSpriteState())
        DrawText("UNALIVE", 120, 20, 20, RED);
    else
        DrawText("ALIVE", 120, 20, 20, GREEN);


    DrawText("SPEED:", 20, 45, 20, WHITE);
    Vector2 vel = gAlien->getVelocity();
    int speed = static_cast<int>(sqrt(vel.x * vel.x + vel.y * vel.y));
    const char* cSpeed = (std::to_string(speed)).c_str();
    if (speed < 20.f)
        DrawText(cSpeed, 100, 45, 20, GREEN);
    else if (speed < 30.f)
        DrawText(cSpeed, 100, 45, 20, YELLOW);
    else
        DrawText(cSpeed, 100, 45, 20, RED);


    DrawText("FUEL:", 20, 70, 20, WHITE);
    int fuel = gAlien->getFuel();
    const char* cFuel = (std::to_string(fuel)).c_str();
    if (fuel > 50.f)
        DrawText(cFuel, 90, 70, 20, GREEN);
    else if (fuel > 20.f)
        DrawText(cFuel, 90, 70, 20, YELLOW);
    else
        DrawText(cFuel, 90, 70, 20, RED);


    DrawText("TARGET DIST:", 20, 95, 20, WHITE);
    Vector2 curPos = gAlien->getPosition();
    int dist = static_cast<int>(sqrt(pow(GOAL_LOC.x - curPos.x, 2) + pow(GOAL_LOC.y - curPos.y, 2)));// distance formula
    const char* cDist = (std::to_string(dist)).c_str();
    if (dist > 1000.f)
        DrawText(cDist, 180, 95, 30, DARKBLUE);
    else if (dist > 400.f)
        DrawText(cDist, 180, 95, 30, BLUE);
    else
        DrawText(cDist, 180, 95, 30, SKYBLUE);
    
    // middle timer
    const char* cTimer = (std::to_string(sTimer)).c_str();
    DrawRectangle(SCREEN_WIDTH /2 - 40, 10, 80, 60, DARKGRAY);
    DrawText(cTimer, SCREEN_WIDTH / 2 - 20, 20, 40, RED);
}

int calcScore() {
    int score = 1;
    Vector2 curPos = gAlien->getPosition();
    int dist = static_cast<int>(sqrt(pow(GOAL_LOC.x - curPos.x, 2) + pow(GOAL_LOC.y - curPos.y, 2)));
    int fuel = gAlien->getFuel();

    if (gAlien->getSpriteState() == GOOD) {
        score += 499;
        score += (60 - sTimer) * 10;
        score += fuel * 5;
    }
    score += (OG_TRV_DIST - dist) / 10;

    return score;

    
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");

    /*
        ----------- PROTAGONIST -----------
    */
    //int animationAtlas; // GOOD = 1 , FIRE = 2 , BAD = 3

    // float sizeRatio  = 48.0f / 64.0f; // square sprites

    // Assets from my beautiful artwork
    gAlien = new Entity(
        ALIEN_START, // position
        {50.0f , 50.0f},  // scale
        "assets/game/sheet.png",        // texture file address
        PLAYER,
        SPRITE_SHEET_SIZE
    );
    
    exhaust = new Entity();
    exhaust->setTexture("assets/game/sheet.png");
    exhaust->setEntityType(FIRE);
    exhaust->setScale({ 50.f,50.f });
    
    crashingAlien = new Entity();
    crashingAlien->setTexture("assets/game/sheet.png");
    crashingAlien->setEntityType(ASTEROID);
    crashingAlien->setScale({ 50.f,50.f });
    crashingAlien->setColliderDimensions({ 50.f,50.f });
    crashingAlien->setPosition({ ORIGIN.x, 200.f});


    gAlien->setJumpingPower(20.f);
    gAlien->setColliderDimensions({
        gAlien->getScale().x ,
        gAlien->getScale().y 
    });
    gAlien->setAcceleration({400.f, ACCELERATION_OF_GRAVITY});


    /*
        ----------- GOALS -----------
    */
    gGoals = new Entity[NUMBER_OF_GOALS];
    // i will manually set this up 
    /*
    // Compute the left‑most x coordinate so that the entire row is centred
    float leftMostX = ORIGIN.x - (NUMBER_OF_TILES * TILE_DIMENSION) / 2.0f;
    */
    
    
    gGoals[0].setTexture("assets/game/sheet.png");
    gGoals[0].setEntityType(GOAL);
    gGoals[0].setScale({50.f, 15.f});
    gGoals[0].setColliderDimensions({50.f, 15.f});
    gGoals[0].setPosition(GOAL_LOC); // change this per platform to make map
    
    
    /*
        ----------- RIDGES -----------
    */
    gRidges = new Entity[NUMBER_OF_RIDGES];
    
    //for (int i = 0; i < NUMBER_OF_RIDGES; i++) {
    //    gRidges[i].setAngle( 180.f);
    //}
    gRidges[0].setTexture("assets/game/sheet.png");
    gRidges[0].setEntityType(RIDGE);
    gRidges[0].setScale({ 150.f, 35.f });
    gRidges[0].setColliderDimensions({ 150.f, 35.f });
    gRidges[0].setPosition({ ORIGIN.x - 235.f , ORIGIN.y + 60.0f });
    
    gRidges[1].setTexture("assets/game/sheet.png");
    gRidges[1].setEntityType(RIDGE);
    gRidges[1].setScale({ 50.f, 15.f });
    gRidges[1].setColliderDimensions({ 50.f, 15.f });
    gRidges[1].setPosition({ SCREEN_WIDTH - 5.f ,220.f });
    gRidges[1].setAngle(-45.f); 
    
    gRidges[2].setTexture("assets/game/sheet.png");
    gRidges[2].setEntityType(RIDGE);
    gRidges[2].setScale({ 150.f, 35.f });
    gRidges[2].setColliderDimensions({ 150.f, 35.f });
    gRidges[2].setPosition({ SCREEN_WIDTH - 230.f ,330.f });
    gRidges[2].setAngle(90.f);

    gRidges[13].setTexture("assets/game/sheet.png");
    gRidges[13].setEntityType(RIDGE);
    gRidges[13].setScale({ 150.f, 35.f });
    gRidges[13].setColliderDimensions({ 150.f, 35.f });
    gRidges[13].setPosition({ SCREEN_WIDTH - 430.f ,330.f });
    gRidges[13].setAngle(90.f);

    gRidges[15].setTexture("assets/game/sheet.png");
    gRidges[15].setEntityType(RIDGE);
    gRidges[15].setScale({ 150.f, 35.f });
    gRidges[15].setColliderDimensions({ 150.f, 35.f });
    gRidges[15].setPosition({ SCREEN_WIDTH - 430.f ,180.f });
    gRidges[15].setAngle(90.f);

    gRidges[16].setTexture("assets/game/sheet.png");
    gRidges[16].setEntityType(RIDGE);
    gRidges[16].setScale({ 150.f, 35.f });
    gRidges[16].setColliderDimensions({ 150.f, 35.f });
    gRidges[16].setPosition({ SCREEN_WIDTH - 430.f ,30.f });
    gRidges[16].setAngle(90.f);

    gRidges[14].setTexture("assets/game/sheet.png");
    gRidges[14].setEntityType(RIDGE);
    gRidges[14].setScale({ 150.f, 35.f });
    gRidges[14].setColliderDimensions({ 150.f, 35.f });
    gRidges[14].setPosition({ SCREEN_WIDTH - 505.f ,405.f });

    gRidges[3].setTexture("assets/game/sheet.png");
    gRidges[3].setEntityType(RIDGE);
    gRidges[3].setScale({ 150.f, 35.f });
    gRidges[3].setColliderDimensions({ 150.f, 35.f });
    gRidges[3].setPosition({ SCREEN_WIDTH - 155.f , 252.f });

    gRidges[4].setTexture("assets/game/sheet.png");
    gRidges[4].setEntityType(RIDGE);
    gRidges[4].setScale({ 150.f, 35.f });
    gRidges[4].setColliderDimensions({ 150.f, 35.f });
    gRidges[4].setPosition({ SCREEN_WIDTH - 230.f ,480.f });
    gRidges[4].setAngle(90.f);
    
    
    gRidges[5].setTexture("assets/game/sheet.png");
    gRidges[5].setEntityType(RIDGE);
    gRidges[5].setScale({ 150.f, 35.f });
    gRidges[5].setColliderDimensions({ 150.f, 35.f });
    gRidges[5].setPosition({ SCREEN_WIDTH - 300.f , 555.f });

    gRidges[6].setTexture("assets/game/sheet.png");
    gRidges[6].setEntityType(RIDGE);
    gRidges[6].setScale({ 150.f, 35.f });
    gRidges[6].setColliderDimensions({ 150.f, 35.f });
    gRidges[6].setPosition({ SCREEN_WIDTH - 375.f ,630.f });
    gRidges[6].setAngle(90.f);

    gRidges[7].setTexture("assets/game/sheet.png");
    gRidges[7].setEntityType(RIDGE);
    gRidges[7].setScale({ 150.f, 35.f });
    gRidges[7].setColliderDimensions({ 150.f, 35.f });
    gRidges[7].setPosition({ SCREEN_WIDTH - 575.f ,630.f });
    gRidges[7].setAngle(90.f);

    gRidges[17].setTexture("assets/game/sheet.png");
    gRidges[17].setEntityType(RIDGE);
    gRidges[17].setScale({ 150.f, 35.f });
    gRidges[17].setColliderDimensions({ 150.f, 35.f });
    gRidges[17].setPosition({ SCREEN_WIDTH - 575.f ,480.f });
    gRidges[17].setAngle(90.f);

    gRidges[8].setTexture("assets/game/sheet.png");
    gRidges[8].setEntityType(RIDGE);
    gRidges[8].setScale({ 150.f, 35.f });
    gRidges[8].setColliderDimensions({ 150.f, 35.f });
    gRidges[8].setPosition({ SCREEN_WIDTH - 375.f ,780.f });
    gRidges[8].setAngle(90.f);

    gRidges[11].setTexture("assets/game/sheet.png");
    gRidges[11].setEntityType(RIDGE);
    gRidges[11].setScale({ 150.f, 35.f });
    gRidges[11].setColliderDimensions({ 150.f, 35.f });
    gRidges[11].setPosition({ SCREEN_WIDTH - 575.f ,780.f });
    gRidges[11].setAngle(90.f);

    gRidges[9].setTexture("assets/game/sheet.png");
    gRidges[9].setEntityType(RIDGE);
    gRidges[9].setScale({ 150.f, 35.f });
    gRidges[9].setColliderDimensions({ 150.f, 35.f });
    gRidges[9].setPosition({ SCREEN_WIDTH - 375.f ,930.f });
    gRidges[9].setAngle(90.f);

    gRidges[12].setTexture("assets/game/sheet.png");
    gRidges[12].setEntityType(RIDGE);
    gRidges[12].setScale({ 150.f, 35.f });
    gRidges[12].setColliderDimensions({ 150.f, 35.f });
    gRidges[12].setPosition({ SCREEN_WIDTH - 650.f ,855.f });
    

    gRidges[10].setTexture("assets/game/sheet.png");
    gRidges[10].setEntityType(RIDGE);
    gRidges[10].setScale({ 150.f, 35.f });
    gRidges[10].setColliderDimensions({ 150.f, 35.f });
    gRidges[10].setPosition({ SCREEN_WIDTH - 450.f ,1010.f });

    gRidges[18].setTexture("assets/game/sheet.png");
    gRidges[18].setEntityType(RIDGE);
    gRidges[18].setScale({ 150.f, 35.f });
    gRidges[18].setColliderDimensions({ 150.f, 35.f });
    gRidges[18].setPosition({ SCREEN_WIDTH - 600.f ,1010.f });
    
    gRidges[19].setTexture("assets/game/sheet.png");
    gRidges[19].setEntityType(RIDGE);
    gRidges[19].setScale({ 150.f, 35.f });
    gRidges[19].setColliderDimensions({ 150.f, 35.f });
    gRidges[19].setPosition({ ORIGIN.x - 610.0f, ORIGIN.y - 320.0f });
    gRidges[19].setAngle(90.f);

    gRidges[20].setTexture("assets/game/sheet.png");
    gRidges[20].setEntityType(RIDGE);
    gRidges[20].setScale({ 150.f, 35.f });
    gRidges[20].setColliderDimensions({ 150.f, 35.f });
    gRidges[20].setPosition({ ORIGIN.x - 610.0f, ORIGIN.y - 170.0f });
    gRidges[20].setAngle(90.f);
    
    gRidges[21].setTexture("assets/game/sheet.png");
    gRidges[21].setEntityType(RIDGE);
    gRidges[21].setScale({ 150.f, 35.f });
    gRidges[21].setColliderDimensions({ 150.f, 35.f });
    gRidges[21].setPosition({ ORIGIN.x - 685.0f, ORIGIN.y - 90.0f });

    gRidges[22].setTexture("assets/game/sheet.png");
    gRidges[22].setEntityType(RIDGE);
    gRidges[22].setScale({ 150.f, 35.f });
    gRidges[22].setColliderDimensions({ 150.f, 35.f });
    gRidges[22].setPosition({ ORIGIN.x - 835.f, ORIGIN.y - 90.0f });

    gRidges[23].setTexture("assets/game/sheet.png");
    gRidges[23].setEntityType(RIDGE);
    gRidges[23].setScale({ 150.f, 35.f });
    gRidges[23].setColliderDimensions({ 150.f, 35.f });
    gRidges[23].setPosition({ 0.f, ORIGIN.y - 90.0f });

    gRidges[24].setTexture("assets/game/sheet.png");
    gRidges[24].setEntityType(RIDGE);
    gRidges[24].setScale({ 150.f, 35.f });
    gRidges[24].setColliderDimensions({ 150.f, 35.f });
    gRidges[24].setPosition({ ORIGIN.x - 535, ORIGIN.y - 395.0f });

    gRidges[25].setTexture("assets/game/sheet.png");
    gRidges[25].setEntityType(RIDGE);
    gRidges[25].setScale({ 150.f, 35.f });
    gRidges[25].setColliderDimensions({ 150.f, 35.f });
    gRidges[25].setPosition({ ORIGIN.x - 385, ORIGIN.y - 395.0f });

    gRidges[26].setTexture("assets/game/sheet.png");
    gRidges[26].setEntityType(RIDGE);
    gRidges[26].setScale({ 150.f, 35.f });
    gRidges[26].setColliderDimensions({ 150.f, 35.f });
    gRidges[26].setPosition({ ORIGIN.x - 310, ORIGIN.y - 170.0f });
    gRidges[26].setAngle(90.f);

    gRidges[27].setTexture("assets/game/sheet.png");
    gRidges[27].setEntityType(RIDGE);
    gRidges[27].setScale({ 150.f, 35.f });
    gRidges[27].setColliderDimensions({ 150.f, 35.f });
    gRidges[27].setPosition({ ORIGIN.x - 310.0f, ORIGIN.y - 20.0f });
    gRidges[27].setAngle(90.f);

    gRidges[28].setTexture("assets/game/sheet.png");
    gRidges[28].setEntityType(RIDGE);
    gRidges[28].setScale({ 150.f, 35.f });
    gRidges[28].setColliderDimensions({ 150.f, 35.f });
    gRidges[28].setPosition({ ORIGIN.x - 310, ORIGIN.y - 320.0f });
    gRidges[28].setAngle(90.f);

    gRidges[29].setTexture("assets/game/sheet.png");
    gRidges[29].setEntityType(RIDGE);
    gRidges[29].setScale({ 150.f, 35.f });
    gRidges[29].setColliderDimensions({ 150.f, 35.f });
    gRidges[29].setPosition({ ORIGIN.x - 160.f , ORIGIN.y + 135.0f });
    gRidges[29].setAngle(90.f);

    gRidges[30].setTexture("assets/game/sheet.png");
    gRidges[30].setEntityType(RIDGE);
    gRidges[30].setScale({ 150.f, 35.f });
    gRidges[30].setColliderDimensions({ 150.f, 35.f });
    gRidges[30].setPosition({ ORIGIN.x - 160.f , ORIGIN.y + 285.0f });
    gRidges[30].setAngle(90.f);

    gRidges[31].setTexture("assets/game/sheet.png");
    gRidges[31].setEntityType(RIDGE);
    gRidges[31].setScale({ 150.f, 35.f });
    gRidges[31].setColliderDimensions({ 150.f, 35.f });
    gRidges[31].setPosition({ ORIGIN.x - 160.f , ORIGIN.y + 385.0f });
    gRidges[31].setAngle(90.f);

    gRidges[32].setTexture("assets/game/sheet.png");
    gRidges[32].setEntityType(RIDGE);
    gRidges[32].setScale({ 150.f, 35.f });
    gRidges[32].setColliderDimensions({ 150.f, 35.f });
    gRidges[32].setPosition({ SCREEN_WIDTH - 750.f ,1010.f });

    gRidges[33].setTexture("assets/game/sheet.png");
    gRidges[33].setEntityType(RIDGE);
    gRidges[33].setScale({ 150.f, 35.f });
    gRidges[33].setColliderDimensions({ 150.f, 35.f });
    gRidges[33].setPosition({ SCREEN_WIDTH - 900.f ,1010.f });

    gRidges[34].setTexture("assets/game/sheet.png");
    gRidges[34].setEntityType(RIDGE);
    gRidges[34].setScale({ 150.f, 35.f });
    gRidges[34].setColliderDimensions({ 150.f, 35.f });
    gRidges[34].setPosition({ SCREEN_WIDTH - 1050.f ,1010.f });

    /* 
    for (int i = 0; i < NUMBER_OF_RIDGES; i++) 
    {
        // assets from @see https://kenney.nl/assets/pixel-platformer-industrial-expansion
        gBlocks[i].setTexture("assets/game/tile_0061.png");
        gBlocks[i].setEntityType(BLOCK);
        gBlocks[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gBlocks[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
    }

    gBlocks[0].setPosition({ORIGIN.x - TILE_DIMENSION * 3, ORIGIN.y - TILE_DIMENSION * 2.5f});
    gBlocks[1].setPosition({ORIGIN.x, ORIGIN.y - TILE_DIMENSION * 2.5f});
    gBlocks[2].setPosition({ORIGIN.x + TILE_DIMENSION * 3, ORIGIN.y - TILE_DIMENSION * 2.5f});
    */
    SetTargetFPS(FPS);

    for (int i = 0; i < 30; i++) {
        starX[i] = (rand() % SCREEN_WIDTH);
        
        starY[i] = (rand() % SCREEN_HEIGHT);
        
    }
}

void processInput() 
{
    gAlien->resetMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if (gAlien->checkDone()) {
        return;
    }

    if      (IsKeyDown(KEY_A)) gAlien->turnLeft();
    else if (IsKeyDown(KEY_D)) gAlien->turnRight();

    //if (IsKeyPressed(KEY_W) && !(gAlien->isCollidingBottom())) gAlien->jump();
    if (IsKeyDown(KEY_W)) gAlien->jump();

    // to avoid faster diagonal speed
    //if (GetLength(gAlien->getMovement()) > 1.0f) 
    //    gAlien->normaliseMovement(); 

    
}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    

    while (deltaTime >= FIXED_TIMESTEP)
    {

        gAlien->update(FIXED_TIMESTEP, gGoals, NUMBER_OF_GOALS, gRidges,
            NUMBER_OF_RIDGES);
        exhaust->update(FIXED_TIMESTEP, gAlien, 1, nullptr, 0);

        /* // i dont think i need this now
        gIsPlatformActive = false;

        for (int i = 0; i < NUMBER_OF_RIDGES; i++) 
        {
            gRidges[i].update(FIXED_TIMESTEP, nullptr, 0, nullptr, 0);
            //if (gRidges[i].isActive()) gIsPlatformActive = true;
        }

        for (int i = 0; i < NUMBER_OF_GOALS; i++) 
        {
            //if (!gIsPlatformActive) gTiles[i].deactivate();
            gGoals[i].update(FIXED_TIMESTEP, nullptr, 0, nullptr, 0);
        }*/
        crashingAlien->update(FIXED_TIMESTEP, gAlien, 1, nullptr, 0);

        deltaTime -= FIXED_TIMESTEP;
    }
    if (!gAlien->checkDone()) {
        miniTimer += deltaTime;
        if (miniTimerR != 0) {
            miniTimer += miniTimerR;
            miniTimerR = 0;
        }
        if (miniTimer > 1)
            miniTimerR = miniTimer - 1;
        miniTimer = 0;
        timer += 1;
        sTimer = timer / 100;
    }
    if (gAlien->checkDone()) {
        gEndTimer += deltaTime;
    }
    if (gEndTimer >= END_TIME) {
        gAppStatus = TERMINATED;
    }

    // if you somehow end up out of bounds
    //if (gAlien->getPosition().y > END_GAME_THRESHOLD) 
    //    gAppStatus = TERMINATED;

}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    
    for (int i = 0; i < 30; i++) {
        DrawCircleGradient(starX[i], starY[i], 5.f, WHITE, ColorFromHex(BG_COLOUR));         // Draw a gradient-filled circle
    }

    gAlien->render();
    crashingAlien->render();
    if (gAlien->wasJumping())
        exhaust->render();

    for (int i = 0; i < NUMBER_OF_GOALS;  i++) gGoals[i].render();
    for (int i = 0; i < NUMBER_OF_RIDGES; i++) gRidges[i].render();
    
    if (gEndTimer > 0.f) {
        if (gAlien->getSpriteState() == GOOD) {
            DrawRectangle(ORIGIN.x - 560, ORIGIN.y - 30, 1100, 120, LIGHTGRAY);
            DrawText("MISSION ACCOMPLISHED", ORIGIN.x-535, ORIGIN.y, 80, GREEN);
        }
        else {
            if (gAlien->flewAway()) {
                DrawRectangle(ORIGIN.x - 690, ORIGIN.y - 30, 1340, 250, LIGHTGRAY);
                
                DrawText("MISSION FAILED", ORIGIN.x - 370, ORIGIN.y, 80, RED);
                DrawText("YOU DRIFTED OFF INTO SPACE", ORIGIN.x - 680, ORIGIN.y + 100, 80, RED);
            }
            else {

                DrawRectangle(ORIGIN.x - 600, ORIGIN.y - 30, 1150, 250, LIGHTGRAY);
                DrawText("MISSION FAILED", ORIGIN.x - 370, ORIGIN.y, 80, RED);
                DrawText("YOU WENT BOOM BOOM", ORIGIN.x - 510, ORIGIN.y + 100, 80, RED);
            }
        }
        DrawRectangle(ORIGIN.x - 220, ORIGIN.y - 120, 400, 80, LIGHTGRAY);
        DrawText("SCORE:", ORIGIN.x - 190, ORIGIN.y -110, 60, WHITE);
        //printf(calcScore());
        int intScore = calcScore();
        const char* score = (std::to_string(intScore)).c_str();
        DrawText(score, ORIGIN.x + 40, ORIGIN.y - 110, 60, PURPLE);
    }

    renderUI();

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
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