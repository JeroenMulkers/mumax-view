#include <fstream>
#include <functional>
#include <iostream>

#include <emscripten/bind.h>
#include <emscripten/emscripten.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ovf.hpp"
#include "viewer.hpp"

GLFWwindow* window;
Viewer* viewer;
void main_loop() {
  viewer->loop();
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
  return viewer->fieldCollection.selectedFieldName();
}

void loadConfigFile(std::string filename) {
  viewer->fieldCollection.load(filename);
}

EMSCRIPTEN_BINDINGS(myModule) {
  emscripten::function("getFieldName", &getFieldName);
  emscripten::function("loadConfigFile", &loadConfigFile);
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void setScrollSensitivity(double sensitivity) {
  mouse.scrollSensitivity = sensitivity;
}

EMSCRIPTEN_KEEPALIVE
void useArrowGlyph() {
  viewer->fieldRenderer.setGlyphType(ARROW);
}

EMSCRIPTEN_KEEPALIVE
void useCuboidGlyph() {
  viewer->fieldRenderer.setGlyphType(CUBOID);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowShaftRadius(float r) {
  viewer->fieldRenderer.arrow.setShaftRadius(r);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRadius(float r) {
  viewer->fieldRenderer.arrow.setHeadRadius(r);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRatio(float r) {
  viewer->fieldRenderer.arrow.setHeadRatio(r);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowScalingsFactor(float s) {
  viewer->fieldRenderer.setArrowScalingsFactor(s);
}

EMSCRIPTEN_KEEPALIVE
void updateCuboidScalingsFactor(float s) {
  viewer->fieldRenderer.setCuboidScalingsFactor(s);
}

EMSCRIPTEN_KEEPALIVE
void updateRelativeRange(float xmin,
                         float ymin,
                         float zmin,
                         float xmax,
                         float ymax,
                         float zmax) {
  std::cout << xmin << std::endl;
  viewer->fieldRenderer.setRelativeRange({xmin, ymin, zmin},
                                         {xmax, ymax, zmax});
}

EMSCRIPTEN_KEEPALIVE
void emptyFieldCollection() {
  return viewer->fieldCollection.emptyCollection();
}

EMSCRIPTEN_KEEPALIVE
int fieldCollectionSize() {
  return viewer->fieldCollection.size();
}

EMSCRIPTEN_KEEPALIVE
int fieldCollectionSelected() {
  return viewer->fieldCollection.selectedFieldIdx();
}

EMSCRIPTEN_KEEPALIVE
void fieldCollectionSelect(int idx) {
  viewer->fieldCollection.select(idx);
}

EMSCRIPTEN_KEEPALIVE
void setTimeInterval(double timeInterval) {
  viewer->timeIntervalTrigger.setTimeInterval(timeInterval);
}

EMSCRIPTEN_KEEPALIVE
void startTimeIntervalTrigger() {
  viewer->timeIntervalTrigger.start();
}

EMSCRIPTEN_KEEPALIVE
void stopTimeIntervalTrigger() {
  viewer->timeIntervalTrigger.stop();
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
  viewer->scene.setBackgroundColor(r, g, b);
}

EMSCRIPTEN_KEEPALIVE
void setOutlineVisibility(bool visible) {
  std::cout << visible << std::endl;
  viewer->fieldBoxRenderer.setVisibility(visible);
}

EMSCRIPTEN_KEEPALIVE
void setOutlineColor(float r, float g, float b) {
  viewer->fieldBoxRenderer.setColor(r, g, b);
}

EMSCRIPTEN_KEEPALIVE
void setAmbientLighting(float intensity) {
  viewer->fieldRenderer.shader.setFloat("ambientLight", intensity);
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
  viewer->fieldRenderer.setGradientColorScheme(
      glm::mat3(r1, g1, b1, r2, g2, b2, r3, g3, b3));
}

EMSCRIPTEN_KEEPALIVE
void setMumaxColorScheme() {
  viewer->fieldRenderer.setMumaxColorScheme();
}
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

  int width = canvas_get_width();
  int height = canvas_get_height();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);  // anti-aliasing
  glfwWindowHint(GLFW_RESIZABLE, true);

  window = glfwCreateWindow(width, height, "mumax-view", NULL, NULL);

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

  viewer = new Viewer(window);

  // -------- LOAD INITIAL FIELD --------------------------------------------

  viewer->fieldCollection.add(
      NamedField(testField(glm::ivec3(50, 50, 1)), "example"));

  //--------- MAIN LOOP ----------------------------------------------------

  emscripten_set_main_loop(main_loop, 0, true);

  //-------- CLEAN UP --------------------------------------------------------

  glfwTerminate();
  return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  viewer->scene.ensureRendering();
  glViewport(0, 0, width, height);
}

void keyCallBack(GLFWwindow* window,
                 int key,
                 int scancode,
                 int action,
                 int mods) {
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (mods == 0) {
      viewer->scene.camera.setYaw(glm::pi<float>() / 2);
    } else if (mods == 1) {
      viewer->scene.camera.setYaw(-glm::pi<float>() / 2);
    }
    viewer->scene.camera.setPitch(0);

  } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    if (mods == 0) {
      viewer->scene.camera.setYaw(0);
    } else if (mods == 1) {
      viewer->scene.camera.setYaw(glm::pi<float>());
    }
    viewer->scene.camera.setPitch(0);

  } else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    if (mods == 0) {
      viewer->scene.camera.setPitch(glm::pi<float>() / 2);
    } else if (mods == 1) {
      viewer->scene.camera.setPitch(-glm::pi<float>() / 2);
    }
    viewer->scene.camera.setYaw(0);

  } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    viewer->scene.camera.reset();

  } else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    viewer->timeIntervalTrigger.start();
  } else if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
    viewer->timeIntervalTrigger.stop();

  } else if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    viewer->setPlayDirection(PLAYBACKWARD);
    viewer->timeIntervalTrigger.start();
  } else if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
    viewer->timeIntervalTrigger.stop();
    viewer->setPlayDirection(PLAYFORWARD);  // set back default play direction

  } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (viewer->timeIntervalTrigger.isActive()) {
      viewer->timeIntervalTrigger.stop();
    } else {
      viewer->timeIntervalTrigger.start();
    }
  }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  float dist = viewer->scene.camera.distance();
  viewer->scene.camera.setDistance(
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

  Camera* camera = &viewer->scene.camera;

  if (mouse.leftButtonDown) {
    float sensitivity = 0.02;
    float newYaw = camera->yaw() + sensitivity * xoffset;
    float newPitch = camera->pitch() - sensitivity * yoffset;
    camera->setYaw(newYaw);
    camera->setPitch(newPitch);
  }
  // TODO: fix middle button feature
  if (mouse.middleButtonDown) {
    float sensitivity = 0.02;
    glm::vec3 target = camera->target();
    glm::vec3 position = camera->position();
    glm::vec3 vertical = camera->up();
    glm::vec3 horizontal =
        glm::normalize(glm::cross(target - position, vertical));
    target -= sensitivity * yoffset * vertical;
    target -= sensitivity * xoffset * horizontal;
    camera->setTarget(target);
  }
}

void errorCallback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
}