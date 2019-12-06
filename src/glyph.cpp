#include <vector>

#include <GLES3/gl3.h>
#include <glm/glm.hpp>

#include "glyph.hpp"
#include "scene.hpp"

Glyph::Glyph() : nVertices_(0) {
  parent_ = nullptr;
  glGenBuffers(1, &VBO_);
}

Glyph::~Glyph() {
  glDeleteBuffers(1, &VBO_);
}

int Glyph::nVertices() const {
  return nVertices_;
};

void Glyph::updateVBOdata() {
  std::vector<Triangle> triangles_ = triangles();
  int nTriangles_ = triangles_.size();
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * nTriangles_, &triangles_[0],
               GL_STATIC_DRAW);
  nVertices_ = 3 * nTriangles_;
  parent_->ensureRendering();
}

void Glyph::setParent(SceneObject* parent) {
  parent_ = parent;
  parent_->ensureRendering();
}

unsigned int Glyph::VBO() const {
  return VBO_;
}