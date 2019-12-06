#pragma once

#include <GLFW/glfw3.h>

#include "fieldcollection.hpp"
#include "fieldrenderer.hpp"
#include "scene.hpp"
#include "timeintervaltrigger.hpp"

class Vimag {
 public:
  Vimag(GLFWwindow* window) : window_(window), timeIntervalTrigger(0.1) {
    renderer = new FieldRenderer();
    renderer->putOnScene(&scene);
  }

  ~Vimag() { delete renderer; }

  void loop() {
    timeIntervalTrigger();
    if (scene.needRendering())
      scene.render();
    glfwSwapBuffers(window_);
    glfwPollEvents();  // alternative: glfwWaitEvents();
  }

  Scene scene;
  FieldRenderer* renderer;
  FieldCollection fieldCollection;
  TimeIntervalTrigger timeIntervalTrigger;

 private:
  GLFWwindow* window_;
};