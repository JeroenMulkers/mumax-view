#include <fstream>
#include <iostream>

#include "fieldcollection.hpp"

FieldCollection::FieldCollection() : selectedIdx_(-1) {}

FieldCollection::~FieldCollection() {
  emptyCollection();
}

void FieldCollection::emptyCollection() {
  for (Field* f : fields) {
    delete f;
  }
  fields.clear();
  selectedIdx_ = -1;
}

void FieldCollection::add(Field* field) {
  fields.push_back(field);
  selectedIdx_ = fields.size() - 1;
}

void FieldCollection::load(std::string filename) {
  Field* field;
  bool succes = false;

  try {
    field = readFieldFromOVF(filename);
    succes = true;
  } catch (const std::fstream::failure& e) {
    std::cerr << e.what() << std::endl;
  }

  if (succes) {
    fields.push_back(field);
    selectedIdx_ = fields.size() - 1;
  } else {
    delete field;
  }
}

Field* FieldCollection::selectedField() const {
  if (selectedIdx_ < 0 || selectedIdx_ >= fields.size())
    return nullptr;

  std::cout << selectedIdx_ << std::endl;
  return fields[selectedIdx_];
}

int FieldCollection::selectedFieldIdx() const {
  return selectedIdx_;
}

void FieldCollection::select(int Idx) {
  selectedIdx_ = Idx;
}

void FieldCollection::selectNext() {
  selectedIdx_ = (selectedIdx_ + 1) % fields.size();
}

void FieldCollection::selectPrevious() {
  if (selectedIdx_ == 0)
    selectedIdx_ = fields.size() - 1;
  else
    selectedIdx_--;
}

int FieldCollection::size() const {
  return fields.size();
}
