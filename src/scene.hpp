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
  SceneObject(const SceneObject&);
  SceneObject& operator=(const SceneObject&);
  Scene* scene_;
};

class Scene {
 public:
  Scene();
  ~Scene();
  bool needRendering() const;
  void setBackgroundColor(float r, float g, float b);
  void ensureRendering();
  void render();
  void resetCamera();
  Camera camera;

 private:
  Scene(const Scene&);
  Scene& operator=(const Scene&);
  std::set<SceneObject*> objects_;
  friend void SceneObject::putOnScene(Scene* scene);
  friend void SceneObject::removeFromScene();
  bool needRender_;
};