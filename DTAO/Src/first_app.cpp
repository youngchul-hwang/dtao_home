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

#include "PEXResistorModel.h"
#include "PEXCapacitorModel.h"
#include "LayoutModel.h"

namespace lve {

    FirstApp::FirstApp() { loadGameObjects(); }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };
        LveCamera camera{};

        camera.setViewTarget(
            glm::vec3(1.0f,1.0f,1.0f), 
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

            //cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            cameraController.moveCamera(lveWindow.getGLFWwindow(), frameTime, camera);
            //camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            cameraController.visibleSetting(lveWindow.getGLFWwindow(), this->gameObjects);

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

        //PEX model
        
        std::string pex_info_file_path = "Data/MVP_PEX_RC_r1_0130.csv";

        //pex_info_file_path = "Data/pex_res_test_simple.csv";
        std::shared_ptr<PEXResistorModel> pex_model
            = std::make_unique<PEXResistorModel>(this->lveDevice, MODEL_TYPE::MODEL_TYPE_PEX_RESISTOR, pex_info_file_path);
        auto cube = LveGameObject::createGameObject();
        cube.model = pex_model;
        cube.transform.translation = { 0.f, 0.f, 0.0f };
        cube.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(cube));

        //Layout model
        //*
        std::string layout_info_file_path = "Data/layout_input_data.csv";
        std::shared_ptr<LayoutModel> layout_model
            = std::make_unique<LayoutModel>(this->lveDevice, MODEL_TYPE::MODEL_TYPE_LAYOUT, layout_info_file_path);    
        cube = LveGameObject::createGameObject();
        cube.model = layout_model;
        cube.transform.translation = { 0.0f, 0.0f, 0.0f };
        cube.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(cube));
        //*/
        
        //cube = LveGameObject::createGameObject();
        //cube.model = layout_model;
        //cube.transform.translation = { -0.5f, 0.0f, 0.0f };
        //cube.transform.scale = { 1.0f, 1.0f, 1.0f };
        //gameObjects.push_back(std::move(cube));
        

        //Axis model
        std::shared_ptr<LveModel> axis_model
            = std::make_unique<LveModel>(this->lveDevice, MODEL_TYPE::MODEL_TYPE_AXIS);
        cube = LveGameObject::createGameObject();
        cube.model = axis_model;
        cube.transform.translation = { 0.0f, 0.0f, 0.0f };
        cube.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(cube));
        //*/
    }

}  // namespace lve

//수정 테스트, 가능한가?
