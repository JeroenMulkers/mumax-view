#pragma once

#include <glm/glm.hpp>

#include "arrowmodel.hpp"
#include "camera.hpp"
#include "field.hpp"
#include "shaderprogram.hpp"

enum ColorSchemeType { COLORSCHEME_MUMAX, COLORSCHEME_GRADIENT };

class FieldRenderer {
 public:
  FieldRenderer(Field* field);
  ~FieldRenderer();

  void setField(Field* field);
  void setGradientColorScheme(glm::mat3 gradient);
  void setMumaxColorScheme();
  void initShader();
  void initVertexArray();
  void updateFieldAttribPointers();
  void resetCamera();
  void updateFieldVBOs();

  void render();

  glm::mat3 colorGradient() const;
  int nRenderings() const;
  ColorSchemeType colorSchemeType() const;

  Field* field_;
  Arrow arrow;
  ShaderProgram shader;
  Camera camera;
  bool needRender;
  float arrowScalingsFactor;

 private:
  unsigned int positionVBO_;
  unsigned int vectorsVBO_;
  unsigned int VAO_;
  int aInstancePosLoc_;
  int aInstanceVecLoc_;
  ColorSchemeType colorSchemeType_;
  glm::mat3 colorGradient_;
  int nRenderings_;
};