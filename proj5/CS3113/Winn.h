

#ifndef WINN
#define WINN
#include "Scene.h" // use the header guards (i think im using that term right) correctly
#include "Map.h"

class Winn : public Scene {
private:

public:

    Winn();
    Winn(Vector2 origin, const char* bgHexCode);
    ~Winn();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif