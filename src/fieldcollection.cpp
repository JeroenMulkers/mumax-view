#include <fstream>
#include <string>
#include <vector>

#include "fieldcollection.hpp"
#include "ovf.hpp"
#include "viewer.hpp"

NamedField::NamedField(Field* field, std::string name)
    : field(field), name(name) {}

FieldCollection::FieldCollection(Viewer* viewer)
    : selectedIdx_(-1), viewer_(viewer) {}

FieldCollection::~FieldCollection() {
  emptyCollection();
}

void FieldCollection::emptyCollection() {
  for (NamedField f : fields) {
    delete f.field;
  }
  fields.clear();
  selectedIdx_ = -1;
}

void FieldCollection::add(NamedField field) {
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
    add(NamedField(field, filename));
  } else {
    delete field;
  }
}

Field* FieldCollection::selectedField() const {
  if (selectedIdx_ < 0 || selectedIdx_ >= fields.size())
    return nullptr;
  return fields[selectedIdx_].field;
}

std::string FieldCollection::selectedFieldName() const {
  if (selectedIdx_ < 0 || selectedIdx_ >= fields.size())
    return nullptr;
  return fields[selectedIdx_].name;
}

int FieldCollection::selectedFieldIdx() const {
  return selectedIdx_;
}

void FieldCollection::select(int Idx) {
  selectedIdx_ = Idx;
  if (viewer_)
    viewer_->updateField();
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
