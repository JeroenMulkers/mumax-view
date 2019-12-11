#include "field.hpp"

#include <vector>

#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Field::Field() : Field(glm::ivec3(0, 0, 0)) {}

Field::Field(glm::ivec3 gridsize, glm::vec3 cellsize)
    : gridsize_(gridsize), cellsize(cellsize) {
  data.resize(ncells());
  positions.resize(ncells());
  glm::vec3 shift;
  shift.x = -0.5 * cellsize.x * (gridsize.x - 1);
  shift.y = -0.5 * cellsize.y * (gridsize.y - 1);
  shift.z = -0.5 * cellsize.z * (gridsize.z - 1);
  for (int ix = 0; ix < gridsize.x; ix++) {
    for (int iy = 0; iy < gridsize.y; iy++) {
      for (int iz = 0; iz < gridsize.z; iz++) {
        int idx = iz * gridsize.x * gridsize.y + iy * gridsize.x + ix;
        positions[idx] = shift + glm::vec3{ix * cellsize.x, iy * cellsize.y,
                                           iz * cellsize.z};
      }
    }
  }
}

int Field::ncells() {
  return gridsize_.x * gridsize_.y * gridsize_.z;
}

glm::ivec3 Field::gridsize() {
  return gridsize_;
}

Field* testField(glm::ivec3 gridsize) {
  Field* field = new Field(gridsize);
  for (int ix = 0; ix < field->gridsize().x; ix++) {
    for (int iy = 0; iy < field->gridsize().y; iy++) {
      for (int iz = 0; iz < field->gridsize().z; iz++) {
        int idx = iz * field->gridsize().x * field->gridsize().y +
                  iy * field->gridsize().x + ix;
        float phi =
            (float)iy / (float)field->gridsize().y * 2 * glm::pi<float>();
        float theta = (float)ix / (float)field->gridsize().x * glm::pi<float>();
        field->data[idx] =
            glm::vec3(glm::cos(phi) * glm::sin(theta),
                      glm::sin(phi) * glm::sin(theta), glm::cos(theta));
      }
    }
  }
  return field;
}