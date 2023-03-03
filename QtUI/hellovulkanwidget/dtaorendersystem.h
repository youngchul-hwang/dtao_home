#ifndef DTAORENDERSYSTEM_H
#define DTAORENDERSYSTEM_H

#include <QVulkanWindow>
#include <QVulkanDeviceFunctions>
#include <vulkan/vulkan.h>

#include "lve_window.h"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "simple_render_system.hpp"

#include <iostream>
#include <vector>




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
    QVulkanDeviceFunctions *m_devFuncs = nullptr;

    void loadGameObjects();

    LveWindow* lveWindow = nullptr;
    LveDevice* lveDevice = nullptr;
    SimpleRenderSystem* simpleRenderSystem = nullptr;

    std::vector<LveGameObject> gameObjects{};
    LveCamera camera{};

    bool render_object_created = {false};

    //KeyboardMovementController cameraController;

private:
    void createLveDevice();
    void deleteLveDevice();
    void createSimpleRenderSystem();
    void deleteSimpleRenderSystem();
    void beginRenderPass(VkCommandBuffer command_buffer);

};

#endif // DTAORENDERSYSTEM_H
