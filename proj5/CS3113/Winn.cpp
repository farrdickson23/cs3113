#include "Winn.h"

Winn::Winn() : Scene{ {0.0f}, nullptr } {}
Winn::Winn(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

Winn::~Winn() { shutdown(); }

void Winn::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/game/Semi Modular NYE - Joe Esposito.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.nextSceneID = -1;
    //mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
    //mGameState.lives = 3;
    //mPlayerWon = false;
    //mPlayerLost = false;


    /*
       ----------- MAP -----------
    */

    /*
       ----------- PROTAGONIST -----------

    std::map<Direction, std::vector<int>> ninjaAnimationAtlas = {
       {IDLE,  {  0,  1,  2,  3,  4,  5,  6,  7, 8,  9, 10, 11 }}
    }; */


    //float sizeRatio = 48.0f / 48.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    //mGameState.xochitl = new Entity();
    
    mGameState.xochitl = new Entity(
        { 0.f,0.f }, // position
        { 1000.f, 600.0f },             // scale
        "assets/game/puiple.png",                   // texture file address
        NONET                                    // entity type
    );
    //mGameState.ninja->setDirection(IDLE);

    //mGameState.ninja->setJumpingPower(550.0f);
    //mGameState.ninja->setColliderDimensions({
    //   mGameState.ninja->getScale().x / 3.5f,
    //   mGameState.ninja->getScale().y / 3.0f
    //    });
    //mGameState.ninja->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    /*
       ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };                                    // zero initialize
    mGameState.camera.target = { 0.f, 0.f }; // camera follows player
    mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
    mGameState.camera.rotation = 0.0f;                            // no rotation
    mGameState.camera.zoom = 1.0f;                                // default zoom
    //mGameState.sceneType = START;
}

void Winn::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    //mGameState.nextSceneID = 1;


    // CAMERA
    //Vector2 currentPlayerPosition = { mGameState.ninja->getPosition().x, mOrigin.y };

    //panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Winn::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    //printf("RAHHH\n");
    //mGameState.ninja->setDirection(IDLE);
    //mGameState.xochitl->render();
    //printf("Pos%f\n", mGameState.ninja->getPosition());
    Vector2 pos = mGameState.xochitl->getPosition();
    DrawRectangle(pos.x + 500, pos.y-300, 1000, 600, LIME);
    DrawText("WINNER WINNER", 40, 250, 85, BLACK);
    //DrawText("PRESS ENTER TO START\n  PUNCH L,R -> Q,E", -25, -50, 40, DARKPURPLE);

    //mGameState.map->render();
}

void Winn::shutdown()
{
    delete mGameState.xochitl;
    //delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    //UnloadSound(mGameState.jumpSound);
}