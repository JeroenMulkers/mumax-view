#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "glyph.hpp"

class Cuboid : public Glyph {
 public:
  Cuboid(glm::vec3 size = glm::vec3{0.8, 0.8, 0.8});

  void setSize(glm::vec3 size);
  std::vector<Triangle> triangles() const;
  int nTriangles() const;

 private:
  glm::vec3 size_;
};
