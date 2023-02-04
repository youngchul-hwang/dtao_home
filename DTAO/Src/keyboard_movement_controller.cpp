#include "keyboard_movement_controller.hpp"

// std
#include <limits>
#include <iostream>

#include "lve_model.hpp"

namespace lve {
    void KeyboardMovementController::visibleSetting(GLFWwindow* window, std::vector<LveGameObject>& gameObjects) {
        MODEL_TYPE target_model_type = MODEL_TYPE::MODEL_TYPE_DEFAULT;
        if (glfwGetKey(window, model_visible_set.layout_key) == GLFW_PRESS) model_visible_set.layout_key_pressed = true;
        else if (glfwGetKey(window, model_visible_set.resistor_key) == GLFW_PRESS) model_visible_set.resistor_key_pressed = true;
        else if (glfwGetKey(window, model_visible_set.capacitor_key) == GLFW_PRESS) model_visible_set.capacitor_key_pressed = true;
        else if (glfwGetKey(window, model_visible_set.axis_key) == GLFW_PRESS) model_visible_set.axis_key_pressed = true;

        if (glfwGetKey(window, model_visible_set.layout_key) == GLFW_RELEASE && model_visible_set.layout_key_pressed) {
            model_visible_set.layout_key_pressed = false;
            target_model_type = MODEL_TYPE::MODEL_TYPE_LAYOUT;
        }
        else if (glfwGetKey(window, model_visible_set.resistor_key) == GLFW_RELEASE && model_visible_set.resistor_key_pressed) {
            model_visible_set.resistor_key_pressed = false;
            target_model_type = MODEL_TYPE::MODEL_TYPE_PEX_RESISTOR;
        }
        else if (glfwGetKey(window, model_visible_set.capacitor_key) == GLFW_RELEASE && model_visible_set.capacitor_key_pressed){
            model_visible_set.capacitor_key_pressed = false;
            target_model_type = MODEL_TYPE::MODEL_TYPE_PEX_CAPACITOR;
        }
        else if (glfwGetKey(window, model_visible_set.axis_key) == GLFW_RELEASE && model_visible_set.axis_key_pressed) {
            model_visible_set.axis_key_pressed = false;
            target_model_type = MODEL_TYPE::MODEL_TYPE_AXIS;
        }        
        
        if (target_model_type != MODEL_TYPE::MODEL_TYPE_DEFAULT) {
            for (auto& obj : gameObjects) {
                if (obj.model->getModelType() == target_model_type) obj.model->toggleVisible();
            }
        }        
        
    }

    void KeyboardMovementController::moveInPlaneXZ(
        GLFWwindow* window, float dt, LveGameObject& gameObject) {
        glm::vec3 rotate{ 0 };

        //std::cout << "moveInPlaneXZ\n";
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;
        if (glfwGetMouseButton(window, keys.mouseLeft) == GLFW_PRESS) {
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

            int width, height;
            glfwGetWindowSize(window, &width, &height);


            oldXPos = mouseX, oldYPos = mouseY;

            glfwGetCursorPos(window, &mouseX, &mouseY);

            float xDelta, yDelta;
            xDelta = (mouseX - oldXPos) / width;
            yDelta = (mouseY - oldYPos) / height;

            rotate.y -= xDelta * 1000;
            rotate.x += yDelta * 1000 * height / width;

            // std::cout <<"old" << oldXPos << " " << oldYPos << "\n";
            // std::cout << mouseX << " " << mouseY << "\n";
        }
        if (glfwGetMouseButton(window, keys.mouseLeft) == GLFW_RELEASE) {
            glfwGetCursorPos(window, &mouseX, &mouseY);
            oldXPos = mouseX, oldYPos = mouseY;
        };

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {

            gameObject.transform.rotation += lookSpeed * dt * rotate;
        }

        // limit pitch values between about +/- 85ish degrees
        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
        const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
        const glm::vec3 upDir{ 0.f, -1.f, 0.f };

        glm::vec3 moveDir{ 0.f };
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
    }
}  // namespace lve
