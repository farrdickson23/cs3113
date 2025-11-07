#include "LevelB.h"

#ifndef STARTSCREEN
#define STARTSCREEN

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