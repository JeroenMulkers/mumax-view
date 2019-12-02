#include <fstream>
#include <regex>
#include <string>

#include "ovf.hpp"

static void readTextData(Field* field, std::ifstream& data) {
  for (auto& value : field->data) {
    data >> value.x;
    data >> value.y;
    data >> value.z;
  }
}

static void readBinaryData(Field* field, std::ifstream& data) {
  float magic;
  data.read(reinterpret_cast<char*>(&magic), sizeof(float));
  // TODO: check if magic number has a meaning
  for (auto& value : field->data) {
    data.read(reinterpret_cast<char*>(&value.x), sizeof(float));
    data.read(reinterpret_cast<char*>(&value.y), sizeof(float));
    data.read(reinterpret_cast<char*>(&value.z), sizeof(float));
  }
}

Field* readFieldFromOVF(std::string filename) {
  Field* field = nullptr;
  glm::ivec3 gridsize = {0, 0, 0};

  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::ios::failure("Error reading file");
  }

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

    if (!key.compare("Begin") && (!strValue.compare("Data Text") ||
                                  !strValue.compare("Data Binary 4"))) {
      field = new Field(gridsize);
      if (!strValue.compare("Data Binary 4")) {
        readBinaryData(field, file);
      } else if (!strValue.compare("Data Text")) {
        readTextData(field, file);
      } else {
        throw "Data format not understood";
      }

      return field;
    }
  }
  return field;
}