#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "arrow.hpp"
#include "camera.hpp"
#include "field.hpp"
#include "fieldrenderer.hpp"
#include "shaderprogram.hpp"
#include "shaders.hpp"

FieldRenderer::FieldRenderer(Field* field)
    : cuboid(glm::vec3{1.0, 1.0, 1.0}),
      arrow(0.12, 0.2, 0.6, 40),
      nRenderings_(0),
      needRender(true) {
  glGenBuffers(1, &vectorsVBO_);
  glGenBuffers(1, &positionVBO_);

  initShader();
  initVertexArray();
  shader.use();  // TODO: check why this is needed here

  setGlyphType(ARROW);
  setCuboidScalingsFactor(1.0);
  setArrowScalingsFactor(1.0);
  setMumaxColorScheme();

  setField(field);

  resetCamera();
}

FieldRenderer::~FieldRenderer() {
  glDeleteBuffers(1, &positionVBO_);
  glDeleteBuffers(1, &vectorsVBO_);
  glDeleteBuffers(1, &VAO_);
}

void FieldRenderer::setField(Field* field) {
  field_ = field;
  resetCamera();
  updateFieldVBOs();
  updateFieldAttribPointers();
  needRender = true;
}

void FieldRenderer::setGradientColorScheme(glm::mat3 gradient) {
  shader.setBool("useColorGradient", true);
  shader.setMat3("colorGradient", gradient);
  colorGradient_ = gradient;
  colorSchemeType_ = COLORSCHEME_GRADIENT;
  needRender = true;
};

void FieldRenderer::setMumaxColorScheme() {
  shader.setBool("useColorGradient", false);
  colorSchemeType_ = COLORSCHEME_MUMAX;
  needRender = true;
};

void FieldRenderer::setArrowScalingsFactor(float scalingsFactor) {
  arrowScalingsFactor_ = scalingsFactor;
  shader.setFloat("arrowScalingsFactor", scalingsFactor);
  needRender = true;
}

void FieldRenderer::setCuboidScalingsFactor(float scalingsFactor) {
  cuboidScalingsFactor_ = scalingsFactor;
  shader.setFloat("cuboidScalingsFactor", scalingsFactor);
  needRender = true;
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
  needRender = true;
};

ColorSchemeType FieldRenderer::colorSchemeType() const {
  return colorSchemeType_;
};

GlyphType FieldRenderer::glyphType() const {
  return glyphType_;
};

void FieldRenderer::updateFieldVBOs() {
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
  needRender = true;
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
  needRender = true;
};

void FieldRenderer::render() {
  shader.setMat4("projection", camera.projectionMatrix());
  shader.setMat4("view", camera.viewMatrix());
  shader.setVec3("viewPos", camera.position());
  shader.use();
  glDrawArraysInstanced(GL_TRIANGLES, 0, glyph->nVertices(),
                        field_->ncells());  // needRender = false;
  needRender = false;
  nRenderings_++;
}

void FieldRenderer::resetCamera() {
  needRender = true;
  camera.yaw = 0.0;
  camera.pitch = 3.1415 / 10.;
  if (field_) {
    camera.target = glm::vec3((field_->gridsize().x - 1) / 2.0,
                              (field_->gridsize().y - 1) / 2.0, 0.0f);
  }
};

int FieldRenderer::nRenderings() const {
  return nRenderings_;
}
