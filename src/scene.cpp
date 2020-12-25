#include "scene.hpp"

#include <GLES3/gl3.h>

#include <iostream>

SceneObject::SceneObject() {
  scene_ = nullptr;
  visible_ = true;
}

SceneObject::~SceneObject() {
  removeFromScene();
}

void SceneObject::putOnScene(Scene* newScene) {
  removeFromScene();
  scene_ = newScene;
  scene_->objects_.insert(this);
  scene_->ensureRendering();
}

void SceneObject::removeFromScene() {
  if (!scene_)
    return;
  scene_->objects_.erase(this);
  scene_->ensureRendering();
}

void SceneObject::ensureRendering() {
  if (scene_)
    scene_->ensureRendering();
}

Scene::Scene() : camera(this) {
  resetCamera();
  setBackgroundColor(0.3, 0.3, 0.3);
}

Scene::~Scene() {
  while (!objects_.empty()) {
    (*objects_.begin())->removeFromScene();
  }
}

Scene* SceneObject::scene() {
  return scene_;
}

void SceneObject::setVisibility(bool visible) {
  if (visible == visible_)
    return;
  visible_ = visible;
  ensureRendering();
}

bool SceneObject::isVisible() const {
  return visible_;
}

bool Scene::needRendering() const {
  return needRender_;
}

void Scene::setBackgroundColor(float r, float g, float b) {
  glClearColor(r, g, b, 1.0f);
  ensureRendering();
}

void Scene::ensureRendering() {
  needRender_ = true;
}

void Scene::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (auto object : objects_) {
    if (object->isVisible())
      object->render();
  }
  needRender_ = false;
}

void Scene::resetCamera() {
  camera.setYaw(0.0);
  camera.setPitch(3.1415 / 10.);
  camera.setTarget(glm::vec3(0, 0, 0));
}