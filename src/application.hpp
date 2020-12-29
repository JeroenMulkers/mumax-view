#pragma once

#include <GLFW/glfw3.h>

class Mouse {
 public:
  Mouse() : scrollSensitivity(0.1){};
  bool leftButtonDown;
  bool middleButtonDown;
  double lastX;
  double lastY;
  double scrollSensitivity;
};

class Application {
 public:
  Application();
  ~Application();

  Application(const Application&) = delete;
  void operator=(const Application&) = delete;

  void setWindowSize(int width, int height);

  void swapWindowBuffers();
  void pollEvents();

  virtual void frameBufferSizeCallback(int width, int height) {}
  virtual void keyCallBack(int key, int scancode, int action, int mods) {}
  virtual void scrollCallback(double xoffset, double yoffset) {}
  virtual void mouseButtonCallback(int button, int action, int mods) {}
  virtual void curserPosCallback(double xpos, double ypos) {}

  virtual void loop() = 0;

  GLFWwindow* window;
  Mouse mouse;
};