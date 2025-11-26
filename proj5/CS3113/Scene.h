#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *xochitl = nullptr;
    Entity* blaster = nullptr;
    Entity* bullets[5]{};
    Entity* chest1 = nullptr;
    Entity* enemies[5]{};
    Entity* bird = nullptr;

    Vector2 mousePos{};

    Map *map = nullptr;

    Music bgm{};
    Sound jumpSound{};

    Camera2D camera{};
    float lives = 3;

    int nextSceneID =0;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif