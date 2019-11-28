#pragma once

#include <glm/glm.hpp>

#include "arrowmodel.hpp"
#include "camera.hpp"
#include "field.hpp"
#include "shaderprogram.hpp"
#include "shaders.hpp"

enum ColorSchemeType { COLORSCHEME_MUMAX, COLORSCHEME_GRADIENT };

class FieldRenderer {
 public:
  FieldRenderer(Field* field)
      : arrow(0.12, 0.2, 0.6, 40),
        arrowScalingsFactor(1.0),
        nRenderings_(0),
        needRender(true) {
    initShader();
    initVertexArray();
    setField(field);
    resetCamera();
    updateProjectionMatrix();
    shader.use();  // TODO: check why this is needed here
    setMumaxColorScheme();
  }

  ~FieldRenderer() { glDeleteBuffers(1, &VAO); }

  void setField(Field* field) {
    field_ = field;
    resetCamera();
    updateFieldAttribPointers();
    needRender = true;
  }

  void setGradientColorScheme(glm::mat3 gradient) {
    shader.setBool("useColorGradient", true);
    shader.setMat3("colorGradient", gradient);
    colorGradient_ = gradient;
    colorSchemeType_ = COLORSCHEME_GRADIENT;
    needRender = true;
  };

  void setMumaxColorScheme() {
    shader.setBool("useColorGradient", false);
    colorSchemeType_ = COLORSCHEME_MUMAX;
    needRender = true;
  };

  ColorSchemeType colorSchemeType() { return colorSchemeType_; };

  glm::mat3 colorGradient() { return colorGradient_; }

  void initShader() {
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

  void initVertexArray() {
    aInstancePosLoc = glGetAttribLocation(shader.ID, "aInstancePos");
    aInstanceVecLoc = glGetAttribLocation(shader.ID, "aInstanceVector");
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(aInstancePosLoc);
    glVertexAttribDivisor(aInstancePosLoc, 1);

    glEnableVertexAttribArray(aInstanceVecLoc);
    glVertexAttribDivisor(aInstanceVecLoc, 1);

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

  void updateFieldAttribPointers() {
    field_->updateVBOs();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, field_->positionVBO());
    glVertexAttribPointer(aInstancePosLoc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, field_->vectorsVBO());
    glVertexAttribPointer(aInstanceVecLoc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), (void*)0);
    needRender = true;
  };

  void render() {
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

  void resetCamera() {
    needRender = true;
    camera.yaw = 0.0;
    camera.pitch = 3.1415 / 10.;
    if (field_) {
      camera.target = glm::vec3((field_->gridsize().x - 1) / 2.0,
                                (field_->gridsize().y - 1) / 2.0, 0.0f);
    }
  };

  void updateProjectionMatrix() {
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

  void setAspectRatio(float aspect) {
    float nearCut = 0.1;
    float fovy = glm::radians(45.0f);
    glm::mat4 projection = glm::infinitePerspective(fovy, aspect, nearCut);
    shader.setMat4("projection", projection);
    needRender = true;
  }

  int nRenderings() { return nRenderings_; }

  Arrow arrow;
  ShaderProgram shader;
  Camera camera;
  unsigned int VAO;
  int aInstancePosLoc;
  int aInstanceVecLoc;
  bool needRender;
  float arrowScalingsFactor;
  Field* field_;
  int nRenderings_;

 private:
  ColorSchemeType colorSchemeType_;
  glm::mat3 colorGradient_;
};