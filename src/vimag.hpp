#pragma once

#include <GLFW/glfw3.h>

#include "fieldcollection.hpp"
#include "fieldrenderer.hpp"
#include "scene.hpp"
#include "timeintervaltrigger.hpp"

class Vimag {
 public:
  Vimag(GLFWwindow* window) : window_(window), timeIntervalTrigger(0.1) {
    fieldRenderer.putOnScene(&scene);
  }

  void loop() {
    timeIntervalTrigger();
    if (scene.needRendering())
      scene.render();
    glfwSwapBuffers(window_);
    glfwPollEvents();  // alternative: glfwWaitEvents();
  }

  FieldCollection fieldCollection;
  Scene scene;
  TimeIntervalTrigger timeIntervalTrigger;
  FieldRenderer fieldRenderer;

 private:
  GLFWwindow* window_;
};