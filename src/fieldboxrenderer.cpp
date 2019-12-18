#include <string>

#include "field.hpp"
#include "fieldboxrenderer.hpp"
#include "scene.hpp"
#include "shaderprogram.hpp"

static const std::string vertexshader = R"(#version 100

precision highp float;

attribute vec3 aCorners;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 model = vec4(aCorners, 1.0);
 	  gl_Position =  projection * view * model;
}
)";

const std::string fragmentshader = R"(#version 100

precision highp float;

uniform vec3 color;

void main()
{
    gl_FragColor = vec4(color, 1.0);
}
)";

FieldBoxRenderer::FieldBoxRenderer() {
  glGenBuffers(1, &outlineVBO_);

  initShader();
  initVertexArray();

  setColor(1., 1., 1.);

  field_ = nullptr;
}

FieldBoxRenderer::~FieldBoxRenderer() {
  glDeleteBuffers(1, &outlineVBO_);
  glDeleteBuffers(1, &VAO_);
}

void FieldBoxRenderer::initShader() {
  shader.attachShader(vertexshader, GL_VERTEX_SHADER);
  shader.attachShader(fragmentshader, GL_FRAGMENT_SHADER);
  shader.link();
  shader.setVec3("color", color_);
}

void FieldBoxRenderer::initVertexArray() {
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);
  int aCorners = glGetAttribLocation(shader.ID, "aCorners");
  glEnableVertexAttribArray(aCorners);
}

void FieldBoxRenderer::updateOutlineVBO() {
  if (!field_)
    return;
  glBindBuffer(GL_ARRAY_BUFFER, outlineVBO_);
  glm::vec3 cs = field_->cellsize;
  glm::ivec3 gs = field_->gridsize();
  glm::vec3 L = {cs.x * gs.x, cs.y * gs.y, cs.z * gs.z};
  std::vector<glm::vec3> data;
  data.reserve(24);

  // upper
  data.push_back({0.5 * L.x, 0.5 * L.y, -0.5 * L.z});
  data.push_back({-0.5 * L.x, 0.5 * L.y, -0.5 * L.z});
  data.push_back({-0.5 * L.x, 0.5 * L.y, -0.5 * L.z});
  data.push_back({-0.5 * L.x, -0.5 * L.y, -0.5 * L.z});
  data.push_back({-0.5 * L.x, -0.5 * L.y, -0.5 * L.z});
  data.push_back({0.5 * L.x, -0.5 * L.y, -0.5 * L.z});
  data.push_back({0.5 * L.x, -0.5 * L.y, -0.5 * L.z});
  data.push_back({0.5 * L.x, 0.5 * L.y, -0.5 * L.z});

  // lower
  data.push_back({0.5 * L.x, 0.5 * L.y, 0.5 * L.z});
  data.push_back({-0.5 * L.x, 0.5 * L.y, 0.5 * L.z});
  data.push_back({-0.5 * L.x, 0.5 * L.y, 0.5 * L.z});
  data.push_back({-0.5 * L.x, -0.5 * L.y, 0.5 * L.z});
  data.push_back({-0.5 * L.x, -0.5 * L.y, 0.5 * L.z});
  data.push_back({0.5 * L.x, -0.5 * L.y, 0.5 * L.z});
  data.push_back({0.5 * L.x, -0.5 * L.y, 0.5 * L.z});
  data.push_back({0.5 * L.x, 0.5 * L.y, 0.5 * L.z});

  // sides
  data.push_back({0.5 * L.x, 0.5 * L.y, 0.5 * L.z});
  data.push_back({0.5 * L.x, 0.5 * L.y, -0.5 * L.z});
  data.push_back({-0.5 * L.x, 0.5 * L.y, 0.5 * L.z});
  data.push_back({-0.5 * L.x, 0.5 * L.y, -0.5 * L.z});
  data.push_back({-0.5 * L.x, -0.5 * L.y, 0.5 * L.z});
  data.push_back({-0.5 * L.x, -0.5 * L.y, -0.5 * L.z});
  data.push_back({0.5 * L.x, -0.5 * L.y, 0.5 * L.z});
  data.push_back({0.5 * L.x, -0.5 * L.y, -0.5 * L.z});

  int bufferSize = sizeof(glm::vec3) * data.size();
  glBufferData(GL_ARRAY_BUFFER, bufferSize, &data[0], GL_STATIC_DRAW);
}

void FieldBoxRenderer::updateAttribPointers() {
  updateOutlineVBO();
  glBindVertexArray(VAO_);

  int aCornersPos = glGetAttribLocation(shader.ID, "aCorners");
  glBindBuffer(GL_ARRAY_BUFFER, outlineVBO_);
  glVertexAttribPointer(aCornersPos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (void*)0);

  ensureRendering();
}

void FieldBoxRenderer::setField(Field* field) {
  field_ = field;
  updateOutlineVBO();
  updateAttribPointers();
  ensureRendering();
};

void FieldBoxRenderer::setColor(float r, float g, float b) {
  color_ = {r, g, b};
  shader.setVec3("color", color_);
  ensureRendering();
}

void FieldBoxRenderer::render() {
  if (!field_)
    return;
  shader.setMat4("projection", scene()->camera.projectionMatrix());
  shader.setMat4("view", scene()->camera.viewMatrix());
  shader.use();
  glBindVertexArray(VAO_);
  glLineWidth(1.0);
  glDrawArrays(GL_LINES, 0, 24);
}