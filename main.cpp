#include <functional>
#include <iostream>

// USE GLES API FOR RENDERING
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fieldrenderer.hpp"

std::function<void()> loop;
void main_loop() {
  loop();
};

// TODO: put mouse and window in a descent window class
class Mouse {
 public:
  bool leftButtonDown;
  bool middleButtonDown;
  double lastX;
  double lastY;
};
Mouse mouse;
GLFWwindow* window;

FieldRenderer* renderer;

extern "C" {
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void updateArrowShaftRadius(float r) {
  renderer->arrow.setShaftRadius(r);
  renderer->needRender = true;
}
EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRadius(float r) {
  renderer->arrow.setHeadRadius(r);
  renderer->needRender = true;
}
EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRatio(float r) {
  renderer->arrow.setHeadRatio(r);
  renderer->needRender = true;
}
EMSCRIPTEN_KEEPALIVE
void updateArrowSegments(float n) {
  renderer->arrow.setSegments(n);
  renderer->needRender = true;
}
EMSCRIPTEN_KEEPALIVE
void updateArrowScalingsFactor(float s) {
  renderer->arrowScalingsFactor = s;
  renderer->needRender = true;
}
EMSCRIPTEN_KEEPALIVE
void loadfile(std::string filename) {
  std::cout << filename << std::endl;
  Field* oldfield = renderer->field_;
  renderer->setField(readFieldFromOVF(filename));
  delete oldfield;
}
EM_JS(int, canvas_get_width, (), {
  return document.getElementById('canvas').scrollWidth;
});
EM_JS(int, canvas_get_height, (), {
  return document.getElementById('canvas').scrollHeight;
});
EMSCRIPTEN_KEEPALIVE
void updateCanvasSize() {
  if (!window) {
    return;
  }
  int width = canvas_get_width();
  int height = canvas_get_height();
  glfwSetWindowSize(window, width, height);
  glViewport(0, 0, width, height);
  renderer->needRender = true;
}
#endif
}

// ------- GLFW CALLBACKS ------------------------------------------------------
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void curserPosCallback(GLFWwindow* window, double xpos, double ypos);
void errorCallback(int error, const char* description);

int main() {
  // ------- INIT GLFW ---------------------------------------------------------

  glfwSetErrorCallback(errorCallback);
  if (!glfwInit()) {
    std::cerr << "glfwInit failed" << std::endl;
    return -1;
  }

  // ------- CREATE WINDOW -----------------------------------------------------

#ifdef __EMSCRIPTEN__
  int width = canvas_get_width();
  int height = canvas_get_height();
#else
  int width = 800;
  int height = 600;
#endif

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);  // anti-aliasing
  glfwWindowHint(GLFW_RESIZABLE, true);

  window = glfwCreateWindow(width, height, "magvis", NULL, NULL);

  if (!window) {
    std::cerr << "Can not create window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, curserPosCallback);

  // -------- GL SETTINGS --------------------------------------------------

  glClearColor(0.3f, 0.3f, 0.3f, 0.1f);
  glEnable(GL_DEPTH_TEST);
#ifndef __EMSCRIPTEN__
  glEnable(GL_MULTISAMPLE);  // Needed for anti-aliasing
#endif

  // -------- CREATE RENDERER ----------------------------------------------

  renderer = new FieldRenderer(testField(glm::ivec3(50, 50, 2)));

  //--------- MAIN LOOP ----------------------------------------------------

  loop = [&] {
    if (renderer->needRender) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      renderer->render();
    }
    glfwSwapBuffers(window);
    glfwPollEvents();  // alternative: glfwWaitEvents();
  };

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, true);
#else
  while (!glfwWindowShouldClose(window))
    loop();
#endif

  //-------- CLEAN UP --------------------------------------------------------

  delete renderer;
  glfwTerminate();
  return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  renderer->needRender = true;
  glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  renderer->needRender = true;
  renderer->camera.targetDistance *= (1 - 0.1 * (float)yoffset);
  // camera.position += (float)yoffset * 0.1f * (camera.target -
  // camera.position);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (GLFW_PRESS == action) {
      mouse.leftButtonDown = true;
      glfwGetCursorPos(window, &mouse.lastX, &mouse.lastY);
    } else if (GLFW_RELEASE == action) {
      mouse.leftButtonDown = false;
    }
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    if (GLFW_PRESS == action) {
      mouse.middleButtonDown = true;
      glfwGetCursorPos(window, &mouse.lastX, &mouse.lastY);
    } else if (GLFW_RELEASE == action) {
      mouse.middleButtonDown = false;
    }
  }
}

void curserPosCallback(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = xpos - mouse.lastX;
  float yoffset = mouse.lastY - ypos;
  mouse.lastX = xpos;
  mouse.lastY = ypos;

  if (mouse.leftButtonDown) {
    renderer->needRender = true;
    float sensitivity = 0.02;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    renderer->camera.yaw += xoffset;
    renderer->camera.pitch -= yoffset;
  }
  // TODO: fix middle button feature
  // if (mouse.middleButtonDown) {
  //  renderer->needRender = true;
  //  renderer->camera.target -= 0.05f * yoffset * renderer->camera.up();
  //  // camera.target[0] -= 0.05 * xoffset;
  //  // camera.target[2] -= 0.05 * yoffset;
  //}
}

void errorCallback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
}