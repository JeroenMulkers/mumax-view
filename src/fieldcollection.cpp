#include <fstream>
#include <iostream>

#include "fieldcollection.hpp"
#include "ovf.hpp"
#include "vimag.hpp"

FieldCollection::FieldCollection(Vimag* vimag)
    : selectedIdx_(-1), vimag_(vimag) {}

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
  select(fields.size() - 1);
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
    add(field);
  } else {
    delete field;
  }
}

Field* FieldCollection::selectedField() const {
  if (selectedIdx_ < 0 || selectedIdx_ >= fields.size())
    return nullptr;
  return fields[selectedIdx_];
}

int FieldCollection::selectedFieldIdx() const {
  return selectedIdx_;
}

void FieldCollection::select(int Idx) {
  selectedIdx_ = Idx;
  if (vimag_)
    vimag_->updateField();
}

void FieldCollection::selectNext() {
  int newIdx = (selectedIdx_ + 1) % fields.size();
  select(newIdx);
}

void FieldCollection::selectPrevious() {
  int newIdx;
  if (selectedIdx_ == 0)
    newIdx = fields.size() - 1;
  else
    newIdx = selectedIdx_ - 1;
  select(newIdx);
}

int FieldCollection::size() const {
  return fields.size();
}
