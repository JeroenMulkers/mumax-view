#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLES3/gl3.h>

const float PI = glm::pi<float>();

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

struct Triangle {
  Vertex v1;
  Vertex v2;
  Vertex v3;
};

class Arrow {
 public:
  Arrow(float shaftRadius, float headRadius, float headRatio, int nSegments);
  ~Arrow();

  void updateVBOdata();
  void setShaftRadius(float shaftRadius);
  void setHeadRadius(float headRadius);
  void setHeadRatio(float headRatio);
  void setSegments(float nSegments);

  bool hasShaft() const;

  int nTriangles() const;
  int nVertices() const;

  std::vector<Triangle> triangles() const;
  unsigned int VBO() const;

 private:
  float shaftRadius_;
  float headRadius_;
  float headRatio_;
  int nSegments_;
  unsigned int VBO_;
};
