#pragma once

#include <GLFW/glfw3.h>

#include "fieldboxrenderer.hpp"
#include "fieldcollection.hpp"
#include "fieldrenderer.hpp"
#include "scene.hpp"
#include "timeintervaltrigger.hpp"

enum PlayDirection { PLAYFORWARD, PLAYBACKWARD };

class Viewer {
 public:
  Viewer(GLFWwindow* window);

  void loop();
  void updateField();
  void setPlayDirection(PlayDirection);

  FieldCollection fieldCollection;
  Scene scene;
  TimeIntervalTrigger timeIntervalTrigger;
  FieldRenderer fieldRenderer;
  FieldBoxRenderer fieldBoxRenderer;

 private:
  Viewer(const Viewer&);
  Viewer& operator=(const Viewer&);
  GLFWwindow* window_;
};