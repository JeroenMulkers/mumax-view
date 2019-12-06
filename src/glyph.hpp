#pragma once

#include <glm/glm.hpp>
#include <vector>

class SceneObject;

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
  unsigned int VBO() const;
  void updateVBOdata();

  /// Set the parent SceneObject of a glyph
  /// When the glyph is change, parent->ensureRendering() is called
  void setParent(SceneObject* parent);

 private:
  Glyph(const Glyph&);
  Glyph& operator=(const Glyph&);
  unsigned int VBO_;
  int nVertices_;
  SceneObject* parent_;
};