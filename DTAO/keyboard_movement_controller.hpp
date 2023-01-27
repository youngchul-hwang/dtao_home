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
    int enableEdgeLine = GLFW_KEY_L;
  };

  void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);

  KeyMappings keys{};
  double mouseX;
  double mouseY;
  double oldXPos;
  double oldYPos;
  float moveSpeed{3.f};
  float lookSpeed{1.5f};
};
}  // namespace lve