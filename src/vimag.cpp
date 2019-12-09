#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "fieldcollection.hpp"
#include "vimag.hpp"

Vimag::Vimag(GLFWwindow* window)
    : window_(window), timeIntervalTrigger(0.1), fieldCollection(this) {
  fieldRenderer.putOnScene(&scene);
  timeIntervalTrigger.setAction([&]() { fieldCollection.selectNext(); });
}

void Vimag::loop() {
  timeIntervalTrigger();
  if (scene.needRendering())
    scene.render();
  glfwSwapBuffers(window_);
  glfwPollEvents();  // alternative: glfwWaitEvents();
}

void Vimag::updateField() {
  fieldRenderer.setField(fieldCollection.selectedField());

// update web gui
#ifdef __EMSCRIPTEN__
  EM_ASM({ updateFieldSelector(); });
#endif
}