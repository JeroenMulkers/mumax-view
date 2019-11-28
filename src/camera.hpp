#pragma once

#include <glm/glm.hpp>

#define PI 3.2415

class Camera {
 public:
  Camera() {
    // up = glm::vec3(0.0f, 0.0f, 1.0f);
    targetDistance = 40.0f;
  }

  glm::vec3 position() {
    float x = -glm::cos(pitch) * glm::sin(yaw);
    float y = -glm::cos(pitch) * glm::cos(yaw);
    float z = glm::sin(pitch);
    return target + targetDistance * glm::vec3(x, y, z);
  }

  glm::vec3 up() {
    float x = -glm::cos(pitch + glm::pi<float>() / 2.0) * glm::sin(yaw);
    float y = -glm::cos(pitch + glm::pi<float>() / 2.0) * glm::cos(yaw);
    float z = glm::sin(pitch + glm::pi<float>() / 2.0);
    return glm::vec3(x, y, z);
  }

  glm::mat4 viewMatrix() { return glm::lookAt(position(), target, up()); }

  float targetDistance;
  float yaw, pitch;
  glm::vec3 target;

  // glm::vec3 position;
};