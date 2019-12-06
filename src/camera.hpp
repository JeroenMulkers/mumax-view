#pragma once

#include <glm/glm.hpp>

class Scene;

class Camera {
 public:
  Camera(Scene* scene = nullptr);

  /// Uses the state (but not the scene pointer) of a camera to construct a
  /// new camera
  Camera(const Camera& camera);

  /// Copies the state of the camera (but keeps his own pointer to a scene)
  Camera& operator=(const Camera& camera);

  void setTarget(glm::vec3);
  void setDistance(float);
  void setYaw(float);
  void setPitch(float);

  glm::vec3 target() const;
  float distance() const;
  float yaw() const;
  float pitch() const;
  glm::vec3 position() const;
  glm::vec3 up() const;
  glm::mat4 viewMatrix() const;
  glm::mat4 projectionMatrix() const;

 private:
  void ensureRendering();

 private:
  float targetDistance_;
  float yaw_, pitch_;
  glm::vec3 target_;
  Scene* scene_;
};