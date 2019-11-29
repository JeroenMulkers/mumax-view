#include "field.hpp"

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Field::Field() : Field(glm::ivec3(0, 0, 0)) {}

Field::Field(glm::ivec3 gridsize) : gridsize_(gridsize) {
  data.resize(ncells());
  positions.resize(ncells());
  for (int ix = 0; ix < gridsize.x; ix++) {
    for (int iy = 0; iy < gridsize.y; iy++) {
      for (int iz = 0; iz < gridsize.z; iz++) {
        int idx = iz * gridsize.x * gridsize.y + iy * gridsize.x + ix;
        positions[idx] = {(float)ix, (float)iy, (float)iz};
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

Field* readFieldFromOVF(std::string filename) {
  Field* field = nullptr;
  glm::ivec3 gridsize = {0, 0, 0};

  std::ifstream file(filename);

  std::regex headerRegex("^# (.*?): (.*?)$");
  std::smatch m;

  std::string line;
  while (std::getline(file, line)) {
    if (!std::regex_match(line, m, headerRegex))
      continue;

    std::string key = m[1];
    std::string strValue = m[2];

    if (!key.compare("xnodes"))
      gridsize.x = std::stoi(strValue);
    if (!key.compare("ynodes"))
      gridsize.y = std::stoi(strValue);
    if (!key.compare("znodes"))
      gridsize.z = std::stoi(strValue);

    if (!key.compare("Begin") && !strValue.compare("Data Text")) {
      field = new Field(gridsize);
      if (field->ncells() == 0) {
        std::cout << "Field is empty" << std::endl;
        return field;
      }
      for (auto& value : field->data) {
        file >> value.x;
        file >> value.y;
        file >> value.z;
      }
      return field;
    }
  }
  return field;
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