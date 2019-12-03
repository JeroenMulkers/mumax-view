#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

class Field {
 public:
  Field();
  Field(glm::ivec3 gridsize, glm::vec3 cellsize = {1, 1, 1});

  int ncells();
  glm::ivec3 gridsize();

  std::vector<glm::vec3> data;
  std::vector<glm::vec3> positions;

  glm::vec3 cellsize;

 private:
  glm::ivec3 gridsize_;
};

Field* testField(glm::ivec3 gridsize);