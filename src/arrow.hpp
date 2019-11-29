#pragma once

#include <vector>

#include "glyph.hpp"

class Arrow : public Glyph {
 public:
  Arrow(float shaftRadius, float headRadius, float headRatio, int nSegments);

  void setShaftRadius(float shaftRadius);
  void setHeadRadius(float headRadius);
  void setHeadRatio(float headRatio);
  void setSegments(float nSegments);

  std::vector<Triangle> triangles() const;
  bool hasShaft() const;
  int nTriangles() const;

 private:
  float shaftRadius_;
  float headRadius_;
  float headRatio_;
  int nSegments_;
};