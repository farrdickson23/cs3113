#include "LevelA.h"
//#DEFINE RED
LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

Vector2 spawnPos1 = { 0.f, 0.f };
Vector2 spawnPos2 = { 0.f, 0.f };
Vector2 spawnPos3 = { 0.f, 0.f };
Vector2 spawnPos4 = { 0.f, 0.f };
Vector2 spawnPos5 = { 0.f, 0.f };
bool trap = true;

int kills = 0;

float timeChange;

bool hit = false;

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
    //mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    //SetMusicVolume(mGameState.bgm, 0.33f);
    // PlayMusicStream(gState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

    trap = true;

    kills = 0;

    timeChange = 0.f;

    hit = false;

    /*
       ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVELA_WIDTH, LEVELA_HEIGHT,   // map grid cols & rows
        (unsigned int*)mLevelData, // grid data
        "assets/game/mytileset.png",   // texture filepath
        TILE_DIMENSION,              // tile size
        8, 1,                        // texture cols & rows
        mOrigin                      // in-game origin
    );

    // eneamie spawn positions
    //   ^ what in the fuck did i just spell (it was enemy)

    spawnPos1 = { mGameState.map->getLeftBoundary() + (mGameState.map->getRightBoundary() - mGameState.map->getLeftBoundary()) / 3,
        mGameState.map->getTopBoundary() + (mGameState.map->getBottomBoundary() - mGameState.map->getTopBoundary())/2 };

    spawnPos4 = {  -(mGameState.map->getLeftBoundary()) + 200, mGameState.map->getTopBoundary() + (mGameState.map->getBottomBoundary() - mGameState.map->getTopBoundary()) / 2 };

    /*
       ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
       {DOWN,  {  0,  1,  2,  3 }},
       {DOWNRIGHT, {4,  5,  6,  7 }},
       {DOWNLEFT,  {  8,  9, 10, 11}},
       {LEFT,{ 16, 17, 18, 19}},
       {UP,    {  20, 21, 22, 23 }},
       {UPRIGHT,{24, 25, 26, 27 }},
       {RIGHT, { 12, 13, 14, 15 }},
       {UPLEFT, {28, 29, 30, 31}}
    };

    std::map<Direction, std::vector<int>> blasterAnimationAtlas = {
        {RIGHT, {0}},
        {SRR, {0}},
        {SRL, {1}},
        {SRU, {2}},
        {SRD, {3}},
        {SGR, {4}},
        {SGL, {5}},
        {SGU, {6}},
        {SGD, {7}},
        {ARR, {8}},
        {ARL, {9}},
        {ARU, {10}},
        {ARD, {11}}
    };

    std::map<Direction, std::vector<int>> blastAnimationAtlas = {
        {RIGHT, {0}},
        {SRR, {0}},
        {SRL, {0}},
        {SRU, {0}},
        {SRD, {0}},
        {SGR, {1}},
        {SGL, {1}},
        {SGU, {1}},
        {SGD, {1}},
        {ARR, {2}},
        {ARL, {2}},
        {ARU, {2}},
        {ARD, {2}}
    };
    std::map<Direction, std::vector<int>> BirdAnimationAtlas = {
        {RIGHT, {0,1,2,3}}, 
        {LEFT, {4,5,6,7}},
        {UP, {0,1,2,3}},
        {DOWN, {0,1,2,3}}
    };
    std::map<Direction, std::vector<int>> enemyAnimationAtlas = {
        {LEFT, {0,1,2,3}},
        {RIGHT, {4,5,6,7}},
        {UP, {0,1,2,3}},
        {DOWN, {0,1,2,3}}
    };

    std::map<Direction, std::vector<int>> chestAnimationAtlas = {
        {RIGHT, {0}}, // this works ? i guess
        {DOWN, {0}},
        {UP, {1}}
    };

    //float sizeRatio = 48.0f / 64.0f;
    float tileSize = mGameState.map->getTileSize();
    Vector2 spawnPos = { mGameState.map->getLeftBoundary() + 2 * tileSize ,
                         mGameState.map->getBottomBoundary() - 2 * tileSize };
    // Assets from @see https://sscary.itch.io/the-adventurer-female
    mGameState.xochitl = new Entity(
        spawnPos, // position
        { 100.0f, 100.0f },             // scale
        "assets/game/walk.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 8, 4 },                                 // atlas dimensions
        xochitlAnimationAtlas,                    // actual atlas
        PLAYER                                    // entity type
    );
    mGameState.bird = new Entity(
        spawnPos, // position
        { 150.0f, 100.0f },             // scale
        "assets/game/bird.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 2, 4 },                                 // atlas dimensions
            BirdAnimationAtlas,                    // actual atlas      // i dont know why i indented here but its all messed up and oh well
            BIRD                                    // entity type
            );
            mGameState.bird->setAIType(FOLLOWER);
            mGameState.bird->setColliderDimensions({ 0.f,0.f });
            mGameState.bird->setSpeed(150);

            mGameState.blaster = new Entity(
                { mOrigin.x - 300, mOrigin.y - 200.0f }, // position
                { 200.f , 200.0f },                      // scale
                "assets/game/blaster.png",                // texture file address
                ATLAS,                                    // single image or atlas?
                { 3, 4 },                                 // atlas dimensions
                blasterAnimationAtlas,
                BLASTER                                    // entity type
            );
            mGameState.blaster->deactivate();

            for (int i = 0; i < 5; i++) {
                mGameState.bullets[i] = new Entity(
                    { mOrigin.x - 300, mOrigin.y - 200.0f }, // position
                    { 50.0f , 50.0f },             // scale
                    "assets/game/blast.png",                   // texture file address
                    ATLAS,                                    // single image or atlas?
                    { 1, 3 },                                 // atlas dimensions
                    blastAnimationAtlas,
                    BLAST                                    // entity type
                );
            }


            mGameState.chest1 = new Entity(
                { spawnPos.x + 108, spawnPos.y - 70 }, // position
                { 50.0f , 50.0f },             // scale
                "assets/game/tile_0000.png",                   // texture file address
                ATLAS,                                    // single image or atlas?
                { 2, 1 },                                 // atlas dimensions
                chestAnimationAtlas,                    // actual atlas
                BLOCK                                    // entity type
            );

            for (int i = 0; i < 5; i++) {

                mGameState.enemies[i] = new Entity(
                    spawnPos, // position
                    { 100.f, 100.f },             // scale
                    "assets/game/zed.png",                   // texture file address
                    ATLAS,                                    // single image or atlas?
                    { 3, 4 },                                 // atlas dimensions
                    xochitlAnimationAtlas,                    // actual atlas
                    NPC                                    // entity type
                );

                //mGameState.enemis[i]->setColliderDimensions({
                    //mGameState.enemies[i]->getScale().x / 3.5f,
                    //mGameState.enemies[i]->getScale().y / 3.0f
                    //});
                if (i == 0) {
                    mGameState.enemies[i]->setAIType(SNIPER);
                    mGameState.enemies[i]->setAIState(IDLE);
                    mGameState.enemies[i]->setPosition(spawnPos4);
                    mGameState.enemies[i]->setColliderDimensions({
                        mGameState.xochitl->getScale().x / 3.5f,
                        mGameState.xochitl->getScale().y / 3.0f
                        });
                }
                else {
                    mGameState.enemies[i]->setAIType(FOLLOWER);
                    mGameState.enemies[i]->setAIState(IDLE);
                    //mGameState.enemies[i]->setPosition(spawnPos4);
                    mGameState.enemies[i]->setColliderDimensions({
                        mGameState.xochitl->getScale().x / 3.5f,
                        mGameState.xochitl->getScale().y / 3.0f
                        });
                }
                if (i != 0)
                    mGameState.enemies[i]->deactivate();
            }
            for (int i = 0; i < 5; i++) {
                mGameState.bullets[i]->setSpeed(1200);
                mGameState.bullets[i]->deactivate();
            }
            //mGameState.blast1->setSpeed(800);
            mGameState.blaster->setDirection(SRR);
            //mGameState.blast1->deactivate();
            //mGameState.xochitl->setJumpingPower(550.0f);
            mGameState.xochitl->setColliderDimensions({
               mGameState.xochitl->getScale().x / 1.5f,
               mGameState.xochitl->getScale().y / 1.1f
                });
            //mGameState.xochitl->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

            /*
               ----------- CAMERA -----------
            */
            mGameState.camera = { 0 };                                    // zero initialize
            mGameState.camera.target = mGameState.xochitl->getPosition(); // camera follows player
            mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
            mGameState.camera.rotation = 0.0f;                            // no rotation
            mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    timeChange = deltaTime;

    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition() };
    Vector2 currentChestPosition = { mGameState.chest1->getPosition() };

    if (currentPlayerPosition.x > -100.f && trap) {
        mGameState.enemies[1]->activate();
        mGameState.enemies[1]->setPosition({ currentPlayerPosition.x + 400 ,currentPlayerPosition.y - 200});
        mGameState.enemies[1]->setSpeed(50);
        mGameState.enemies[2]->activate();
        mGameState.enemies[2]->setPosition({ currentPlayerPosition.x + 400 ,currentPlayerPosition.y + 200 });
        mGameState.enemies[2]->setSpeed(50);
        
        trap = false;

   }
    if (currentPlayerPosition.x > 405.f && kills < 15) {
        if (!mGameState.enemies[1]->isActive()) {
            kills += 1;
            mGameState.enemies[1]->activate();
            mGameState.enemies[1]->setSpeed(50);
            mGameState.enemies[1]->setPosition({ 100.f, 100.f });
        } else if (!mGameState.enemies[2]->isActive()) {
            kills += 1;
            mGameState.enemies[2]->activate();
            mGameState.enemies[2]->setSpeed(50);
            mGameState.enemies[2]->setPosition({ 100.f, 500.f });
        } else if (!mGameState.enemies[3]->isActive()) {
            kills += 1;
            mGameState.enemies[3]->activate();
            mGameState.enemies[3]->setSpeed(50);
            mGameState.enemies[3]->setPosition({ 900.f, 500.f });
        } else if (!mGameState.enemies[4]->isActive()) {
            kills += 1;
            mGameState.enemies[4]->activate(); 
            mGameState.enemies[4]->setSpeed(50);
            mGameState.enemies[4]->setPosition({ 900.f, 100.f });
        }
    }
    //if (mGameState.xochitl->isColliding(mGameState.enemies[i]))
   if (!mGameState.chest1->isOpen()) {
       Entity* collidables[1] = { mGameState.chest1 };
       mGameState.xochitl->update(
           deltaTime,      // delta time / fixed timestep
           nullptr,        // player
           mGameState.map, // map
           collidables,        // collidable entities
           1               // col. entity count
       );
   }
   else {
       mGameState.xochitl->update(
           deltaTime,      // delta time / fixed timestep
           nullptr,        // player
           mGameState.map, // map
           mGameState.enemies,        // collidable entities
           5               // col. entity count
       );
   }
   

   mGameState.bird->update(
       deltaTime,      // delta time / fixed timestep
       mGameState.xochitl,        // player
       nullptr, // map
       nullptr,        // collidable entities
       0               // col. entity count
   );

   mGameState.blaster->update(
       deltaTime,      // delta time / fixed timestep
       mGameState.xochitl,        // player
       nullptr, // map
       nullptr,        // collidable entities
       0               // col. entity count
   );
   Direction gunDir = mGameState.blaster->getDirection();
   //printf( "pos: %f\n", currentPlayerPosition.x);
   if ((mGameState.xochitl->counterTime < mGameState.xochitl->dashTime) && !mGameState.xochitl->canDash) {
       mGameState.xochitl->setSpeed(600);
       PlaySound(mGameState.jumpSound);
   }
   else mGameState.xochitl->setSpeed(200);
   if (!mGameState.xochitl->canDash) {
       mGameState.xochitl->counterTime += deltaTime;
       if (mGameState.xochitl->counterTime > 3.f) {
           mGameState.xochitl->counterTime = 0.f;
           mGameState.xochitl->canDash = true;
       }
   }

   if (!mGameState.blaster->canBeShot()) {
       coolDownTimer += deltaTime;
       //Direction dir = mGameState.blaster->getDirection();
       if (coolDownTimer > 5.f && (gunDir == SRD || gunDir == SRL || gunDir == SRR || gunDir == SRU)) {
           coolDownTimer = 0.f;
           mGameState.blaster->shotCooldownOver();
       } else if (coolDownTimer > 3.f && (gunDir == SGD || gunDir == SGL || gunDir == SGR || gunDir == SGU)) {
           coolDownTimer = 0.f;
           mGameState.blaster->shotCooldownOver();
       } else if (coolDownTimer > 0.5f && (gunDir == ARD || gunDir == ARL || gunDir == ARR || gunDir == ARU)) {
           coolDownTimer = 0.f;
           mGameState.blaster->shotCooldownOver();
       }
   }
   else if (mGameState.blaster->wantToShoot()) {
       //printf("bang in update\n");
       mGameState.blaster->tryToShoot(mGameState.bullets);
       PlaySound(mGameState.punchSound);
       //mGameState.blast1->activate();
   }

   for (int i = 0; i < 5; i++) {
       mGameState.bullets[i]->update(
           deltaTime,      // delta time / fixed timestep
           nullptr,        // player
           nullptr, // map
           mGameState.enemies,        // collidable entities
           5               // col. entity count
       );
       Vector2 bulletPos = mGameState.bullets[i]->getPosition();
       if (mGameState.bullets[i]->isActive() && bulletPos.x > mGameState.map->getRightBoundary() || bulletPos.y > mGameState.map->getBottomBoundary() ||
           bulletPos.x < mGameState.map->getLeftBoundary() || bulletPos.y < mGameState.map->getTopBoundary()) {
           mGameState.bullets[i]->setMovement({ 0.f,0.f });
           //mGameState.blast1->setPosition({ 0.f,0.f });
           mGameState.bullets[i]->deactivate();
       }
       if (mGameState.bullets[i]->isActive() && (gunDir == SGR || gunDir == SGD ||
           gunDir == SGU || gunDir == SGL)) {
           //Vector2 bulletSamp = mGameState.bullets[5]->getPosition();
           if (hypot(currentPlayerPosition.x - bulletPos.x, currentPlayerPosition.y - bulletPos.y) > 200) {
               mGameState.bullets[i]->setMovement({ 0.f,0.f });
               //mGameState.blast1->setPosition({ 0.f,0.f });
               mGameState.bullets[i]->deactivate();
           }
       }
   }
   for (int i = 0; i < 5; i++) {
       mGameState.enemies[i]->update(
           deltaTime,      // delta time / fixed timestep
           mGameState.xochitl,        // player
           mGameState.map, // map
           nullptr,        // collidable entities
           0               // col. entity count
       );
       if (mGameState.xochitl->isColliding(mGameState.enemies[i]))
           hit = true;
   }

   if (mGameState.xochitl->getPosition().y > 800.0f) mGameState.nextSceneID = 1;

   /*
   if (currentPlayerPosition.x > currentChestPosition.x + 100) {
       mGameState.xochitl->mSecondBarrier = true;
   }*/

   if ((currentPlayerPosition.x > mGameState.map->getRightBoundary() - 400) && kills > 14) {
       mGameState.nextSceneID = 2;
   }

   if (hit) {
       PlaySound(mGameState.ouchSound);
       hit = false;
       trap = true;
       mGameState.lives -= 1;
       if (mGameState.lives > 0)
           mGameState.nextSceneID = 1; // to reset // will eventually be 1
       if (mGameState.lives == 0) {
           mGameState.xochitl->youSuck = true;
           PlaySound(mGameState.loseSound);
           mGameState.nextSceneID = 0;
       }
   }
   //printf("\n %f", mGameState.xochitl->getPosition().y);
   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();

   mGameState.chest1->render();

   mGameState.bird->render();

   for (int i = 0; i < 5; i++)
       mGameState.bullets[i]->render();

   for (int i = 0; i < 5; i++) {
       mGameState.enemies[i]->render();
       //if (i == 0) {
           //printf("Enemy X:%f  , Ememy Y %f\n", mGameState.enemies[i]->getPosition().x, mGameState.enemies[i]->getPosition().y);
           //printf("My X:%f  , My Y %f\n\n", mGameState.xochitl->getPosition().x, mGameState.xochitl->getPosition().y);
       //}
   }

   Direction dir = mGameState.xochitl->getDirection();
   if (dir == UP || dir == RIGHT || dir == UPRIGHT || dir == DOWNRIGHT) {
       mGameState.blaster->render(); mGameState.xochitl->render();
       
   }
   else if (dir == DOWN || dir == LEFT || dir == UPLEFT || dir == DOWNLEFT) {
       
       mGameState.xochitl->render(); mGameState.blaster->render();
   }
   for (int i = 0; i < 5; i++) {
       
       if (mGameState.enemies[i]->isActive() && mGameState.enemies[i]->isAiming()) {
           mGameState.enemies[i]->aimingTimer += timeChange;
           if (mGameState.enemies[i]->aimingTimer < 8.f) {
               DrawLineEx(mGameState.enemies[i]->getPosition(), mGameState.xochitl->getPosition(), mGameState.enemies[i]->aimingTimer * 5.f, RED);
               //printf("distance: %f \n", (mGameState.enemies[i]->getPosition() - mGameState.xochitl->getPosition()));
           }
           else {
               //printf("Distance: %f", (mGameState.enemies[i]->getPosition().x - mGameState.xochitl->getPosition().x));
               mGameState.enemies[i]->aimingTimer = 0.f;
               hit = true;
               //mGameState.xochitl
           }
       }
   }
   
   //mGameState.xochitl->render();
   //mGameState.chest1->render();
   //mGameState.chest1->displayCollider();
   //mGameState.map->render();
   //mGameState.blaster->render();
   

   Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition()};
   Vector2 currentChestPosition = { mGameState.chest1->getPosition()};
   //printf("value: %f\n", abs(currentChestPosition.x - currentPlayerPosition.x));
   if (abs(currentChestPosition.x - currentPlayerPosition.x) < 80.f && abs(currentChestPosition.y - currentPlayerPosition.y) < 80.f 
       && !mGameState.chest1->isOpen()) {
       DrawText("PRESS F TO OPEN", currentPlayerPosition.x, currentPlayerPosition.y, 30, RAYWHITE);
   }
}

void LevelA::shutdown()
{
    
   delete mGameState.xochitl;
   delete mGameState.map;
   delete mGameState.blaster;
   //delete mGameState.blast1;
   delete mGameState.chest1;
   for (int i = 0; i < 5; i++)
       delete mGameState.enemies[i];

   //delete mGameState.enemies; 
   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.loseSound);
   UnloadSound(mGameState.punchSound);
   UnloadSound(mGameState.ouchSound);


   for (int i = 0; i < 5; i++)
       delete mGameState.bullets[i];

   //delete mGameState.bullets;

   //UnloadMusicStream(mGameState.bgm);
   //UnloadSound(mGameState.jumpSound);
}