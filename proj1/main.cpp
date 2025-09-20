/**
* Author: Farr Dickson
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct. The only vibe coding I've done is 
* coding based off my own vibes. Until I got to your class 
* I thought vibe coding meant like lazy coding based off 
* how you feel without rules I didn't know about the AI 
* thing.
**/

#include "raylib.h"
#include <math.h>

// Enums
enum AppStatus { TERMINATED, RUNNING };


// Global Constants
constexpr int SCREEN_WIDTH        = 800 * 1.5f,
              SCREEN_HEIGHT       = 450 * 1.5f,
              FPS                 = 60;

constexpr Vector2 CENTER = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE = { 100.f, 100.f };
constexpr Vector2 FLAGSPOT = { 100.f, SCREEN_HEIGHT / 2 };


// Global Variables
Vector2 cPosition = { 1000.f, SCREEN_HEIGHT/2 };
Vector2 hPosition = CENTER;
Vector2 rScale = {1000.f, 1000.f};
Vector2 cScale = BASE_SIZE;

AppStatus gAppStatus   = RUNNING;

float     cAngle = 0.f,
          hAngle = 0.f,
          hOffsetA = 0.0f,
          fAngle = 0.f,
          gPreviousTicks = 0.f,
          cClock = 0.f,
          gClock = 0.f;

int     flagDir =1; // this didnt work but im leavin git for the memory of what couldve been
int     carDir;

Color*    backgroundColor;

Texture2D cTexture;
Texture2D hTexture;
Texture2D rTexture;
Texture2D fTexture;

// believe it or not i am an incredible artist and 
// these were all made by yours truely 
constexpr char  cah[] = "cah.png",
                road[] = "road.png",
                flag[] = "flag.png",
                hooman[] = "hooman.png";
                

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rainbow road");

    cTexture = LoadTexture(cah);
    hTexture = LoadTexture(hooman);
    fTexture = LoadTexture(flag);
    rTexture = LoadTexture(road);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    float ticks = (float)GetTime();           // step 1
    float deltaTime = ticks - gPreviousTicks; // step 2
    gPreviousTicks = ticks;                   // step 3

    // heart beat esq track / road
    rScale.x = rScale.x + sin(10 * gClock); // 5 is max amplitude
    rScale.y = rScale.y + sin(10 * gClock);

   

    //300 is the amount of pixels from the center to the edge of the track
    // I had to look up the equation for this
    // https://en.wikipedia.org/wiki/Lemniscate_of_Bernoulli
    cPosition.x = ((300 * sqrt(2) * cos(cClock)) / (pow(sin(cClock), 2) + 1)) + CENTER.x;
    cPosition.y = ((300 * sqrt(2) * cos(cClock) * sin(cClock)) / (pow(sin(cClock), 2) + 1)) + CENTER.y;

    //angles to make the cah look better. messing around in desmos
    // the decimals are tunning the cah to look make turning look correct 
    // i also realize i should calculate where the hooman sits here as well
    // i also want to make the flag wave at a specific time and doing it out
    // of this loop wasnt working
    hOffsetA = 15 * sin(10 * gClock); // 15 is max amp of swing
    if (cClock < PI/5 - .1) {
        cAngle = -45.f;
        carDir = 1;
        hPosition.x = cPosition.x + 5;// all of these magic numbers are to make the hooman sit in the cah correct
        hPosition.y = cPosition.y - 45;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < PI / 5 + .1) {
        cAngle = 0.f;
        hPosition.x = cPosition.x + 25;
        hPosition.y = cPosition.y - 25;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < 3 * PI / 5 +.2) {
        cAngle = 45.f;
        hPosition.x = cPosition.x +45;
        hPosition.y = cPosition.y -5;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < 4 * PI / 5 +.1) {
        cAngle = 0.f;
        hPosition.x = cPosition.x + 25;
        hPosition.y = cPosition.y - 25;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < PI ) {
        cAngle = -45.f;
        hPosition.x = cPosition.x + 5;
        hPosition.y = cPosition.y - 45;
        hAngle = cAngle + hOffsetA;
        fAngle = fAngle + 15 * cos( 30 * gClock);//15 is amplitude of flag swing, 30 is how fast  
    }   // i gave up on flipping the flag but it shaking like it needs to pee is funny
    else if (cClock < 6 * PI / 5 - .1) {
        fAngle = 0.f; // maintaining the flag start be 0 (it should be but floating points could cause small diff)
        cAngle = 45.f;
        carDir = -1;
        hPosition.x = cPosition.x;
        hPosition.y = cPosition.y -40;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < 6 * PI / 5 + .2) {
        cAngle = 0.f;
        hPosition.x = cPosition.x - 25;
        hPosition.y = cPosition.y - 25;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < 8 * PI / 5 + .2 ) {
        cAngle = -45.f;
        hPosition.x = cPosition.x - 45;
        hPosition.y = cPosition.y - 5;
        hAngle = cAngle + hOffsetA;
    }
    else if (cClock < 9 * PI / 5 + .2) {
        cAngle = 0.f;
        hPosition.x = cPosition.x - 25;
        hPosition.y = cPosition.y - 25;
        hAngle = cAngle + hOffsetA;
    }
    else {
        cAngle = 45.f;
        hPosition.x = cPosition.x;
        hPosition.y = cPosition.y - 40;
        hAngle = cAngle + hOffsetA;
    }
   
    // i had to look up raylib info to make sure i was using
    // color right and the equations were a bit trial and 
    // and error to not have a sharp cut and realized if i
    // evenly spaced the waves then there arent as many 
    // sudden jumps. i also realied abs() will allow it to 
    // smooth the jumps from when cos is negative
    // i cannot figure out blue and purple and that makes me
    // sad as purple is my favorite color. nvm I figured blue
    // out we are so back
    Color newColor{ static_cast<unsigned char>(abs(255 * cos(gClock))),
                    static_cast<unsigned char>(abs(255 * cos(gClock + 2 * PI / 3))),
                    static_cast<unsigned char>(abs(255 * cos(gClock + 4 * PI / 3)))};
    backgroundColor = &newColor;
    

    cClock += .5f * deltaTime;
    if (cClock >= 2 * PI) { cClock = 0; }
    gClock += 1.f * deltaTime;
    if (gClock >= 2 * PI) { gClock = 0; }


}

void render()
{
    
    BeginDrawing();

    //somehow moving this to here made the colors change again
    ClearBackground(*backgroundColor);

    Rectangle textureArea = {
        0.0f,
        0.0f,
        static_cast<float>(rTexture.width ),
        static_cast<float>(rTexture.height)
    };
    

    Rectangle destinationArea = {
        CENTER.x,
        CENTER.y,
        static_cast<float>(rScale.x),
        static_cast<float>(rScale.y)
    };

    Vector2 objectOrigin = {
        static_cast<float>(rScale.x / 2.0f),
        static_cast<float>(rScale.y / 2.0f)
    };

    DrawTexturePro(
        rTexture,
        textureArea,
        destinationArea,
        objectOrigin,
        0.f,
        WHITE
    );
    Rectangle textureArea2 = {
        0.0f,
        0.0f,
        static_cast<float>(hTexture.width),
        static_cast<float>(hTexture.height)
    };

    Rectangle destinationArea2 = {
        hPosition.x,
        hPosition.y,
        static_cast<float>(BASE_SIZE.x),
        static_cast<float>(BASE_SIZE.y)
    };

    Vector2 objectOrigin2 = {
        static_cast<float>(BASE_SIZE.x / 2.0f),
        static_cast<float>(BASE_SIZE.y ) // i want the obj to rotate around bottom
    };

    DrawTexturePro(
        hTexture,
        textureArea2,
        destinationArea2,
        objectOrigin2,
        hAngle,
        WHITE
    );
    Rectangle textureArea3 = {
        0.0f,
        0.0f,
        static_cast<float>(cTexture.width * carDir), // flip the cah when cah is going the other way
        static_cast<float>(cTexture.height)
    };

    Rectangle destinationArea3 = {
        cPosition.x,
        cPosition.y,
        static_cast<float>(cScale.x),
        static_cast<float>(cScale.y)
    };

    Vector2 objectOrigin3 = {
        static_cast<float>(cScale.x / 2.0f),
        static_cast<float>(cScale.y / 2.0f)
    };

    DrawTexturePro(
        cTexture,
        textureArea3,
        destinationArea3,
        objectOrigin3,
        cAngle,
        WHITE
    );
    Rectangle textureArea4 = {
        0.0f,
        0.0f,
        static_cast<float>(fTexture.width), // hopefully flip flag
        static_cast<float>(fTexture.height)
    };

    Rectangle destinationArea4 = {
        FLAGSPOT.x,
        FLAGSPOT.y,
        static_cast<float>(BASE_SIZE.x),
        static_cast<float>(BASE_SIZE.y)
    };

    Vector2 objectOrigin4 = {
        static_cast<float>(BASE_SIZE.x / 2.0f),
        static_cast<float>(BASE_SIZE.y / 2.0f)
    };

    DrawTexturePro(
        fTexture,
        textureArea4,
        destinationArea4,
        objectOrigin4,
        fAngle ,
        WHITE
    );
    /* // im sad this doesnt work
    for (int i = 0; i < 4; i++) {
        Rectangle textureArea = {
        0.0f,
        0.0f,
        static_cast<float>((objLst[i]).width),
        static_cast<float>((objLst[i]).height)
        };

        Rectangle destinationArea = {
            (positions[i]).x,
            (positions[i]).y,
            static_cast<float>((sizes[i]).x),
            static_cast<float>((sizes[i]).y)
        };

        Vector2 objectOrigin = {
            static_cast<float>((sizes[i]).x / 2.0f),
            static_cast<float>((sizes[i]).y / 2.0f)
        };

        DrawTexturePro(
            (objLst[i]),
            textureArea,
            destinationArea,
            objectOrigin,
            (angles[i]),
            WHITE
        );
    }*/
    //printf("%d\n", *backgroundColor);

    //ClearBackground(*backgroundColor);
    //ClearBackground(RAYWHITE);

    EndDrawing();
}

void shutdown() 
{ 
    delete backgroundColor;
    CloseWindow(); // Close window and OpenGL context
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}