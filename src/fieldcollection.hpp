#pragma once

#include <vector>

#include "field.hpp"
#include "ovf.hpp"

class FieldCollection {
 public:
  FieldCollection();
  ~FieldCollection();

  void emptyCollection();
  void add(Field* field);
  void load(std::string filename);

  void select(int Idx);
  void selectPrevious();
  void selectNext();

  Field* selectedField() const;
  int selectedFieldIdx() const;
  int size() const;

 private:
  std::vector<Field*> fields;
  int selectedIdx_;
};
