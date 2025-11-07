//#include "CS3113/LevelB.h"
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
    gLevelB = new LevelB(ORIGIN, "#C0897E");

    gLevels.push_back(gLevelStart);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentScene->getType() == START) {
        if (IsKeyPressed(KEY_L)) {
            gCurrentScene->setType();
        }
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }
    if (gCurrentScene->getState().ninja->getAIState() == PUNCH) {
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }

    gCurrentScene->getState().ninja->resetMovement();

    if (IsKeyPressed(KEY_E)) {
        gCurrentScene->getState().ninja->setAIState(PUNCH);
        gCurrentScene->getState().ninja->attackRight();
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    } 
    if (IsKeyPressed(KEY_Q)) {
        gCurrentScene->getState().ninja->setAIState(PUNCH);
        gCurrentScene->getState().ninja->attackLeft();
        if (IsKeyPressed(27) || WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().ninja->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().ninja->moveRight(); 

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

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}