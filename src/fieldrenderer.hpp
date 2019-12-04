#pragma once

#include <glm/glm.hpp>

#include "arrow.hpp"
#include "camera.hpp"
#include "cuboid.hpp"
#include "field.hpp"
#include "glyph.hpp"
#include "scene.hpp"
#include "shaderprogram.hpp"

enum ColorSchemeType { COLORSCHEME_MUMAX, COLORSCHEME_GRADIENT };
enum GlyphType { ARROW, CUBOID };

class FieldRenderer : public SceneObject {
 public:
  FieldRenderer(Field* field);
  ~FieldRenderer();

  void setField(Field* field);
  void setGradientColorScheme(glm::mat3 gradient);
  void setMumaxColorScheme();
  void setGlyphType(GlyphType);
  void setArrowScalingsFactor(float);
  void setCuboidScalingsFactor(float);
  void initShader();
  void initVertexArray();
  void updateGlyphAttribPointers();
  void updateFieldAttribPointers();
  void updateFieldVBOs();

  void ensureRendering();
  void render();

  glm::mat3 colorGradient() const;
  ColorSchemeType colorSchemeType() const;
  GlyphType glyphType() const;

  Field* field_;
  Arrow arrow;
  Cuboid cuboid;
  ShaderProgram shader;

 private:
  Glyph* glyph;  // points either to arrow or to cuboid
  unsigned int positionVBO_;
  unsigned int vectorsVBO_;
  unsigned int VAO_;
  ColorSchemeType colorSchemeType_;
  GlyphType glyphType_;
  glm::mat3 colorGradient_;
  float arrowScalingsFactor_;
  float cuboidScalingsFactor_;
};