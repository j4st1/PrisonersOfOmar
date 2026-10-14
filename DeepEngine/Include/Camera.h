#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

class Camera {
public:
    glm::vec3 position{0.0f, 1.65f, 0.0f};
    float yaw   = -90.0f;   // degrees, 0 = +X
    float pitch = 0.0f;     // degrees, clamped internally
    float fov   = 70.0f;

    glm::vec3 front() const
    {
        float cy = std::cos(glm::radians(yaw));
        float sy = std::sin(glm::radians(yaw));
        float cp = std::cos(glm::radians(pitch));
        float sp = std::sin(glm::radians(pitch));
        return glm::normalize(glm::vec3(cy * cp, sp, sy * cp));
    }

    glm::vec3 right() const
    {
        return glm::normalize(glm::cross(front(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    glm::mat4 view() const
    {
        return glm::lookAt(position, position + front(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 projection(float aspect) const
    {
        return glm::perspective(glm::radians(fov), aspect, 0.05f, 250.0f);
    }
};
