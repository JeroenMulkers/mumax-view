#include "application.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

Application::Application() {

  glfwSetErrorCallback([](int error,  const char* description){
  std::cerr << "Error: " << description << std::endl;
  });

  if (!glfwInit())
    std::runtime_error("glfwInit failed");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);  // anti-aliasing
  glfwWindowHint(GLFW_RESIZABLE, true);

  window = glfwCreateWindow(1, 1, "", NULL, NULL);

  glfwMakeContextCurrent(window);

  if (!window)
    std::runtime_error("Can not create window");

  glfwSetWindowUserPointer(window, this);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow* window, int width, int height) {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->frameBufferSizeCallback(width, height);
      });

  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->keyCallBack(key, scancode, action, mods);
  });

  glfwSetScrollCallback(
      window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->scrollCallback(xoffset, yoffset);
      });

  glfwSetMouseButtonCallback(
      window, [](GLFWwindow* window, int button, int action, int mods) {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->mouseButtonCallback(button, action, mods);
      });

  glfwSetCursorPosCallback(
      window, [](GLFWwindow* window, double xpos, double ypos) {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->curserPosCallback(xpos, ypos);
      });

  glEnable(GL_DEPTH_TEST);
}

Application::~Application() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Application::setWindowSize(int width, int height) {
  glfwSetWindowSize(window, width, height);
  glViewport(0, 0, width, height);
}

void Application::swapWindowBuffers() {
    glfwSwapBuffers(window);
}

void Application::pollEvents() {
    glfwPollEvents();
}
