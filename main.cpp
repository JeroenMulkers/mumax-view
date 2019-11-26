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

#include "arrowmodel.hpp"
#include "camera.hpp"
#include "field.hpp"
#include "fieldrenderer.hpp"
#include "shaderprogram.hpp"
#include "shaders.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::function<void()> loop;
void main_loop() {
  loop();
};

GLFWwindow* window;
FieldRenderer* renderer;

extern "C" {

#ifdef __EMSCRIPTEN__
void EMSCRIPTEN_KEEPALIVE updateArrowShaftRadius(float r) {
  renderer->arrow.setShaftRadius(r);
  renderer->needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowHeadRadius(float r) {
  renderer->arrow.setHeadRadius(r);
  renderer->needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowHeadRatio(float r) {
  renderer->arrow.setHeadRatio(r);
  renderer->needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowSegments(float n) {
  renderer->arrow.setSegments(n);
  renderer->needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowScalingsFactor(float s) {
  renderer->arrowScalingsFactor = s;
  renderer->needRender = true;
}

void EMSCRIPTEN_KEEPALIVE loadfile(std::string filename) {
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

void EMSCRIPTEN_KEEPALIVE updateCanvasSize() {
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  renderer->needRender = true;
  glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  renderer->needRender = true;
  renderer->camera.targetDistance *= (1 - 0.1 * (float)yoffset);
  // camera.position += (float)yoffset * 0.1f * (camera.target -
  // camera.position);
}

class Mouse {
 public:
  bool leftButtonDown;
  bool middleButtonDown;
  double lastX;
  double lastY;
};

Mouse mouse;

static void mouse_callback(GLFWwindow* window,
                           int button,
                           int action,
                           int mods) {
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

void mousemove_callback(GLFWwindow* window, double xpos, double ypos) {
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

  if (mouse.middleButtonDown) {
    renderer->needRender = true;
    // todo: move camera
    renderer->camera.target -= 0.05f * yoffset * renderer->camera.up();
    // camera.target[0] -= 0.05 * xoffset;
    // camera.target[2] -= 0.05 * yoffset;
  }
}

static void error_callback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
}

int main() {
  // ------- INIT GLFW ----------------------------------------

  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    std::cerr << "glfwInit failed" << std::endl;
    return -1;
  }

  // ------- CREATE WINDOW ----------------------------------------

#ifdef __EMSCRIPTEN__
  int width = canvas_get_width();
  int height = canvas_get_height();
#else
  int width = SCR_WIDTH;
  int height = SCR_HEIGHT;
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
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_callback);
  glfwSetCursorPosCallback(window, mousemove_callback);

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
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}