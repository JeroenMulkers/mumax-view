#pragma once

#include "scene.hpp"
#include "shaderprogram.hpp"

class Field;

class FieldBoxRenderer : public SceneObject {
 public:
  FieldBoxRenderer();
  ~FieldBoxRenderer();
  void initShader();
  void initVertexArray();
  void updateOutlineVBO();
  void updateAttribPointers();
  void setField(Field* field);
  void setColor(float r, float g, float b);
  void render();

 private:
  FieldBoxRenderer(const FieldBoxRenderer&);
  FieldBoxRenderer& operator=(const FieldBoxRenderer&);

 private:
  ShaderProgram shader;
  Field* field_;
  unsigned int outlineVBO_;
  unsigned int VAO_;
  glm::vec3 color_;
};