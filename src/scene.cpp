#include "scene.hpp"

#include <GLES3/gl3.h>
#include <libpng/png.h>

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

void Scene::screenshot(std::string filename) {
  render();

  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  int width = viewport[2];
  int height = viewport[3];
  int nComp = 4;  // RGBA
  int pixelDataSize = width * height * nComp;

  uint8_t* pixelData = new uint8_t[pixelDataSize];

  glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGBA,
               GL_UNSIGNED_BYTE, pixelData);

  png_bytepp rows = new png_bytep[height];
  for (int i = 0; i < height; i++) {
    rows[i] = &pixelData[4 * width * (height - i - 1)];
  }

  // TODO: check errors, add meta data

  FILE* fp = fopen(&filename[0], "wb");

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, width, height, 8 /* bit_depth */,
               PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, rows);
  png_write_end(png_ptr, NULL);

  fclose(fp);

  delete[] pixelData;
  delete[] rows;
}

void Scene::resetCamera() {
  camera.setYaw(0.0);
  camera.setPitch(3.1415 / 10.);
  camera.setTarget(glm::vec3(0, 0, 0));
}