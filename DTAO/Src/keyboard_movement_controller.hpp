#pragma once

#include "lve_game_object.hpp"
#include "lve_window.hpp"

namespace lve {
    class KeyboardMovementController {
    public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int mouseLeft = GLFW_MOUSE_BUTTON_LEFT;
        };

        struct ModelVisibleSet {

            int edge_key = GLFW_KEY_E;
            int layout_key = GLFW_KEY_L;
            int resistor_key = GLFW_KEY_R;
            int capacitor_key = GLFW_KEY_C;
            int axis_key = GLFW_KEY_A;
            int vertical_resistor_key = GLFW_KEY_V;
            int horizontal_resistor_key = GLFW_KEY_H;

            bool edge_key_pressed = false;
            bool layout_key_pressed = false;
            bool resistor_key_pressed = false;
            bool capacitor_key_pressed = false;
            bool axis_key_pressed = false;
            bool vertical_resistor_key_pressed = false;
            bool horizontal_resistor_key_pressed = false;            
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);
        
        void visibleSetting(GLFWwindow* window, std::vector<LveGameObject>& gameObjects);
        ModelVisibleSet model_visible_set;

        KeyMappings keys{};
        double mouseX;
        double mouseY;
        double oldXPos;
        double oldYPos;
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.5f };
    };
}  // namespace lve
