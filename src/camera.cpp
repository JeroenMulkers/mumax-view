#include "camera.hpp"
#include "scene.hpp"

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(Scene* scene) : scene_(scene) {
  reset();
}

Camera::Camera(const Camera& camera) : scene_(nullptr) {
  targetDistance_ = camera.targetDistance_;
  yaw_ = camera.yaw_;
  pitch_ = camera.pitch_;
  target_ = camera.target_;
  ensureRendering();
}

Camera& Camera::operator=(const Camera& camera) {
  targetDistance_ = camera.targetDistance_;
  yaw_ = camera.yaw_;
  pitch_ = camera.pitch_;
  target_ = camera.target_;
  ensureRendering();
  return *this;
}

void Camera::reset() {
  targetDistance_ = 40.0f;
  yaw_ = (0.0);
  pitch_ = (3.1415 / 10.);
  target_ = glm::vec3(0, 0, 0);
  ensureRendering();
}

void Camera::setTarget(glm::vec3 target) {
  target_ = target;
  ensureRendering();
}

void Camera::setDistance(float distance) {
  targetDistance_ = distance;
  ensureRendering();
}

void Camera::setYaw(float yaw) {
  yaw_ = yaw;
  ensureRendering();
}

void Camera::setPitch(float pitch) {
  pitch_ = pitch;
  ensureRendering();
}

glm::vec3 Camera::target() const {
  return target_;
}

float Camera::distance() const {
  return targetDistance_;
}

float Camera::yaw() const {
  return yaw_;
}

float Camera::pitch() const {
  return pitch_;
}

glm::vec3 Camera::position() const {
  float x = -glm::cos(pitch_) * glm::sin(yaw_);
  float y = -glm::cos(pitch_) * glm::cos(yaw_);
  float z = glm::sin(pitch_);
  return target_ + targetDistance_ * glm::vec3(x, y, z);
}

glm::vec3 Camera::up() const {
  float x = -glm::cos(pitch_ + glm::pi<float>() / 2.0) * glm::sin(yaw_);
  float y = -glm::cos(pitch_ + glm::pi<float>() / 2.0) * glm::cos(yaw_);
  float z = glm::sin(pitch_ + glm::pi<float>() / 2.0);
  return glm::vec3(x, y, z);
}

glm::mat4 Camera::viewMatrix() const {
  return glm::lookAt(position(), target_, up());
}

glm::mat4 Camera::projectionMatrix() const {
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  int width = vp[2] - vp[0];
  int height = vp[3] - vp[1];
  float aspect = static_cast<float>(width) / static_cast<float>(height);
  float nearCut = 0.1;
  float fovy = glm::radians(45.0f);
  return glm::infinitePerspective(fovy, aspect, nearCut);
}

void Camera::ensureRendering() {
  if (scene_)
    scene_->ensureRendering();
}
