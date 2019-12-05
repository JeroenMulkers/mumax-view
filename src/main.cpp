#include <cmath>
#include <fstream>
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

#include "fieldcollection.hpp"
#include "fieldrenderer.hpp"
#include "ovf.hpp"
#include "scene.hpp"

std::function<void()> loop;
void main_loop() {
  loop();
};

class TimeIntervalTrigger {
 public:
  TimeIntervalTrigger(double timeInterval)
      : action_([] {}), active_(false), timeInterval_(timeInterval) {}

  void setAction(std::function<void()> action) { action_ = action; }

  void setTimeInterval(double timeInterval) { timeInterval_ = timeInterval; }

  void start() {
    active_ = true;
    previousTime_ = glfwGetTime();
  }

  void stop() { active_ = false; }

  void operator()() {
    if (!active_)
      return;

    double time = glfwGetTime();
    double delta = time - previousTime_;

    // number of time intervals since previous action
    int nIntervals = static_cast<int>(floor(delta / timeInterval_));

    for (int i = 0; i < nIntervals; i++)
      action_();

    if (nIntervals > 0)
      previousTime_ = time;
  }

  bool isActive() const { return active_; }

 private:
  double previousTime_;
  double timeInterval_;
  bool active_;
  std::function<void()> action_;
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
GLFWwindow* window;
Scene scene;
FieldRenderer* renderer;
FieldCollection fieldCollection;
TimeIntervalTrigger timeIntervalTrigger(0.1);

extern "C" {
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void setScrollSensitivity(double sensitivity) {
  mouse.scrollSensitivity = sensitivity;
}
EMSCRIPTEN_KEEPALIVE
void useArrowGlyph() {
  renderer->setGlyphType(ARROW);
}
EMSCRIPTEN_KEEPALIVE
void useCuboidGlyph() {
  renderer->setGlyphType(CUBOID);
}
EMSCRIPTEN_KEEPALIVE
void updateArrowShaftRadius(float r) {
  renderer->arrow.setShaftRadius(r);
  renderer->ensureRendering();
}
EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRadius(float r) {
  renderer->arrow.setHeadRadius(r);
  renderer->ensureRendering();
}
EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRatio(float r) {
  renderer->arrow.setHeadRatio(r);
  renderer->ensureRendering();
}
EMSCRIPTEN_KEEPALIVE
void updateArrowScalingsFactor(float s) {
  renderer->setArrowScalingsFactor(s);
}
EMSCRIPTEN_KEEPALIVE
void updateCuboidScalingsFactor(float s) {
  renderer->setCuboidScalingsFactor(s);
}
EMSCRIPTEN_KEEPALIVE
void loadfile(std::string filename) {
  fieldCollection.load(filename);
  renderer->setField(fieldCollection.selectedField());
}
EMSCRIPTEN_KEEPALIVE
void emptyFieldCollection() {
  return fieldCollection.emptyCollection();
}
EMSCRIPTEN_KEEPALIVE
int fieldCollectionSize() {
  return fieldCollection.size();
}
EMSCRIPTEN_KEEPALIVE
int fieldCollectionSelected() {
  return fieldCollection.selectedFieldIdx();
}
EMSCRIPTEN_KEEPALIVE
void fieldCollectionSelect(int idx) {
  fieldCollection.select(idx);
  renderer->setField(fieldCollection.selectedField());
}
EMSCRIPTEN_KEEPALIVE
void setTimeInterval(double timeInterval) {
  timeIntervalTrigger.setTimeInterval(timeInterval);
}
EMSCRIPTEN_KEEPALIVE
void startTimeIntervalTrigger() {
  timeIntervalTrigger.start();
}
EMSCRIPTEN_KEEPALIVE
void stopTimeIntervalTrigger() {
  timeIntervalTrigger.stop();
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
  glClearColor(r, g, b, 1.0f);
  scene.ensureRendering();
}
EMSCRIPTEN_KEEPALIVE
void setAmbientLighting(float intensity) {
  renderer->shader.setFloat("ambientLight", intensity);
  scene.ensureRendering();
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
  renderer->setGradientColorScheme(
      glm::mat3(r1, g1, b1, r2, g2, b2, r3, g3, b3));
}
EMSCRIPTEN_KEEPALIVE
void setMumaxColorScheme() {
  renderer->setMumaxColorScheme();
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

  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);
#ifndef __EMSCRIPTEN__
  glEnable(GL_MULTISAMPLE);  // Needed for anti-aliasing
#endif

  // -------- CREATE RENDERER ----------------------------------------------

  renderer = new FieldRenderer();
  renderer->putOnScene(&scene);

#ifdef __EMSCRIPTEN__
  fieldCollection.add(testField(glm::ivec3(50, 50, 1)));
  renderer->setField(fieldCollection.selectedField());
#else
  if (argc > 1) {
    // std::string filename(argv[1]);
    try {
      fieldCollection.add(readFieldFromOVF(argv[1]));
      renderer->setField(fieldCollection.selectedField());
    } catch (const std::fstream::failure& e) {
      std::cerr << e.what() << std::endl;
      return -1;
    }

  } else {
    fieldCollection.add(testField(glm::ivec3(50, 50, 1)));
    renderer->setField(fieldCollection.selectedField());
  }
#endif

  //--------- MAIN LOOP ----------------------------------------------------

  timeIntervalTrigger.setAction([&]() {
    fieldCollection.selectNext();
    renderer->setField(fieldCollection.selectedField());
  });

  loop = [&] {
    timeIntervalTrigger();

    if (scene.needRendering())
      scene.render();

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
  scene.ensureRendering();
  glViewport(0, 0, width, height);
}

void keyCallBack(GLFWwindow* window,
                 int key,
                 int scancode,
                 int action,
                 int mods) {
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (mods == 0) {
      scene.camera()->setYaw(glm::pi<float>() / 2);
    } else if (mods == 1) {
      scene.camera()->setYaw(-glm::pi<float>() / 2);
    }
    scene.camera()->setPitch(0);

  } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    if (mods == 0) {
      scene.camera()->setYaw(0);
    } else if (mods == 1) {
      scene.camera()->setYaw(glm::pi<float>());
    }
    scene.camera()->setPitch(0);

  } else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    if (mods == 0) {
      scene.camera()->setPitch(glm::pi<float>() / 2);
    } else if (mods == 1) {
      scene.camera()->setPitch(-glm::pi<float>() / 2);
    }
    scene.camera()->setYaw(0);

  } else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    fieldCollection.selectNext();
    renderer->setField(fieldCollection.selectedField());

  } else if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    fieldCollection.selectPrevious();
    renderer->setField(fieldCollection.selectedField());
  }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  scene.ensureRendering();
  float dist = scene.camera()->distance();
  scene.camera()->setDistance(
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

    float newYaw = scene.camera()->yaw() + sensitivity * xoffset;
    float newPitch = scene.camera()->pitch() - sensitivity * yoffset;

    scene.camera()->setYaw(newYaw);
    scene.camera()->setPitch(newPitch);
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