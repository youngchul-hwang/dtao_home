#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace lve {

    FirstApp::FirstApp() { loadGameObjects(); }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };
        LveCamera camera{};

        camera.setViewTarget(
            glm::vec3(2.0f,2.0f,2.0f), 
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, 0.0f, 1.0f));
        

        auto viewerObject = LveGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime =
                std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            //camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);

                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);

                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        std::string layout_info_file_path = "layout_input_data.csv";
        std::shared_ptr<LveModel> lveModel = std::make_unique<LveModel>(this->lveDevice, layout_info_file_path);
            
        auto cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = { 0.0f, 0.0f, 0.0f };
        cube.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(cube));
        
        //*
        cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = { 1.0f, 0.0f, 0.0f };
        cube.transform.scale = { 0.5f, 0.5f, 0.5f };
        gameObjects.push_back(std::move(cube));
        //*/
    }

}  // namespace lve

//수정 테스트, 가능한가?
