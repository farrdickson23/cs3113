#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
   SetMusicVolume(mGameState.bgm, 0.33f);
   // PlayMusicStream(gState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/tileset.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      6, 1,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> ninjaAnimationAtlas = {
      {IDLE,  {  0,  1,  2,  3,  4,  5,  6,  7, 8,  9, 10, 11 }},
      {LEFT,  {  30, 31, 32, 33 }},
      {PLEFT, {  18, 19, 20, 21, 22, 23 }},
      {PRIGHT,{  12, 13, 14, 15, 16, 17 }},
      {RIGHT, {  24, 25, 26, 27 }},
   };

   float sizeRatio  = 48.0f / 64.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.ninja = new Entity(
      {mOrigin.x - 300.0f, mOrigin.y - 200.0f}, // position
      {250.0f * sizeRatio, 250.0f},             // scale
      "assets/game/mc.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 6, 6 },                                 // atlas dimensions
      ninjaAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.ninja->setJumpingPower(550.0f);
   mGameState.ninja->setColliderDimensions({
      mGameState.ninja->getScale().x / 3.f,
      mGameState.ninja->getScale().y / 1.1f
   });
   mGameState.ninja->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.ninja->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.ninja->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   Vector2 currentPlayerPosition = { mGameState.ninja->getPosition().x, mOrigin.y };

   if (mGameState.ninja->getPosition().y > 800.0f) mGameState.nextSceneID = 1;

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.ninja->render();
   mGameState.map->render();
}

void LevelA::shutdown()
{
   delete mGameState.ninja;
   delete mGameState.map;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}