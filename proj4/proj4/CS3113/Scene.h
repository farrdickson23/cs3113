#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *ninja;
    Entity* monster1;
    Entity* monster2;

    Map *map;

    SceneType sceneType = LEVEL;

    Music bgm;
    Sound jumpSound;
    Sound ouchSound;
    Sound punchSound;
    Sound loseSound;

    Camera2D camera;

    int nextSceneID = -1;// i saw somewhere this might help

    int lives = 3;
    float endScreenTimer = 5.f;

    //bool contNext = false;
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
    
    GameState& getState() { return mGameState; } // this was suggested
    const GameState& getState() const { return mGameState; } // along with this although i dont know what we have two tbh
    void        setType()                  { mGameState.sceneType = LEVEL; }
    SceneType   getType()            const { return mGameState.sceneType;  }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }


    bool mPlayerWon = false;
    bool mPlayerLost = false;
};

#endif