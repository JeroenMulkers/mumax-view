#pragma once

#include <set>

#include "camera.hpp"

class Scene;

class SceneObject {
 public:
  SceneObject();
  ~SceneObject();
  void putOnScene(Scene* scene);
  void removeFromScene();
  void ensureRendering();
  Scene* scene();
  virtual void render() = 0;

 private:
  Scene* scene_;
};

class Scene {
 public:
  Scene();
  ~Scene();
  bool needRendering() const;
  void ensureRendering();
  void render();
  void resetCamera();
  Camera* camera();

 private:
  Camera camera_;
  std::set<SceneObject*> objects_;
  friend void SceneObject::putOnScene(Scene* scene);
  friend void SceneObject::removeFromScene();
  bool needRender_;
};