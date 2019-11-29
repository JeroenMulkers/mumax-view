#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

class Field {
 public:
  Field();
  Field(glm::ivec3 gridsize);

  int ncells();
  glm::ivec3 gridsize();

  std::vector<glm::vec3> data;
  std::vector<glm::vec3> positions;

 private:
  glm::ivec3 gridsize_;
};

Field* readFieldFromOVF(std::string filename);
Field* testField(glm::ivec3 gridsize);