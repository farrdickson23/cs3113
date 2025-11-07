#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *ninja;
    Map *map;

    SceneType sceneType = LEVEL;

    Music bgm;
    Sound jumpSound;

    Camera2D camera;

    int nextSceneID = -1;// i saw somewhere this might help
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
    void        setType()                  { mGameState.sceneType = LEVEL; }
    SceneType   getType()            const { return mGameState.sceneType;  }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif