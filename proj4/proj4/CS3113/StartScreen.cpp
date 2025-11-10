#include "StartScreen.h"

StartScreen::StartScreen() : Scene{ {0.0f}, nullptr } {}
StartScreen::StartScreen(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

StartScreen::~StartScreen() { shutdown(); }

void StartScreen::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/game/Semi Modular NYE - Joe Esposito.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    //mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
    mGameState.lives = 3;
    mPlayerWon = false;
    mPlayerLost = false;


    /*
       ----------- MAP -----------
    */

    /*
       ----------- PROTAGONIST -----------
    
    std::map<Direction, std::vector<int>> ninjaAnimationAtlas = {
       {IDLE,  {  0,  1,  2,  3,  4,  5,  6,  7, 8,  9, 10, 11 }}
    }; */
    std::map<Direction, std::vector<int>> ninjaAnimationAtlas = {
      {IDLE,  {  0,  1,  2,  3,  4,  5,  6,  7, 8,  9, 10, 11 }},
      {LEFT,  {  30, 31, 32, 33 }},
      {PLEFT, {  18, 19, 20, 21, 22, 23 }},
      {PRIGHT,{  12, 13, 14, 15, 16, 17 }},
      {RIGHT, {  24, 25, 26, 27 }},
    };

    float sizeRatio = 48.0f / 48.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    mGameState.ninja = new Entity(
        { mOrigin.x - 300.0f, mOrigin.y - 150.0f }, // position
        { 250.0f * sizeRatio, 250.0f },             // scale
        "assets/game/mc.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 6, 6 },                                 // atlas dimensions
        ninjaAnimationAtlas,                    // actual atlas
        PLAYER                                    // entity type
    );
    mGameState.ninja->setDirection(IDLE);

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
    mGameState.camera.target = mGameState.ninja->getPosition(); // camera follows player
    mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
    mGameState.camera.rotation = 0.0f;                            // no rotation
    mGameState.camera.zoom = 1.0f;                                // default zoom
    mGameState.sceneType = START;
}

void StartScreen::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    if (mGameState.sceneType == LEVEL) mGameState.nextSceneID = 1;

    mGameState.ninja->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    // CAMERA
    Vector2 currentPlayerPosition = { mGameState.ninja->getPosition().x, mOrigin.y };

    //panCamera(&mGameState.camera, &currentPlayerPosition);
}

void StartScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    //printf("RAHHH\n");
    mGameState.ninja->setDirection(IDLE);
    mGameState.ninja->render();
    //printf("Pos%f\n", mGameState.ninja->getPosition());

    DrawText("PRESS ENTER TO START\n  PUNCH L,R -> Q,E", -25, -50, 40, DARKPURPLE);

    //mGameState.map->render();
}

void StartScreen::shutdown()
{
    delete mGameState.ninja;
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    //UnloadSound(mGameState.jumpSound);
}