#pragma once

#include <vector>

class Vimag;
class Field;

class FieldCollection {
 public:
  FieldCollection(Vimag* vimag = nullptr);
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
  FieldCollection(const FieldCollection&);
  FieldCollection& operator=(const FieldCollection&);
  std::vector<Field*> fields;
  int selectedIdx_;
  Vimag* vimag_;
};