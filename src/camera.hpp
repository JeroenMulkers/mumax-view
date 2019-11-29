#pragma once

#include <GLES3/gl3.h>
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

  glm::mat4 projectionMatrix() {
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int width = vp[2] - vp[0];
    int height = vp[3] - vp[1];
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    float nearCut = 0.1;
    float fovy = glm::radians(45.0f);
    return glm::infinitePerspective(fovy, aspect, nearCut);
  }

  float targetDistance;
  float yaw, pitch;
  glm::vec3 target;
};