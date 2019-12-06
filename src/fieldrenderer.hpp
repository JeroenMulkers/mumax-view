#pragma once

#include <glm/glm.hpp>

#include "arrow.hpp"
#include "cuboid.hpp"
#include "glyph.hpp"
#include "scene.hpp"
#include "shaderprogram.hpp"

enum ColorSchemeType { COLORSCHEME_MUMAX, COLORSCHEME_GRADIENT };
enum GlyphType { ARROW, CUBOID };

class Field;

class FieldRenderer : public SceneObject {
 public:
  FieldRenderer();
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

  Arrow arrow;
  Cuboid cuboid;
  ShaderProgram shader;

 private:
  FieldRenderer(const FieldRenderer&);
  FieldRenderer& operator=(const FieldRenderer&);
  Field* field_;
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