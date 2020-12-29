#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fieldboxrenderer.hpp"
#include "fieldcollection.hpp"
#include "fieldrenderer.hpp"
#include "scene.hpp"
#include "timeintervaltrigger.hpp"
#include "application.hpp"

enum PlayDirection { PLAYFORWARD, PLAYBACKWARD };

class Viewer : public Application {
 public:
  static Viewer& get() {
    static Viewer viewer; 
    return viewer;
  }

 private:
  Viewer();

 public:
  Viewer(const Viewer&) = delete;
  void operator=(const Viewer&) = delete;

  void updateField();
  void setPlayDirection(PlayDirection);

  void loop();

  void frameBufferSizeCallback(int width, int height);
  void keyCallBack(int key, int scancode, int action, int mods);
  void scrollCallback(double xoffset, double yoffset);
  void mouseButtonCallback(int button, int action, int mods);
  void curserPosCallback(double xpos, double ypos);

  FieldCollection fieldCollection;
  Scene scene;
  TimeIntervalTrigger timeIntervalTrigger;
  FieldRenderer fieldRenderer;
  FieldBoxRenderer fieldBoxRenderer;
};