#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

class Field {
 public:
  Field();
  Field(glm::ivec3 gridsize);

  void updateVBOs();

  int ncells();
  glm::ivec3 gridsize();
  unsigned int positionVBO();
  unsigned int vectorsVBO();

  std::vector<glm::vec3> data;
  std::vector<glm::vec3> positions;

 private:
  glm::ivec3 gridsize_;
  unsigned int positionVBO_;
  unsigned int vectorsVBO_;
};

Field* readFieldFromOVF(std::string filename);
Field* testField(glm::ivec3 gridsize);