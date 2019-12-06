#include <GLFW/glfw3.h>

#include "vimag.hpp"

Vimag::Vimag(GLFWwindow* window) : window_(window), timeIntervalTrigger(0.1) {
  fieldRenderer.putOnScene(&scene);
}

void Vimag::loop() {
  timeIntervalTrigger();
  if (scene.needRendering())
    scene.render();
  glfwSwapBuffers(window_);
  glfwPollEvents();  // alternative: glfwWaitEvents();
}