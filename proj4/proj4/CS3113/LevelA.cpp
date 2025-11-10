#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = -1;

   mGameState.bgm = LoadMusicStream("assets/game/Semi Modular NYE - Joe Esposito.wav");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/ninjaJump.wav");
   mGameState.ouchSound = LoadSound("assets/game/hurt.wav");
   mGameState.punchSound = LoadSound("assets/game/punch.wav");
   mGameState.loseSound = LoadSound("assets/game/lose.wav");
   mGameState.endScreenTimer = 5.f;
   mPlayerWon = false;
   mPlayerLost = false;

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

   float sizeRatio  = 48.0f / 48.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.ninja = new Entity(
      {mOrigin.x - 600.0f, mOrigin.y - 200.0f}, // position
      {150.0f * sizeRatio, 150.0f},             // scale
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
   mGameState.ninja->setFrameSpeed(3);

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.ninja->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom

   // NPCs

   std::map<Direction, std::vector<int>> monsterAnimationAtlas = {
      {LEFT,  {  0, 1, 2, 3 }},
      {RIGHT, {  4, 5, 6, 7 }},
      {DDEAD, { 8, 9, 10 }}
   };

   mGameState.monster1 = new Entity(
       { mOrigin.x - 300.0f, mOrigin.y - 200.0f }, // position
       { 150.0f * sizeRatio, 150.0f },             // scale
       "assets/game/zed.png",                   // texture file address
       ATLAS,                                    // single image or atlas?
       { 3, 4 },                                 // atlas dimensions
       monsterAnimationAtlas,                    // actual atlas
       NPC                                    // entity type
   );
   mGameState.monster1->setColliderDimensions({
      mGameState.monster1->getScale().x / 3.f,
      mGameState.monster1->getScale().y / 1.1f
       });
   mGameState.monster1->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

   mGameState.monster1->setAIType(FOLLOWER);
   mGameState.monster1->setAIState(SIDLE);

   mGameState.monster1->setSpeed(Entity::DEFAULT_SPEED / 3); // the monster is going to fast

   mGameState.monster2 = new Entity(
       { mOrigin.x + 300.f, mOrigin.y - 200.0f }, // position
       { 150.0f * sizeRatio, 150.0f },             // scale
       "assets/game/zed.png",                   // texture file address
       ATLAS,                                    // single image or atlas?
       { 3, 4 },                                 // atlas dimensions
       monsterAnimationAtlas,                    // actual atlas
       NPC                                    // entity type
   );
   mGameState.monster2->setColliderDimensions({
      mGameState.monster2->getScale().x / 3.f,
      mGameState.monster2->getScale().y / 1.1f
       });
   mGameState.monster2->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

   mGameState.monster2->setAIType(FOLLOWER);
   mGameState.monster2->setAIState(SIDLE);

   mGameState.monster2->setSpeed(Entity::DEFAULT_SPEED / 3); // the monster is going to fast
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
   if (mPlayerLost)
   {
       mGameState.endScreenTimer -= deltaTime;

       if (mGameState.endScreenTimer <= 0.0f)
           mGameState.nextSceneID = 0;  // back to start screen

       return;
   }

   mGameState.ninja->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   mGameState.monster1->update(
       deltaTime,      // delta time / fixed timestep
       mGameState.ninja,        // player
       mGameState.map, // map
       nullptr,        // collidable entities
       0               // col. entity count
   );

   mGameState.monster2->update(
       deltaTime,      // delta time / fixed timestep
       mGameState.ninja,        // player
       mGameState.map, // map
       nullptr,        // collidable entities
       0               // col. entity count
   );

   if ((mGameState.ninja->isColliding(mGameState.monster1)) || (mGameState.ninja->isColliding(mGameState.monster2))) // i dont remember why we had this private but hopefully this isn't too big of an issue oh whale
   {
       mGameState.lives = mGameState.lives - 1;

       //std::cout << "HIT! lives = " << mGameState.lives
       //    << "  nextSceneID(before) = " << mGameState.nextSceneID << std::endl;

       if (mGameState.lives > 0) { // reset the level
           mGameState.nextSceneID = 1;
           PlaySound(mGameState.ouchSound);
       }
       else // go to you lose
       {
           mPlayerLost = true;
           PlaySound(mGameState.loseSound);
       }
       //std::cout << "after set: nextSceneID = " << mGameState.nextSceneID << std::endl;
   }
   Vector2 currentPlayerPosition = { mGameState.ninja->getPosition().x, mOrigin.y };

   if (mGameState.ninja->getPosition().y > 800.0f) mGameState.nextSceneID = 2;

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.ninja->render();
   mGameState.monster1->render();
   mGameState.monster2->render();
   mGameState.map->render();

   if (mPlayerLost)
       DrawText("YOU LOSE BOZO!!", mGameState.ninja->getPosition().x, mGameState.ninja->getPosition().y + 30, 40, RED);
}

void LevelA::shutdown()
{
   delete mGameState.ninja;
   delete mGameState.map;
   delete mGameState.monster1; 
   delete mGameState.monster2;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.loseSound);
   UnloadSound(mGameState.jumpSound); 
   UnloadSound(mGameState.punchSound);
   UnloadSound(mGameState.ouchSound);
}