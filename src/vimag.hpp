#pragma once

#include <GLFW/glfw3.h>

#include "fieldcollection.hpp"
#include "fieldrenderer.hpp"
#include "scene.hpp"
#include "timeintervaltrigger.hpp"

class Vimag {
 public:
  Vimag(GLFWwindow* window);

  void loop();
  void updateField();

  FieldCollection fieldCollection;
  Scene scene;
  TimeIntervalTrigger timeIntervalTrigger;
  FieldRenderer fieldRenderer;

 private:
  Vimag(const Vimag&);
  Vimag& operator=(const Vimag&);
  GLFWwindow* window_;
};