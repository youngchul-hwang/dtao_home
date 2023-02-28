#ifndef DTAORENDERSYSTEM_H
#define DTAORENDERSYSTEM_H

#include <QVulkanWindow>
#include <vulkan/vulkan.h>

#include "lve_window.h"
#include "lve_device.hpp"
#include "lve_renderer.hpp"

#include <iostream>
#include <vector>


class LveWindow;
class LveDevice;
class LveRenderer;
class LveCamera{};
class SimpleRenderSystem;
class LveGameObject{};
class KeyboardMovementController{};

class DtaoRenderSystem : public QVulkanWindowRenderer
{
public:
    DtaoRenderSystem(LveWindow *w);
    ~DtaoRenderSystem();

    DtaoRenderSystem(const DtaoRenderSystem &) = delete;
    DtaoRenderSystem &operator=(const DtaoRenderSystem &) = delete;


    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;

    void startNextFrame() override;

private:
    void loadGameObjects();

    LveWindow* lveWindow;
    LveDevice* lveDevice;
    LveRenderer* lveRenderer;
    //SimpleRenderSystem* simpleRenderSystem;

    //std::vector<LveGameObject> gameObjects;
    //LveCamera camera;
    //KeyboardMovementController cameraController;

private:
    void createLveDevice();
    void deleteLveDevice();
    void createLveRenderer();
    void deleteLveRenderer();

};

#endif // DTAORENDERSYSTEM_H
