/**
* Author: Farr Dickson
* Assignment:  
* Date due: 11/26, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/StartScreen.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/ShaderProgram.h"

//#define SDLK_LSHIFT                 0x400000e1u

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 4;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

bool skip = false;

int dialogueCounter;
int currentWeapon = 1;

//float gGameClock;
ShaderProgram gShader;
Vector2 gLightPosition;
Effects* gEffects = nullptr;
// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gDashCoolDown    = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

StartScreen* gLevelStart = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC* gLevelC = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void renderDialogueSwitch() { // thats fucking right were doing this undertale style
    switch (dialogueCounter) {
    case 0:
        DrawRectangle(0, 0, 1000, 600, BLACK);
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("..... arise ..... hmmmhmmmhmm ......", 40, 450, 50, DARKGREEN);
        DrawText("PRESS SPACE TO CONTINUE", 40, 510, 20, RAYWHITE);
        return;
    case 1:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("..yes ... YES .... open your eyes! I", 40, 450, 50, DARKGREEN); 
        DrawText("know you must be confused -", 40, 510, 50, DARKGREEN);
        return;
    case 2:
        DrawRectangle(10, 400, 980, 190, RED);
        DrawText("*A shrill warcry echoes from nearby*", 40, 450, 45, BLACK);
        return;
    case 3:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("I am sorry there is no time I'll", 40, 450, 50, DARKGREEN);
        DrawText("explain when we get out this place", 40, 510, 50, DARKGREEN);
        return;
    case 4:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("Look! There is a rifle by the wall ...", 40, 450, 50, DARKGREEN);
        DrawText("cmon go grab it ...", 40, 510, 50, DARKGREEN);
        return;
    case 5:
        return;
    case 6:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("Alright lets get out of here.", 40, 450, 50, DARKGREEN);
        DrawText("CLICK MOUSE TO FIRE", 60, 510, 30, RAYWHITE);
        return;
    case 7:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("Be careful there are enemies everywhere.", 40, 450, 40, DARKGREEN);
        DrawText("Reviving you took a lot of energy-", 40, 510, 40, DARKGREEN);
        //DrawText("they see you.", 40, 510, 50, DARKGREEN);
        return;

    case 8:
        return;
        
    case 9:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("Watch out for snipers targetting you.", 40, 450, 40, DARKGREEN);
        DrawText("If you dont kill them or run away-", 40, 510, 40, DARKGREEN);
        return;

    case 10:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("before they fully charge up ... lets", 40, 450, 40, DARKGREEN);
        DrawText("just say they hardly ever miss.", 40, 510, 40, DARKGREEN);
        return;

    case 11:
        return;

    case 12:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("Good job- wait were surrounded!", 40, 450, 40, DARKGREEN);
        DrawText("Dash back and pull out your shotgun!", 40, 510, 40, DARKGREEN);
        return;

    case 13:
        DrawRectangle(10, 400, 980, 190, PURPLE); 
        DrawText("CLICK 1 OR 2 TO SWITCH", 60, 510, 30, RAYWHITE);
        //DrawText("before they fully charge up ... lets", 40, 450, 40, DARKGREEN);
        //DrawText("just say they hardly ever miss.", 40, 510, 40, DARKGREEN);
        return;

    case 14:
        return;

    case 15:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("CLICK 3 TO SWITCH TO AUTO", 60, 510, 30, RAYWHITE);
        DrawText("Im sensing a lot around us... ", 40, 450, 40, DARKGREEN);
        //DrawText("just say they hardly ever miss.", 40, 510, 40, DARKGREEN);
        return;

    case 16:
        return;

    case 17:
        DrawRectangle(10, 400, 980, 190, PURPLE);
        DrawText("JUST SURVIVE", 40, 450, 40, DARKGREEN);
        //DrawText("just say they hardly ever miss.", 40, 510, 40, DARKGREEN);
        return;

    default:
        return;
    }
}

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();
    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    gLevelStart = new StartScreen(ORIGIN, "#0F1C21");
    gLevelA = new LevelA(ORIGIN, "#0F1C21");
    gLevelB = new LevelB(ORIGIN, "#011627");
    gLevelC = new LevelC(ORIGIN, "#0F1C21");

    gLevels.push_back(gLevelStart);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    gEffects = new Effects(ORIGIN, SCREEN_WIDTH, SCREEN_HEIGHT);

    switchToScene(gLevels[0]);
    //gEffects->start(FADEIN);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (IsKeyPressed(KEY_P) || WindowShouldClose()) gAppStatus = TERMINATED;

    if (gCurrentScene == gLevels[0] ) {
        if (IsKeyPressed(KEY_ENTER)) {
            dialogueCounter = 0;
            switchToScene(gLevels[1]);
        }
        return;
    }

    if (gCurrentScene->getState().xochitl->youSuck) {
        if (IsKeyPressed(KEY_ENTER)) {
            switchToScene(gLevels[0]);   // back to start screen
        }
        return;                           // 
    }
    if (dialogueCounter == 1 && !skip) {
        skip = !skip;
        gEffects->start(FADEIN);
    }
        
    if (dialogueCounter < 5) {
        if (IsKeyPressed(KEY_SPACE)) {
            dialogueCounter += 1;
        }
        return;
    }
    gCurrentScene->getState().xochitl->resetMovement();

    if (gCurrentScene->getState().xochitl->canDash && IsKeyPressed(KEY_LEFT_SHIFT)) {
        gCurrentScene->getState().xochitl->canDash = false;
    }

    if (gCurrentScene->getState().blaster->isActive() && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        gCurrentScene->getState().blaster->imReallyBadAtFunctionNames();
        //printf("bang\n");
        //printf("bang?%d\n", gCurrentScene->getState().blaster->wantToShoot());
    }
    else { gCurrentScene->getState().blaster->shot(); }
        

    //printf("bool: %d \n", gCurrentScene->getState().xochitl->mSecondBarrier);
    if (dialogueCounter < 8 && ((gCurrentScene->getState().xochitl->getPosition()).x 
                              > (gCurrentScene->getState().chest1->getPosition()).x)) {
        //printf("why");
        if (dialogueCounter == 5)
            dialogueCounter += 1;
        if (IsKeyPressed(KEY_SPACE)) {
            dialogueCounter += 1;
        }
        return;
    } else if (dialogueCounter < 11 && ((gCurrentScene->getState().xochitl->getPosition()).x
                                        > -525.f)) { // about the bridge part of the map
        //printf("why");
        if (dialogueCounter == 8)
            dialogueCounter += 1;
        if (IsKeyPressed(KEY_SPACE)) {
            dialogueCounter += 1;
        }
        return;
    }
    else if (dialogueCounter < 14 && ((gCurrentScene->getState().xochitl->getPosition()).x
                                        > -100.f)) { // about the bridge part of the map
        //printf("why");
        if (dialogueCounter == 11)
            dialogueCounter += 1;
        if (IsKeyPressed(KEY_SPACE)) {
            dialogueCounter += 1;
        }
        return;
    }
    else if (dialogueCounter < 16 && ((gCurrentScene->getState().xochitl->getPosition()).x
                                    > 400.f)) { // about the bridge part of the map
        //printf("why");
        if (dialogueCounter == 14)
            dialogueCounter += 1;
        if (IsKeyPressed(KEY_SPACE)) {
            dialogueCounter += 1;
        }
        return;
    }
    else if (dialogueCounter < 18 && (gCurrentScene == gLevels[2])) { // about the bridge part of the map
        //printf("why");
        if (dialogueCounter == 16)
            dialogueCounter += 1;
        if (IsKeyPressed(KEY_SPACE)) {
            dialogueCounter += 1;
        }
        return;
    }
    //gCurrentScene->getState().xochitl->interact = false;
    if (!(gCurrentScene->getState().blaster->isActive()) &&
        gCurrentScene->getState().chest1 != nullptr) {
        Vector2 playerPos = gCurrentScene->getState().xochitl->getPosition();
        Vector2 chestPos = gCurrentScene->getState().chest1->getPosition();
        //printf("value: %f\n", abs(chestPos.x - playerPos.x));
        if (IsKeyDown(KEY_F) && abs(chestPos.x - playerPos.x) < 80.f && abs(chestPos.y - playerPos.y) < 80.f) {
            gCurrentScene->getState().chest1->open();
            gCurrentScene->getState().blaster->activate();
        }
    }

    if (IsKeyDown(KEY_Q)) {
        gCurrentScene->getState().xochitl->moveUpLeft();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, UPLEFT);
    }
    else if (IsKeyDown(KEY_E)) {
        gCurrentScene->getState().xochitl->moveUpRight();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, UPRIGHT);
    }
    else if (IsKeyDown(KEY_A)) { 
        gCurrentScene->getState().xochitl->moveLeft(); 
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, LEFT);
    }
    else if (IsKeyDown(KEY_D)) { 
        gCurrentScene->getState().xochitl->moveRight();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, RIGHT);
    }

    else if (IsKeyDown(KEY_W)) {
        gCurrentScene->getState().xochitl->moveUp();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, UP);
    }
    else if (IsKeyDown(KEY_S)) {
        gCurrentScene->getState().xochitl->moveDown();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, DOWN);
    }
    else if (IsKeyDown(KEY_Z)) {
        gCurrentScene->getState().xochitl->moveDownLeft();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, DOWNLEFT);
    }
    else if (IsKeyDown(KEY_X)) {
        gCurrentScene->getState().xochitl->moveDownRight();
        if (gCurrentScene->getState().blaster->isActive())
            gCurrentScene->getState().blaster->fixDirection(currentWeapon, DOWNRIGHT);
    }
    if (gCurrentScene->getState().blaster->isActive()) {
        gCurrentScene->getState().blaster->fixDirection(currentWeapon, gCurrentScene->getState().xochitl->getDirection());
        gCurrentScene->getState().blaster->setMousePos(GetMousePosition());
        if (IsKeyPressed(KEY_ONE)) {
            
            currentWeapon = 1;
            for (int i = 0; i < 5; i++) {
                gCurrentScene->getState().bullets[i]->setDirection(SRD);
            }
            //printf("ONE\n");
        }
        else if (IsKeyPressed(KEY_TWO)) {
            
            currentWeapon = 2;
            for (int i = 0; i < 5; i++) {
                gCurrentScene->getState().bullets[i]->setDirection(SGD);
            }
            //gCurrentScene->getState().blast1->setDirection(SGD);
            //printf("TWO\n");
        }
        else if (IsKeyPressed(KEY_THREE)) {
            
            currentWeapon = 3;
            for (int i = 0; i < 5; i++) {
                gCurrentScene->getState().bullets[i]->setDirection(ARD);
            }
            //printf("THREE\n");
        }
    }

    if (GetLength(gCurrentScene->getState().xochitl->getMovement()) > 1.0f) 
        gCurrentScene->getState().xochitl->normaliseMovement();
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

        Vector2 cameraTarget = gCurrentScene->getState().xochitl->getPosition();
        gEffects->update(FIXED_TIMESTEP, &cameraTarget);
        gLightPosition = gCurrentScene->getState().xochitl->getPosition();

        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    

    //gCurrentScene->render();
    if (gCurrentScene == gLevels[0]) {
        BeginMode2D(gCurrentScene->getState().camera);
        gCurrentScene->render();
        EndMode2D();
        EndDrawing();
        return;
    }
    if (gCurrentScene->getState().xochitl->youSuck) {
        ClearBackground(RED);
        DrawText("*You should probably retire*", 10, 250, 65, BLACK);
        EndDrawing();
        return;
    }
    BeginMode2D(gCurrentScene->getState().camera);//yes 

    gShader.begin();

    gCurrentScene->render();
    gShader.setVector2("lightPosition", gLightPosition);
    gShader.setInt(
        "isDark",
        gCurrentScene->getState().xochitl->isItDark() ? 1 : 0
    );
    gCurrentScene->render();

    gShader.end();
    if (dialogueCounter == 1)
        gEffects->render();

    EndMode2D();
    renderDialogueSwitch();
    EndDrawing();
}

void shutdown() 
{
    delete gLevelStart;
    delete gLevelA;
    delete gLevelB;
    delete gEffects;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
    gShader.unload();
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