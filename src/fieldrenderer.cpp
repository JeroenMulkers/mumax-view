#include <glm/glm.hpp>

#include "field.hpp"
#include "fieldrenderer.hpp"
#include "shaders.hpp"

FieldRenderer::FieldRenderer()
    : cuboid(glm::vec3{1.0, 1.0, 1.0}),
      arrow(0.12, 0.2, 0.6, 40),
      shader(this) {
  field_ = nullptr;

  glGenBuffers(1, &vectorsVBO_);
  glGenBuffers(1, &positionVBO_);

  initShader();
  initVertexArray();

  setGlyphType(ARROW);
  setCuboidScalingsFactor(1.0);
  setArrowScalingsFactor(1.0);
  setMumaxColorScheme();
  setRelativeRange({0, 0, 0}, {1, 1, 1});

  arrow.setParent(this);
  cuboid.setParent(this);
}

FieldRenderer::~FieldRenderer() {
  glDeleteBuffers(1, &positionVBO_);
  glDeleteBuffers(1, &vectorsVBO_);
  glDeleteBuffers(1, &VAO_);
}

void FieldRenderer::setField(Field* field) {
  field_ = field;
  updateRange();
  updateFieldVBOs();
  updateFieldAttribPointers();
  cuboid.setSize(field->cellsize);
  cuboid.updateVBOdata();
  ensureRendering();
}

void FieldRenderer::setGradientColorScheme(glm::mat3 gradient) {
  shader.setBool("useColorGradient", true);
  shader.setMat3("colorGradient", gradient);
  colorGradient_ = gradient;
  colorSchemeType_ = COLORSCHEME_GRADIENT;
};

void FieldRenderer::setMumaxColorScheme() {
  shader.setBool("useColorGradient", false);
  colorSchemeType_ = COLORSCHEME_MUMAX;
};

void FieldRenderer::setArrowScalingsFactor(float scalingsFactor) {
  arrowScalingsFactor_ = scalingsFactor;
  shader.setFloat("arrowScalingsFactor", scalingsFactor);
}

void FieldRenderer::setCuboidScalingsFactor(float scalingsFactor) {
  cuboidScalingsFactor_ = scalingsFactor;
  shader.setFloat("cuboidScalingsFactor", scalingsFactor);
}

void FieldRenderer::setRelativeRange(glm::vec3 low, glm::vec3 high) {
  relativeRangeLow_ = low;
  relativeRangeHigh_ = high;
  if (field_)
    updateRange();
}

void FieldRenderer::updateRange() {
  glm::vec3 L(field_->cellsize.x * field_->gridsize().x,
              field_->cellsize.y * field_->gridsize().y,
              field_->cellsize.z * field_->gridsize().z);

  glm::vec3 absoluteLow = (relativeRangeLow_ - glm::vec3(0.5, 0.5, 0.5)) * L;
  glm::vec3 absoluteHigh = (relativeRangeHigh_ - glm::vec3(0.5, 0.5, 0.5)) * L;
  shader.setVec3("rangeLow", absoluteLow);
  shader.setVec3("rangeHigh", absoluteHigh);
}

void FieldRenderer::setGlyphType(GlyphType type) {
  if (type == ARROW) {
    glyph = &arrow;
    shader.setBool("arrowGlyph", true);
  } else {
    glyph = &cuboid;
    shader.setBool("arrowGlyph", false);
  }
  glyphType_ = type;
  updateGlyphAttribPointers();
};

ColorSchemeType FieldRenderer::colorSchemeType() const {
  return colorSchemeType_;
};

GlyphType FieldRenderer::glyphType() const {
  return glyphType_;
};

void FieldRenderer::updateFieldVBOs() {
  if (!field_)
    return;
  int bufferSize = sizeof(glm::vec3) * field_->ncells();
  glBindBuffer(GL_ARRAY_BUFFER, vectorsVBO_);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, &field_->data[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, &field_->positions[0],
               GL_STATIC_DRAW);
}

glm::mat3 FieldRenderer::colorGradient() const {
  return colorGradient_;
}

void FieldRenderer::initShader() {
  shader.attachShader(vertexshader, GL_VERTEX_SHADER);
  shader.attachShader(fragmentshader, GL_FRAGMENT_SHADER);
  shader.link();
  // hard coded lightning values
  float ambientLight = 0.1;
  shader.setFloat("ambientLight", ambientLight);
};

void FieldRenderer::initVertexArray() {
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  int aGlyphPosLoc = glGetAttribLocation(shader.ID, "aPos");
  glEnableVertexAttribArray(aGlyphPosLoc);

  int aGlyphNormalLoc = glGetAttribLocation(shader.ID, "aNormal");
  glEnableVertexAttribArray(aGlyphNormalLoc);

  int aInstancePosLoc_ = glGetAttribLocation(shader.ID, "aInstancePos");
  glEnableVertexAttribArray(aInstancePosLoc_);
  glVertexAttribDivisor(aInstancePosLoc_, 1);

  int aInstanceVecLoc_ = glGetAttribLocation(shader.ID, "aInstanceVector");
  glEnableVertexAttribArray(aInstanceVecLoc_);
  glVertexAttribDivisor(aInstanceVecLoc_, 1);
}

void FieldRenderer::updateGlyphAttribPointers() {
  glyph->updateVBOdata();
  glBindVertexArray(VAO_);

  int aGlyphPosLoc = glGetAttribLocation(shader.ID, "aPos");
  glBindBuffer(GL_ARRAY_BUFFER, glyph->VBO());
  glVertexAttribPointer(aGlyphPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)0);

  int aGlyphNormalLoc = glGetAttribLocation(shader.ID, "aNormal");
  glBindBuffer(GL_ARRAY_BUFFER, glyph->VBO());
  glVertexAttribPointer(aGlyphNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)(sizeof(glm::vec3)));
  ensureRendering();
}

void FieldRenderer::updateFieldAttribPointers() {
  updateFieldVBOs();
  glBindVertexArray(VAO_);

  int aInstancePosLoc_ = glGetAttribLocation(shader.ID, "aInstancePos");
  glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
  glVertexAttribPointer(aInstancePosLoc_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(glm::vec3), (void*)0);

  int aInstanceVecLoc_ = glGetAttribLocation(shader.ID, "aInstanceVector");
  glBindBuffer(GL_ARRAY_BUFFER, vectorsVBO_);
  glVertexAttribPointer(aInstanceVecLoc_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(glm::vec3), (void*)0);
  ensureRendering();
};

void FieldRenderer::render() {
  if (!field_)
    return;
  shader.setMat4("projection", scene()->camera.projectionMatrix());
  shader.setMat4("view", scene()->camera.viewMatrix());
  shader.setVec3("viewPos", scene()->camera.position());
  shader.use();
  glBindVertexArray(VAO_);
  glDrawArraysInstanced(GL_TRIANGLES, 0, glyph->nVertices(), field_->ncells());
}