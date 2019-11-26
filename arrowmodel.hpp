#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "glm/gtc/type_ptr.hpp"

//#include <GL/glew.h>
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
  Arrow(float shaftRadius, float headRadius, float headRatio, int nSegments)
      : shaftRadius_(shaftRadius),
        headRadius_(headRadius),
        headRatio_(headRatio),
        nSegments_(nSegments) {
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    updateVBOdata();
  }

  void updateVBOdata() {
    auto triangles_ = triangles();
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * nTriangles(),
                 &triangles_[0], GL_STATIC_DRAW);
  }

  ~Arrow() { glDeleteBuffers(1, &VBO_); }

  void setShaftRadius(float shaftRadius) {
    shaftRadius_ = shaftRadius;
    updateVBOdata();
  }

  void setHeadRadius(float headRadius) {
    headRadius_ = headRadius;
    updateVBOdata();
  }

  void setHeadRatio(float headRatio) {
    headRatio_ = headRatio;
    updateVBOdata();
  }

  void setSegments(float nSegments) {
    nSegments_ = nSegments;
    updateVBOdata();
  }

  bool hasShaft() const { return headRatio_ < 0.99; }

  int nTriangles() const {
    int nShaftTriangles = 3 * nSegments_;
    int nHeadTriangles = 2 * nSegments_;
    if (hasShaft()) {  // shaft and head
      return nShaftTriangles + nHeadTriangles;
    } else {  // only head
      return nHeadTriangles;
    }
  }

  int nVertices() const {
    return 3 * nTriangles();  // Each triangle consists out of 3 vertices
  }

  std::vector<Triangle> triangles() const {
    std::vector<Triangle> v;
    v.reserve(nTriangles());

    const glm::vec3 e_z{0, 0, 1};

    for (int i = 0; i < nSegments_; i++) {
      float phi1 = (i + 0.5f) * 2.0f * PI / (float)nSegments_;
      float phi2 = (i - 0.5f) * 2.0f * PI / (float)nSegments_;
      glm::vec2 p1 = glm::vec2{glm::cos(phi1), glm::sin(phi1)};
      glm::vec2 p2 = glm::vec2{glm::cos(phi2), glm::sin(phi2)};

      // SHAFT
      if (hasShaft()) {
        glm::vec3 top1{shaftRadius_ * p1, 0.5 - headRatio_};
        glm::vec3 top2{shaftRadius_ * p2, 0.5 - headRatio_};
        glm::vec3 bot1{shaftRadius_ * p1, -0.5};
        glm::vec3 bot2{shaftRadius_ * p2, -0.5};
        glm::vec3 n1 = glm::normalize(glm::vec3{p1, 0});
        glm::vec3 n2 = glm::normalize(glm::vec3{p2, 0});
        // side segments
        v.push_back(
            Triangle{Vertex{top1, n1}, Vertex{top2, n2}, Vertex{bot1, n1}});
        v.push_back(
            Triangle{Vertex{bot1, n1}, Vertex{bot2, n2}, Vertex{top2, n2}});
        // bottom disc segment
        v.push_back(Triangle{Vertex{bot1, -e_z}, Vertex{bot2, -e_z},
                             Vertex{{0, 0, -0.5}, -e_z}});
      }

      // CONE
      glm::vec3 bot1 = glm::vec3{headRadius_ * p1, 0.5f - headRatio_};
      glm::vec3 bot2 = glm::vec3{headRadius_ * p2, 0.5f - headRatio_};
      glm::vec3 t{0, 0, 0.5};
      glm::vec3 n1 = glm::vec3{p1, headRadius_ / headRatio_};
      glm::vec3 n2 = glm::vec3{p2, headRadius_ / headRatio_};
      // side segments
      //    normal on the top is (0,0,0) to get a smooth cone
      //    (https://stackoverflow.com/questions/15283508/low-polygon-cone-smooth-shading-at-the-tip)
      v.push_back(Triangle{Vertex{bot1, n1}, Vertex{bot2, n2},
                           Vertex{{0, 0, 0.5}, {0, 0, 0}}});
      // disc segment
      v.push_back(Triangle{Vertex{bot1, -e_z}, Vertex{bot2, -e_z},
                           Vertex{{0, 0, 0.5 - headRatio_}, -e_z}});
    }
    return v;
  }

  unsigned int VBO() const { return VBO_; }

 private:
  float shaftRadius_;
  float headRadius_;
  float headRatio_;
  int nSegments_;
  unsigned int VBO_;
};
