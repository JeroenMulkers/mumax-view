#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "arrowmodel.hpp"
#include "camera.hpp"
#include "field.hpp"
#include "fieldrenderer.hpp"
#include "shaderprogram.hpp"
#include "shaders.hpp"

FieldRenderer::FieldRenderer(Field* field)
    : arrow(0.12, 0.2, 0.6, 40),
      arrowScalingsFactor(1.0),
      nRenderings_(0),
      needRender(true) {
  glGenBuffers(1, &vectorsVBO_);
  glGenBuffers(1, &positionVBO_);
  initShader();
  initVertexArray();
  setField(field);
  resetCamera();
  updateProjectionMatrix();
  shader.use();  // TODO: check why this is needed here
  setMumaxColorScheme();
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

ColorSchemeType FieldRenderer::colorSchemeType() const {
  return colorSchemeType_;
};

void FieldRenderer::updateFieldVBOs() {
  int bufferSize = sizeof(glm::vec3) * field_->ncells();
  glBindBuffer(GL_ARRAY_BUFFER, vectorsVBO_);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, &field_->data[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, &field_->positions[0],
               GL_STATIC_DRAW);
}

unsigned int FieldRenderer::positionVBO() const {
  return positionVBO_;
}

unsigned int FieldRenderer::vectorsVBO() const {
  return vectorsVBO_;
}

glm::mat3 FieldRenderer::colorGradient() const {
  return colorGradient_;
}

void FieldRenderer::initShader() {
  shader.attachShader(vertexshader, GL_VERTEX_SHADER);
  shader.attachShader(fragmentshader, GL_FRAGMENT_SHADER);
  shader.link();

  // hard coded lightning values
  glm::vec3 lightDirection = glm::vec3(1.0f, 0.5f, -1.0f);
  float ambientLightStrength = 0.1;
  float specularLightStrength = 0.5;

  shader.setVec3("lightDir", lightDirection);
  shader.setFloat("ambient", ambientLightStrength);
  shader.setFloat("specularStrength", specularLightStrength);
};

void FieldRenderer::initVertexArray() {
  aInstancePosLoc_ = glGetAttribLocation(shader.ID, "aInstancePos");
  aInstanceVecLoc_ = glGetAttribLocation(shader.ID, "aInstanceVector");
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  glEnableVertexAttribArray(aInstancePosLoc_);
  glVertexAttribDivisor(aInstancePosLoc_, 1);

  glEnableVertexAttribArray(aInstanceVecLoc_);
  glVertexAttribDivisor(aInstanceVecLoc_, 1);

  // Arrow model triangles
  int aPosLoc = glGetAttribLocation(shader.ID, "aPos");
  glEnableVertexAttribArray(aPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, arrow.VBO());
  glVertexAttribPointer(aPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)0);

  // Arrow model normals
  int aNormalLoc = glGetAttribLocation(shader.ID, "aNormal");
  glEnableVertexAttribArray(aNormalLoc);
  glBindBuffer(GL_ARRAY_BUFFER, arrow.VBO());
  glVertexAttribPointer(aNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)(sizeof(glm::vec3)));
}

void FieldRenderer::updateFieldAttribPointers() {
  updateFieldVBOs();
  glBindVertexArray(VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
  glVertexAttribPointer(aInstancePosLoc_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(glm::vec3), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, vectorsVBO_);
  glVertexAttribPointer(aInstanceVecLoc_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(glm::vec3), (void*)0);
  needRender = true;
};

void FieldRenderer::render() {
  updateProjectionMatrix();
  shader.setFloat("ArrowScalingsFactor", arrowScalingsFactor);
  shader.setMat4("view", camera.viewMatrix());
  shader.setVec3("viewPos", camera.position());
  shader.use();
  glDrawArraysInstanced(GL_TRIANGLES, 0, arrow.nVertices(),
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

void FieldRenderer::updateProjectionMatrix() {
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  int width = vp[2] - vp[0];
  int height = vp[3] - vp[1];
  float aspect = static_cast<float>(width) / static_cast<float>(height);
  float nearCut = 0.1;
  float fovy = glm::radians(45.0f);
  glm::mat4 projection = glm::infinitePerspective(fovy, aspect, nearCut);
  shader.setMat4("projection", projection);
  needRender = true;
}

void FieldRenderer::setAspectRatio(float aspect) {
  float nearCut = 0.1;
  float fovy = glm::radians(45.0f);
  glm::mat4 projection = glm::infinitePerspective(fovy, aspect, nearCut);
  shader.setMat4("projection", projection);
  needRender = true;
}

int FieldRenderer::nRenderings() const {
  return nRenderings_;
}
