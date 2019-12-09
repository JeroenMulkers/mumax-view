#pragma once

#include <string>
#include <vector>

class Vimag;
class Field;

struct NamedField {
  NamedField(Field* field, std::string name);
  std::string name;
  Field* field;
};

class FieldCollection {
 public:
  FieldCollection(Vimag* vimag = nullptr);
  ~FieldCollection();

  void emptyCollection();
  void add(NamedField field);
  void load(std::string filename);

  void select(int Idx);
  void selectPrevious();
  void selectNext();

  Field* selectedField() const;
  std::string selectedFieldName() const;
  int selectedFieldIdx() const;
  int size() const;

 private:
  FieldCollection(const FieldCollection&);
  FieldCollection& operator=(const FieldCollection&);
  std::vector<NamedField> fields;
  int selectedIdx_;
  Vimag* vimag_;
};