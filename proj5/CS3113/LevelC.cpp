#include "LevelC.h"
#include <cassert>
LevelC::LevelC() : Scene{ {0.0f}, nullptr } {}
LevelC::LevelC(Vector2 origin, const char* bgHexCode)
    : Scene{ origin, bgHexCode } {
}

LevelC::~LevelC() { shutdown(); }



float endTimer = 10.f;
bool cHit = false;
float CtimeChange = 0.f;
void LevelC::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/game/Semi Modular NYE - Joe Esposito.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/ninjaJump.wav");
    mGameState.ouchSound = LoadSound("assets/game/hurt.wav");
    mGameState.punchSound = LoadSound("assets/game/punch.wav");
    mGameState.loseSound = LoadSound("assets/game/lose.wav");

    mGameState.nextSceneID = -1;
    endTimer = 10.f;
    CtimeChange = 0.f;

    // ---- AUDIO ----
    mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

    // ---- MAP ----
    mGameState.map = new Map(
        LEVELC_WIDTH, LEVELC_HEIGHT,
        (unsigned int*)mLevelData,
        "assets/game/mytileset.png",
        TILE_DIMENSION,
        8, 1,
        mOrigin
    );

    // ---- PLAYER / BOSS ANIMATIONS ----
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

    std::map<Direction, std::vector<int>> bossAnimationAtlas = {
        {LEFT,  {0,1,2,3}},
        {RIGHT, {0,1,2,3}},
        {UP,    {0,1,2,3}},
        {DOWN,  {0,1,2,3}}
    };

    float tileSize = mGameState.map->getTileSize();

    Vector2 playerSpawn = {
        mGameState.map->getLeftBoundary() + 2 * tileSize,
        mGameState.map->getBottomBoundary() - 2 * tileSize
    };

    Vector2 bossSpawn = {
        (mGameState.map->getLeftBoundary() + mGameState.map->getRightBoundary()) * 0.5f,
        mGameState.map->getTopBoundary() + 3 * tileSize
    };

    // ---- PLAYER ----
    mGameState.xochitl = new Entity(
        playerSpawn,
        { 100.0f, 100.0f },
        "assets/game/walk.png",
        ATLAS,
        { 8, 4 },
        xochitlAnimationAtlas,
        PLAYER
    );
    mGameState.xochitl->setColliderDimensions({
        mGameState.xochitl->getScale().x / 1.5f,
        mGameState.xochitl->getScale().y / 1.1f
        });
    mGameState.xochitl->setSpeed(400);
    mGameState.xochitl->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    // ---- BLASTER + BULLETS ----
    mGameState.blaster = new Entity(
        playerSpawn,
        { 200.0f, 200.0f },
        "assets/game/blaster.png",
        ATLAS,
        { 3, 4 },
        blasterAnimationAtlas,
        BLASTER
    );
    mGameState.blaster->activate();
    mGameState.blaster->setDirection(SRR);

    for (int i = 0; i < 5; i++) {
        mGameState.bullets[i] = new Entity(
            playerSpawn,
            { 50.0f, 50.0f },
            "assets/game/blast.png",
            ATLAS,
            { 1, 3 },
            blastAnimationAtlas,
            BLAST
        );
        mGameState.bullets[i]->setSpeed(1200);
        mGameState.bullets[i]->deactivate();
    }

    // ---- BOSS ----
    for (int i = 0; i < 5; i++) {
        mGameState.enemies[i] = nullptr;
    }

    mGameState.enemies[0] = new Entity(
        bossSpawn,
        { 200.0f, 200.0f },
        "assets/game/boss.png",   // 
        ATLAS,
        { 1, 10 },                 // 
        bossAnimationAtlas,
        NPC
    );
    mGameState.enemies[0]->setAIType(FOLLOWER);      // simple 
    mGameState.enemies[0]->setAIState(FOLLOWING);
    mGameState.enemies[0]->setSpeed(30);
    mGameState.enemies[0]->health = 50;              // tougher than normal
    mGameState.enemies[0]->setColliderDimensions({
        mGameState.enemies[0]->getScale().x / 1.5f,
        mGameState.enemies[0]->getScale().y / 1.5f
        });

    // ---- CAMERA ----
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;

    assert(mGameState.map != nullptr);
    assert(mGameState.xochitl != nullptr);
    assert(mGameState.blaster != nullptr);
    for (int i = 0; i < 5; ++i) {
        assert(mGameState.bullets[i] != nullptr);
    }
    assert(mGameState.enemies[0] != nullptr);
}

void LevelC::update(float deltaTime)
{
    CtimeChange = deltaTime;
    if (mGameState.enemies[0]->health < 0) { // what the hell is happening
        endTimer -= deltaTime;
    }
    if (endTimer < 0) {
        mGameState.nextSceneID = 4;
    }
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        mGameState.enemies,
        1
    );
    if (!mGameState.enemies[0]->isActive()) { // what the hell is happening x2
        mGameState.nextSceneID = 4;
        mGameState.winner = true;
    }

    
    if (mGameState.enemies[0] && mGameState.enemies[0]->isActive()) {
        mGameState.enemies[0]->update(
            deltaTime,
            mGameState.xochitl,
            mGameState.map,
            nullptr,
            0
        );
    }

   
    mGameState.blaster->update(
        deltaTime,
        mGameState.xochitl,
        nullptr,
        nullptr,
        0
    );

    for (int i = 0; i < 5; i++) {
        mGameState.bullets[i]->update(
            deltaTime,
            nullptr,
            nullptr,
            mGameState.enemies,
            1
        );
    }
    Direction gunDir = mGameState.blaster->getDirection();

    // dash 
    if ((mGameState.xochitl->counterTime < mGameState.xochitl->dashTime) &&
        !mGameState.xochitl->canDash) {
        PlaySound(mGameState.jumpSound);
        mGameState.xochitl->setSpeed(600);
    }
    else {
        mGameState.xochitl->setSpeed(200);
    }

    // dash cooldown 
    if (!mGameState.xochitl->canDash) {
        mGameState.xochitl->counterTime += deltaTime;
        if (mGameState.xochitl->counterTime > 3.0f) {
            mGameState.xochitl->counterTime = 0.0f;
            mGameState.xochitl->canDash = true;
        }
    }

    // Win if  boss dead 
    if (mGameState.enemies[0] && !mGameState.enemies[0]->isActive()) {
        mGameState.nextSceneID = 0;  //
    }

    Vector2 currentPlayerPosition = mGameState.xochitl->getPosition();
    panCamera(&mGameState.camera, &currentPlayerPosition);

    if (!mGameState.blaster->canBeShot()) {
        coolDownTimer += deltaTime;
        //Direction dir = mGameState.blaster->getDirection();
        if (coolDownTimer > 5.f && (gunDir == SRD || gunDir == SRL || gunDir == SRR || gunDir == SRU)) {
            coolDownTimer = 0.f;
            mGameState.blaster->shotCooldownOver();
        }
        else if (coolDownTimer > 3.f && (gunDir == SGD || gunDir == SGL || gunDir == SGR || gunDir == SGU)) {
            coolDownTimer = 0.f;
            mGameState.blaster->shotCooldownOver();
        }
        else if (coolDownTimer > 0.5f && (gunDir == ARD || gunDir == ARL || gunDir == ARR || gunDir == ARU)) {
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

    float left = mGameState.map->getLeftBoundary(); // this is better than the BS i was doing in levelB but oh well ill leave it
    float right = mGameState.map->getRightBoundary();
    float top = mGameState.map->getTopBoundary();
    float bottom = mGameState.map->getBottomBoundary();

    for (int i = 0; i < 5; i++) {
        if (!mGameState.bullets[i]->isActive()) continue;

        Vector2 bulletPos = mGameState.bullets[i]->getPosition();
        if (bulletPos.x < left || bulletPos.x > right ||
            bulletPos.y < top || bulletPos.y > bottom) {
            mGameState.bullets[i]->setMovement({ 0.0f, 0.0f });
            mGameState.bullets[i]->deactivate();
        }
    }

    if (mGameState.enemies[0] &&
        mGameState.enemies[0]->isActive() &&
        mGameState.xochitl->isColliding(mGameState.enemies[0])) {
        cHit = true;
    }

    if (cHit) {
        cHit = false;
        PlaySound(mGameState.ouchSound);
        mGameState.lives -= 1;
        if (mGameState.lives > 0) {
            mGameState.nextSceneID = 3;   //  reload LevelC
        }
        else {
            mGameState.xochitl->youSuck = true;
            PlaySound(mGameState.loseSound);
            mGameState.nextSceneID = 0;   // back to title
        }
    }
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    mGameState.map->render();

    mGameState.xochitl->render();
    mGameState.blaster->render();

    for (int i = 0; i < 5; i++) {
        if (mGameState.bullets[i]->isActive())
            mGameState.bullets[i]->render();
    }

    if (mGameState.enemies[0] && mGameState.enemies[0]->isActive()) {
        mGameState.enemies[0]->render();
    }

    //Draw
    //renderWin();
}

void LevelC::shutdown()
{
    if (mGameState.map)      delete mGameState.map;
    if (mGameState.xochitl)  delete mGameState.xochitl;
    if (mGameState.blaster)  delete mGameState.blaster;

    for (int i = 0; i < 5; i++) {
        if (mGameState.bullets[i]) delete mGameState.bullets[i];
        if (mGameState.enemies[i]) delete mGameState.enemies[i];
    }

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.loseSound);
    UnloadSound(mGameState.punchSound);
    UnloadSound(mGameState.ouchSound);

}
