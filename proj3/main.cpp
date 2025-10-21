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

// Global Constants
constexpr int SCREEN_WIDTH  = 1920,
              SCREEN_HEIGHT = 1080,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#0B2630";
constexpr Vector2 ORIGIN            = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SPRITE_SHEET_SIZE = { 358.f, 358.f };

constexpr int   NUMBER_OF_GOALS         = 1,
                NUMBER_OF_RIDGES        = 1;
constexpr float GOAL_LENGTH             = 50.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 1200.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 1000.0f; // little less than screen height

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Entity *gAlien = nullptr;
Entity *gGoals   = nullptr;
Entity *gRidges  = nullptr;

bool gIsPlatformActive = true;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

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
        {ORIGIN.x - 700.0f, ORIGIN.y - 400.0f}, // position
        {50.0f , 50.0f},  // scale
        "assets/game/sheet.png",        // texture file address
        PLAYER,
        SPRITE_SHEET_SIZE
    );

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
    gGoals[0].setScale({200.f, 50.f});
    gGoals[0].setColliderDimensions({200.f, 20.f});
    gGoals[0].setPosition({ ORIGIN.x , ORIGIN.y - 300 });
    
    
    /*
        ----------- RIDGES -----------
    */
    gRidges = new Entity[NUMBER_OF_RIDGES];
    
    //for (int i = 0; i < NUMBER_OF_RIDGES; i++) {
    //    gRidges[i].setAngle( 180.f);
    //}
    gRidges[0].setTexture("assets/game/sheet.png");
    gRidges[0].setEntityType(RIDGE);
    gRidges[0].setScale({ 200.f, 50.f });
    gRidges[0].setColliderDimensions({ 200.f, 20.f });
    gRidges[0].setPosition({ ORIGIN.x , ORIGIN.y +300});
    
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
}

void processInput() 
{
    gAlien->resetMovement();

    if      (IsKeyDown(KEY_A)) gAlien->turnLeft();
    else if (IsKeyDown(KEY_D)) gAlien->turnRight();

    //if (IsKeyPressed(KEY_W) && !(gAlien->isCollidingBottom())) gAlien->jump();
    if (IsKeyDown(KEY_W)) gAlien->jump();

    // to avoid faster diagonal speed
    //if (GetLength(gAlien->getMovement()) > 1.0f) 
    //    gAlien->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
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

    if (gAlien->getDoneStat()) {
        // skip updating
        // display message
        gAppStatus == TERMINATED;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gAlien->update(FIXED_TIMESTEP, gGoals, NUMBER_OF_GOALS, gRidges,
            NUMBER_OF_RIDGES);

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

        deltaTime -= FIXED_TIMESTEP;
    }
    // if you somehow end up out of bounds
    if (gAlien->getPosition().y > END_GAME_THRESHOLD) 
        gAppStatus = TERMINATED;
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gAlien->render();

    for (int i = 0; i < NUMBER_OF_GOALS;  i++) gGoals[i].render();
    for (int i = 0; i < NUMBER_OF_RIDGES; i++) gRidges[i].render();

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