#include "Scene.h"

Scene::Scene() : mOrigin{ {} }, mGameState{} {} // since we made the default -1

Scene::Scene(Vector2 origin, const char* bgHexCode) : mOrigin{ origin }, mBGColourHexCode{ bgHexCode }, mGameState{} 
{
    ClearBackground(ColorFromHex(bgHexCode));
} 