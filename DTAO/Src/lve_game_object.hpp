#pragma once

#include "lve_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp> //vec3, vec4, ivec4, mat4
#include <glm/gtc/type_ptr.hpp> //value_ptr

// std
#include <memory>

namespace lve {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4() {
            glm::mat4 glm_mat4 = glm::mat4{ 1 };
            //glm_mat4 = glm::rotate(glm_mat4,  rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            //glm_mat4 = glm::rotate(glm_mat4, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            //glm_mat4 = glm::rotate(glm_mat4, rotation.z, glm::vec3(1.0f, 0.0f, 1.0f));
            glm_mat4 = glm::scale(glm_mat4, scale);
            glm_mat4 = glm::translate(glm_mat4, translation);
            

            return glm_mat4;
        }
    };

    class LveGameObject {
    public:
        using id_t = unsigned int;

        static LveGameObject createGameObject() {
            static id_t currentId = 0;
            std::cout << "\n\n***LveGameObject :: id : " << currentId << "\n\n";
            return LveGameObject{ currentId++ };
        }

        LveGameObject(const LveGameObject&) = delete;
        LveGameObject& operator=(const LveGameObject&) = delete;
        LveGameObject(LveGameObject&&) = default;
        LveGameObject& operator=(LveGameObject&&) = default;

        id_t getId() { return id; }

        void setEdgeLineEnable(bool flag) { this->edge_line_enabled = flag; }
        void setAxisLineEnable(bool flag) { this->axis_line_enabled = flag; }

    public:
        std::shared_ptr<LveModel> model{};
        //LveModel* model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        LveGameObject(id_t objId) : id{ objId } {}

        id_t id;

    private:
        bool edge_line_enabled = { true };
        bool axis_line_enabled = { true };
    };
}  // namespace lve
