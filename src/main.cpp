#include <fstream>
#include <functional>
#include <iostream>

// USE GLES API FOR RENDERING
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#else
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ovf.hpp"
#include "vimag.hpp"

GLFWwindow* window;
Vimag* vimag;
void main_loop() {
  vimag->loop();
};

// TODO: put mouse and window in a descent window class
class Mouse {
 public:
  Mouse() : scrollSensitivity(0.1){};
  bool leftButtonDown;
  bool middleButtonDown;
  double lastX;
  double lastY;
  double scrollSensitivity;
};
Mouse mouse;

std::string getFieldName() {
  return vimag->fieldCollection.selectedFieldName();
}

void loadConfigFile(std::string filename) {
  vimag->fieldCollection.load(filename);
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(myModule) {
  emscripten::function("getFieldName", &getFieldName);
  emscripten::function("loadConfigFile", &loadConfigFile);
}
#endif

extern "C" {
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void setScrollSensitivity(double sensitivity) {
  mouse.scrollSensitivity = sensitivity;
}
EMSCRIPTEN_KEEPALIVE
void useArrowGlyph() {
  vimag->fieldRenderer.setGlyphType(ARROW);
}
EMSCRIPTEN_KEEPALIVE
void useCuboidGlyph() {
  vimag->fieldRenderer.setGlyphType(CUBOID);
}
EMSCRIPTEN_KEEPALIVE
void updateArrowShaftRadius(float r) {
  vimag->fieldRenderer.arrow.setShaftRadius(r);
}
EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRadius(float r) {
  vimag->fieldRenderer.arrow.setHeadRadius(r);
}
EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRatio(float r) {
  vimag->fieldRenderer.arrow.setHeadRatio(r);
}
EMSCRIPTEN_KEEPALIVE
void updateArrowScalingsFactor(float s) {
  vimag->fieldRenderer.setArrowScalingsFactor(s);
}
EMSCRIPTEN_KEEPALIVE
void updateCuboidScalingsFactor(float s) {
  vimag->fieldRenderer.setCuboidScalingsFactor(s);
}
EMSCRIPTEN_KEEPALIVE
void emptyFieldCollection() {
  return vimag->fieldCollection.emptyCollection();
}
EMSCRIPTEN_KEEPALIVE
int fieldCollectionSize() {
  return vimag->fieldCollection.size();
}
EMSCRIPTEN_KEEPALIVE
int fieldCollectionSelected() {
  return vimag->fieldCollection.selectedFieldIdx();
}
EMSCRIPTEN_KEEPALIVE
void fieldCollectionSelect(int idx) {
  vimag->fieldCollection.select(idx);
}
EMSCRIPTEN_KEEPALIVE
void setTimeInterval(double timeInterval) {
  vimag->timeIntervalTrigger.setTimeInterval(timeInterval);
}
EMSCRIPTEN_KEEPALIVE
void startTimeIntervalTrigger() {
  vimag->timeIntervalTrigger.start();
}
EMSCRIPTEN_KEEPALIVE
void stopTimeIntervalTrigger() {
  vimag->timeIntervalTrigger.stop();
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
}
EMSCRIPTEN_KEEPALIVE
void setBackgroundColor(float r, float g, float b) {
  vimag->scene.setBackgroundColor(r, g, b);
}
EMSCRIPTEN_KEEPALIVE
void setOutlineVisibility(bool visible) {
  std::cout << visible << std::endl;
  vimag->fieldBoxRenderer.setVisibility(visible);
}
EMSCRIPTEN_KEEPALIVE
void setOutlineColor(float r, float g, float b) {
  vimag->fieldBoxRenderer.setColor(r, g, b);
}
EMSCRIPTEN_KEEPALIVE
void setAmbientLighting(float intensity) {
  vimag->fieldRenderer.shader.setFloat("ambientLight", intensity);
}
EMSCRIPTEN_KEEPALIVE
void setGradientColorScheme(float r1,
                            float g1,
                            float b1,
                            float r2,
                            float g2,
                            float b2,
                            float r3,
                            float g3,
                            float b3) {
  vimag->fieldRenderer.setGradientColorScheme(
      glm::mat3(r1, g1, b1, r2, g2, b2, r3, g3, b3));
}
EMSCRIPTEN_KEEPALIVE
void setMumaxColorScheme() {
  vimag->fieldRenderer.setMumaxColorScheme();
}
#endif
}

// ------- GLFW CALLBACKS ------------------------------------------------------
void frameBufferSizeCallback(GLFWwindow* w, int width, int height);
void scrollCallback(GLFWwindow* w, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods);
void curserPosCallback(GLFWwindow* w, double xpos, double ypos);
void errorCallback(int error, const char* description);
void keyCallBack(GLFWwindow* w, int key, int scancode, int action, int mods);

int main(int argc, char** argv) {
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

  window = glfwCreateWindow(width, height, "ViMag", NULL, NULL);

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
  glfwSetKeyCallback(window, keyCallBack);

  // -------- GL SETTINGS --------------------------------------------------

  glEnable(GL_DEPTH_TEST);
#ifndef __EMSCRIPTEN__
  glEnable(GL_MULTISAMPLE);  // Needed for anti-aliasing
#endif

  vimag = new Vimag(window);

  // -------- LOAD INITIAL FIELD --------------------------------------------

#ifdef __EMSCRIPTEN__
  vimag->fieldCollection.add(
      NamedField(testField(glm::ivec3(50, 50, 1)), "example"));
#else
  if (argc > 1) {
    // std::string filename(argv[1]);
    try {
      vimag->fieldCollection.load(argv[1]);
    } catch (const std::fstream::failure& e) {
      std::cerr << e.what() << std::endl;
      return -1;
    }

  } else {
    vimag->fieldCollection.add(
        NamedField(testField(glm::ivec3(50, 50, 1)), "example"));
  }
#endif

  //--------- MAIN LOOP ----------------------------------------------------

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, true);
#else
  while (!glfwWindowShouldClose(window))
    vimag->loop();
#endif

  //-------- CLEAN UP --------------------------------------------------------

  glfwTerminate();
  return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  vimag->scene.ensureRendering();
  glViewport(0, 0, width, height);
}

void keyCallBack(GLFWwindow* window,
                 int key,
                 int scancode,
                 int action,
                 int mods) {
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (mods == 0) {
      vimag->scene.camera.setYaw(glm::pi<float>() / 2);
    } else if (mods == 1) {
      vimag->scene.camera.setYaw(-glm::pi<float>() / 2);
    }
    vimag->scene.camera.setPitch(0);

  } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    if (mods == 0) {
      vimag->scene.camera.setYaw(0);
    } else if (mods == 1) {
      vimag->scene.camera.setYaw(glm::pi<float>());
    }
    vimag->scene.camera.setPitch(0);

  } else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    if (mods == 0) {
      vimag->scene.camera.setPitch(glm::pi<float>() / 2);
    } else if (mods == 1) {
      vimag->scene.camera.setPitch(-glm::pi<float>() / 2);
    }
    vimag->scene.camera.setYaw(0);

  } else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    vimag->timeIntervalTrigger.start();
  } else if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
    vimag->timeIntervalTrigger.stop();

  } else if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    vimag->setPlayDirection(PLAYBACKWARD);
    vimag->timeIntervalTrigger.start();
  } else if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
    vimag->timeIntervalTrigger.stop();
    vimag->setPlayDirection(PLAYFORWARD);  // set back default play direction

  } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (vimag->timeIntervalTrigger.isActive()) {
      vimag->timeIntervalTrigger.stop();
    } else {
      vimag->timeIntervalTrigger.start();
    }
  }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  float dist = vimag->scene.camera.distance();
  vimag->scene.camera.setDistance(
      dist * (1.0 - static_cast<float>(mouse.scrollSensitivity * yoffset)));
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
    float sensitivity = 0.02;

    float newYaw = vimag->scene.camera.yaw() + sensitivity * xoffset;
    float newPitch = vimag->scene.camera.pitch() - sensitivity * yoffset;

    vimag->scene.camera.setYaw(newYaw);
    vimag->scene.camera.setPitch(newPitch);
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