#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

struct Triangle {
  Vertex v1;
  Vertex v2;
  Vertex v3;
};

class Glyph {
 public:
  Glyph();
  ~Glyph();

  virtual std::vector<Triangle> triangles() const = 0;
  virtual int nTriangles() const = 0;

  int nVertices() const;
  void updateVBOdata();
  unsigned int VBO() const;

 private:
  unsigned int VBO_;
  int nVertices_;
};