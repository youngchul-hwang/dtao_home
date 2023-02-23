#ifndef DTAORENDERSYSTEM_H
#define DTAORENDERSYSTEM_H

#include <iostream>
#include <vector>

namespace lve{

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

    //LveWindow lveWindow{WIDTH, HEIGHT, "Vulkan Tutorial"};
    //LveDevice lveDevice{lveWindow};
    //LveRenderer lveRenderer{lveWindow, lveDevice};
    std::vector<LveGameObject> gameObjects;
    //SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), this->gameObjects };
    LveCamera camera;
    KeyboardMovementController cameraController;



};
}//namespace lve
#endif // DTAORENDERSYSTEM_H
