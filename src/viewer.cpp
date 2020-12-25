#include <GLFW/glfw3.h>

#include <emscripten/emscripten.h>

#include "fieldcollection.hpp"
#include "viewer.hpp"

Viewer::Viewer(GLFWwindow* window)
    : window_(window), timeIntervalTrigger(0.1), fieldCollection(this) {
  fieldRenderer.putOnScene(&scene);
  fieldBoxRenderer.putOnScene(&scene);
  setPlayDirection(PLAYFORWARD);
}

void Viewer::loop() {
  timeIntervalTrigger();
  if (scene.needRendering())
    scene.render();
  glfwSwapBuffers(window_);
  glfwPollEvents();  // alternative: glfwWaitEvents();
}

void Viewer::updateField() {
  fieldRenderer.setField(fieldCollection.selectedField());
  fieldBoxRenderer.setField(fieldCollection.selectedField());
  EM_ASM({ updateFieldSelector(); });
}

void Viewer::setPlayDirection(PlayDirection playDirection) {
  if (playDirection == PLAYFORWARD) {
    timeIntervalTrigger.setAction([&]() { fieldCollection.selectNext(); });
  } else {
    timeIntervalTrigger.setAction([&]() { fieldCollection.selectPrevious(); });
  }
}