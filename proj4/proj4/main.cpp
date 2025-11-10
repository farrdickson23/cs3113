#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/BossLevel.h"
#include "CS3113/StartScreen.h"
// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

StartScreen *gLevelStart = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC* gLevelC = nullptr;
BossLevel* gBLevel = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gLevelStart = new StartScreen(ORIGIN, "#D12E0F");
    gLevelA = new LevelA(ORIGIN, "#011627");
    gLevelB = new LevelB(ORIGIN, "#AE3018");
    gLevelC = new LevelC(ORIGIN, "#52186C");
    gBLevel = new BossLevel(ORIGIN, "#1F1B21");

    gLevels.push_back(gLevelStart);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);
    gLevels.push_back(gBLevel);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentScene->getType() == START) {
        if (IsKeyPressed(KEY_ENTER)) {
            gCurrentScene->setType();
            gCurrentScene->getState().lives = 3;
        }
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }
    if (gCurrentScene->getState().ninja->getAIState() == PUNCHR || gCurrentScene->getState().ninja->getAIState() == PUNCHL) {
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }

    gCurrentScene->getState().ninja->resetMovement();

    if (IsKeyPressed(KEY_E)) {
        gCurrentScene->getState().ninja->setAIState(PUNCHR);
        gCurrentScene->getState().ninja->attackRight();
        PlaySound(gCurrentScene->getState().punchSound);
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    } 
    if (IsKeyPressed(KEY_Q)) {
        gCurrentScene->getState().ninja->setAIState(PUNCHL);
        gCurrentScene->getState().ninja->attackLeft();
        PlaySound(gCurrentScene->getState().punchSound);
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }

    if (IsKeyDown(KEY_A)) { 
        gCurrentScene->getState().ninja->moveLeft(); 
        //printf("Key A Press\n");
    }
    else if (IsKeyDown(KEY_D)) { 
        gCurrentScene->getState().ninja->moveRight(); 
        //printf("Key D Press\n");
    }

    if (IsKeyPressed(KEY_W) && 
        gCurrentScene->getState().ninja->isCollidingBottom())
    {
        gCurrentScene->getState().ninja->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().ninja->getMovement()) > 1.0f) 
        gCurrentScene->getState().ninja->normaliseMovement();

    if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    delete gLevelStart;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;
    delete gBLevel;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        // this scene switching stuff is really confisuing me i think this fix should help
        int next = gCurrentScene->getState().nextSceneID;
        if (next >= 0) // start screen was 0 for me so yyes // i also should handle lives count here 
        {
            int lives = gCurrentScene->getState().lives;
            switchToScene(gLevels[next]);
            // int id = gCurrentScene->getState().nextSceneID; // idt i need this
            // switchToScene(gLevels[id]); // naur
            gCurrentScene->getState().lives = lives;

            gCurrentScene->getState().nextSceneID = -1;
        }

        render();
    }

    shutdown();

    return 0;
}