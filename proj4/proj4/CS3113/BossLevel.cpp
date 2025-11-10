#include "BossLevel.h"

BossLevel::BossLevel() : Scene{ {0.0f}, nullptr } {}
BossLevel::BossLevel(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

BossLevel::~BossLevel() { shutdown(); }

void BossLevel::initialise()
{
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
        BLEVEL_WIDTH, BLEVEL_HEIGHT,   // map grid cols & rows
        (unsigned int*)mLevelData,   // grid data
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
       {RIGHT, {  24, 25, 26, 27 }}
    };

    float sizeRatio = 48.0f / 48.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    mGameState.ninja = new Entity(
        { mOrigin.x - 600.0f, mOrigin.y + 220.0f }, // position
        { 150.0f * sizeRatio, 150.0f },             // scale
        "assets/game/mc.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 6, 6 },                                 // atlas dimensions
        ninjaAnimationAtlas,                    // actual atlas
        PLAYER                                    // entity type
    );

    mGameState.ninja->setJumpingPower(550.0f);
    mGameState.ninja->setColliderDimensions({
       mGameState.ninja->getScale().x / 3.0f,
       mGameState.ninja->getScale().y / 1.1f
        });
    mGameState.ninja->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.ninja->setFrameSpeed(3);

    /*
       ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };                                    // zero initialize
    mGameState.camera.target = mGameState.ninja->getPosition(); // camera follows player
    mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
    mGameState.camera.rotation = 0.0f;                            // no rotation
    mGameState.camera.zoom = 1.0f;                                // default zoom

    std::map<Direction, std::vector<int>> monsterAnimationAtlas = {
      {LEFT,  {  0, 1, 2, 3 }},
      {RIGHT, {  0, 1, 2, 3 }},
      {IDLE, {  0, 1, 2, 3 }},
      {DDEAD, { 4, 5, 6, 7, 8, 9}}
    };

    mGameState.monster1 = new Entity(
        { mOrigin.x , mOrigin.y + 150.0f }, // position
        { 350.0f * sizeRatio, 350.0f },             // scale
        "assets/game/boss.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 1, 10 },                                 // atlas dimensions
        monsterAnimationAtlas,                    // actual atlas
        NPC                                       // entity type
    );
    mGameState.monster1->setColliderDimensions({
       mGameState.monster1->getScale().x / 1.2f,
       mGameState.monster1->getScale().y / 1.2f
        });
    mGameState.monster1->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    //mGameState.monster1->setJumpingPower(500.f);

    mGameState.monster1->setAIType(BOSS);
    mGameState.monster1->setAIState(SIDLE);

    mGameState.monster1->setSpeed(Entity::DEFAULT_SPEED); // the monster is going to fast
    mGameState.monster1->setFrameSpeed(3);
}

void BossLevel::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    if (mPlayerWon || mPlayerLost)
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
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    if ((mGameState.ninja->isColliding(mGameState.monster1)) && mGameState.monster1->getAIState() != DEAD) // update so you cant be killed after boss is dead
    {
        mGameState.lives = mGameState.lives - 1;

        //std::cout << "HIT! lives = " << mGameState.lives
        //    << "  nextSceneID(before) = " << mGameState.nextSceneID << std::endl;

        if (mGameState.lives > 0) { // reset the level
            PlaySound(mGameState.ouchSound);
            mGameState.nextSceneID = 4;
        }
        else // go to you lose
        {
            PlaySound(mGameState.loseSound);
            mPlayerLost = true;
        }
        //std::cout << "after set: nextSceneID = " << mGameState.nextSceneID << std::endl;
    }
    // i now need to make a punch work somehow ._.

    if (mGameState.monster1->getAIState() != DEAD)
    {
        AIState cState = mGameState.ninja->getAIState(); // current state so i dont need to continuosly call ninja get ai state
        bool isPunching = (cState == PUNCHL || cState == PUNCHR);

        Vector2 nPos = mGameState.ninja->getPosition(); // same with these so i dont have to recal get position and collider dimensions
        Vector2 nCol = mGameState.ninja->getColliderDimensions();
        Vector2 bPos = mGameState.monster1->getPosition();
        Vector2 bCol = mGameState.monster1->getColliderDimensions();

        if (isPunching) // check if punching
        {
            int facing = (cState == PUNCHR) ? 1 : -1; // kinda like dir

            const float PUNCH_REACH = 75.0f; // this should be global but i cant be bothered

            Vector2 punchCenter = nPos; // punch hitbox calculations using the ninja collider
            punchCenter.x += facing * (nCol.x * 0.5f + PUNCH_REACH * 0.5f);

            // similar to our hitbox calc graciously provided by prof we need to calc half collision box
            float punchHalfW = PUNCH_REACH * 0.5f; 
            float punchHalfH = nCol.y * 0.5f;
            float bossHalfW = bCol.x * 0.5f; // half of boss collision
            float bossHalfH = bCol.y * 0.5f;

            // i need to check the boss and the punch are overlapping
            float dx = fabs(punchCenter.x - bPos.x) - (punchHalfW + bossHalfW); 
            float dy = fabs(punchCenter.y - bPos.y) - (punchHalfH + bossHalfH);

            // is the boss hit by the punch?
            bool punchHitsBoss = (dx < 0.0f && dy < 0.0f);

            // i only want to get the hit from the left because it is a freezing punch this is the "boss mechanic"
            bool fromRight = (cState == PUNCHL && nPos.x > bPos.x);

            // check if both the things are done and then kill boss
            if (punchHitsBoss && fromRight)
            {
                // Boss death animation ?!? hopefully
                mGameState.monster1->setAIState(DEAD);
                mGameState.monster1->setDirection(DDEAD); // DDEAD frames in atlas

                mGameState.monster1->isDying();
            }
        }
        if (mGameState.monster1->getAIState() == DEAD)  //horray we finally got here
            mPlayerWon = true;
    }


    // CAMERA
    Vector2 currentPlayerPosition = mGameState.ninja->getPosition();
    //if (mGameState.ninja->getPosition().y > 800.0f) mGameState.nextSceneID = 3; // this should go to boss scene
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void BossLevel::render()
{
    //if (mGameState.contNext && (mGameState.monster1->getAIState() == DEAD)

    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.ninja->render();
    
    mGameState.map->render();
    mGameState.monster1->render();

    if (mPlayerLost)
        DrawText("YOU LOSE BOZO!!", mGameState.ninja->getPosition().x, mGameState.ninja->getPosition().y+30, 40, RED);
    else if (mPlayerWon)
        DrawText("YOU WIN!!", mGameState.ninja->getPosition().x, mGameState.ninja->getPosition().y +30, 40, GREEN);
}

void BossLevel::shutdown()
{
    delete mGameState.ninja;
    delete mGameState.monster1;
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.loseSound);
    UnloadSound(mGameState.punchSound);
    UnloadSound(mGameState.ouchSound);
}