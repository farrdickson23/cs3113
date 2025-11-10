

#ifndef STARTSCREEN
#define STARTSCREEN
#include "Scene.h" // use the header guards (i think im using that term right) correctly
#include "Map.h"

class StartScreen : public Scene {
private:

public:

    StartScreen();
    StartScreen(Vector2 origin, const char* bgHexCode);
    ~StartScreen();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif