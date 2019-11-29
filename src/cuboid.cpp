#include <vector>

#include <glm/glm.hpp>

#include "cuboid.hpp"

Cuboid::Cuboid(glm::vec3 size) : size_(size) {
  updateVBOdata();
};

void Cuboid::setSize(glm::vec3 size) {
  size_ = size;
  updateVBOdata();
};

std::vector<Triangle> Cuboid::triangles() const {
  std::vector<Triangle> v;
  v.reserve(nTriangles());

  // left and right side
  for (int q : {-1, 1}) {
    glm::vec3 normal(q, 0, 0);
    glm::vec3 a{q * size_.x / 2, size_.y / 2, size_.z / 2};
    glm::vec3 b{q * size_.x / 2, size_.y / 2, -size_.z / 2};
    glm::vec3 c{q * size_.x / 2, -size_.y / 2, -size_.z / 2};
    glm::vec3 d{q * size_.x / 2, -size_.y / 2, size_.z / 2};
    v.push_back(
        Triangle{Vertex{a, normal}, Vertex{b, normal}, Vertex{c, normal}});
    v.push_back(
        Triangle{Vertex{a, normal}, Vertex{d, normal}, Vertex{c, normal}});
  }

  // front and back side
  for (int q : {-1, 1}) {
    glm::vec3 normal(0, q, 0);
    glm::vec3 a{size_.x / 2, q * size_.y / 2, size_.z / 2};
    glm::vec3 b{size_.x / 2, q * size_.y / 2, -size_.z / 2};
    glm::vec3 c{-size_.x / 2, q * size_.y / 2, -size_.z / 2};
    glm::vec3 d{-size_.x / 2, q * size_.y / 2, size_.z / 2};
    v.push_back(
        Triangle{Vertex{a, normal}, Vertex{b, normal}, Vertex{c, normal}});
    v.push_back(
        Triangle{Vertex{a, normal}, Vertex{d, normal}, Vertex{c, normal}});
  }

  // top and bottom size
  for (int q : {-1, 1}) {
    glm::vec3 normal(0, 0, q);
    glm::vec3 a{size_.x / 2, size_.y / 2, q * size_.z / 2};
    glm::vec3 b{size_.x / 2, -size_.y / 2, q * size_.z / 2};
    glm::vec3 c{-size_.x / 2, -size_.y / 2, q * size_.z / 2};
    glm::vec3 d{-size_.x / 2, size_.y / 2, q * size_.z / 2};
    v.push_back(
        Triangle{Vertex{a, normal}, Vertex{b, normal}, Vertex{c, normal}});
    v.push_back(
        Triangle{Vertex{a, normal}, Vertex{d, normal}, Vertex{c, normal}});
  }

  return v;
}

int Cuboid::nTriangles() const {
  return 12;
}