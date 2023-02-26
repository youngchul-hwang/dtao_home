#ifndef DTAORENDERSYSTEM_H
#define DTAORENDERSYSTEM_H

#include <iostream>
#include <vector>


class LveWindow;
class LveDevice;
class LveRenderer;
class LveCamera{};
class SimpleRenderSystem;
class LveGameObject{};
class KeyboardMovementController{};

class DtaoRenderSystem
{
public:
    DtaoRenderSystem();
    ~DtaoRenderSystem();

    DtaoRenderSystem(const DtaoRenderSystem &) = delete;
    DtaoRenderSystem &operator=(const DtaoRenderSystem &) = delete;


private:
    void loadGameObjects();

    LveWindow* lveWindow;
    LveDevice* lveDevice;
    LveRenderer* lveRenderer;
    SimpleRenderSystem* simpleRenderSystem;

    std::vector<LveGameObject> gameObjects;
    LveCamera camera;
    KeyboardMovementController cameraController;



};

#endif // DTAORENDERSYSTEM_H
