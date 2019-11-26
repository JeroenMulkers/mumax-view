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

#include "arrowmodel.hpp"
#include "camera.hpp"
#include "field.hpp"
#include "shaderprogram.hpp"
#include "shaders.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool needRender;

std::function<void()> updateFieldAttributes;
std::function<void()> resetCamera;

std::function<void()> loop;
void main_loop() {
  loop();
};

float arrowScalingsFactor = 1.0;
Arrow* arrow;
Field* field;
Camera camera;

extern "C" {

#ifdef __EMSCRIPTEN__
void EMSCRIPTEN_KEEPALIVE updateArrowShaftRadius(float r) {
  arrow->setShaftRadius(r);
  needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowHeadRadius(float r) {
  arrow->setHeadRadius(r);
  needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowHeadRatio(float r) {
  arrow->setHeadRatio(r);
  needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowSegments(float n) {
  arrow->setSegments(n);
  needRender = true;
}

void EMSCRIPTEN_KEEPALIVE updateArrowScalingsFactor(float s) {
  arrowScalingsFactor = s;
  needRender = true;
}

void EMSCRIPTEN_KEEPALIVE loadfile(std::string filename) {
  std::cout << filename << std::endl;
  Field* oldfield = field;
  field = readFieldFromOVF(filename);
  updateFieldAttributes();
  resetCamera();
  delete oldfield;
  needRender = true;
}

#endif
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  needRender = true;
  glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  needRender = true;
  camera.targetDistance *= (1 - 0.1 * (float)yoffset);
  std::cout << "scroll: " << yoffset << std::endl;
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
    needRender = true;
    float sensitivity = 0.02;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.yaw += xoffset;
    camera.pitch -= yoffset;
  }

  if (mouse.middleButtonDown) {
    needRender = true;
    // todo: move camera
    camera.target -= 0.05f * yoffset * camera.up();
    // camera.target[0] -= 0.05 * xoffset;
    // camera.target[2] -= 0.05 * yoffset;
  }
}

static void error_callback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
}

int main() {
  glm::vec3 backgroundColor = glm::vec3(0.3, 0.3, 0.3);
  glm::vec3 lightDirection = glm::vec3(1.0f, 0.5f, -1.0f);
  float ambientLightStrength = 0.1;
  float specularLightStrength = 0.5;
  float nearCut = 0.1;
  float fovy = glm::radians(45.0f);

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    std::cerr << "glfwInit failed" << std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);  // anti-aliasing

  GLFWwindow* window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "fieldview", NULL, NULL);

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

  glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 0.1f);
  glEnable(GL_DEPTH_TEST);
#ifndef __EMSCRIPTEN__
  glEnable(GL_MULTISAMPLE);  // Needed for anti-aliasing
#endif

  field = testField(glm::ivec3(50, 50, 2));

  arrow = new Arrow(0.12, 0.2, 0.6, 40);

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  ShaderProgram shader = ShaderProgram();
  shader.attachShader(vertexshader, GL_VERTEX_SHADER);
  shader.attachShader(fragmentshader, GL_FRAGMENT_SHADER);
  shader.link();
  shader.use();
  shader.setFloat("ArrowScalingsFactor", arrowScalingsFactor);
  shader.setVec3("lightDir", lightDirection);
  shader.setFloat("ambient", ambientLightStrength);
  shader.setFloat("specularStrength", specularLightStrength);

  int aInstancePosLoc = glGetAttribLocation(shader.ID, "aInstancePos");
  glEnableVertexAttribArray(aInstancePosLoc);
  glVertexAttribDivisor(aInstancePosLoc, 1);

  int aInstanceVecLoc = glGetAttribLocation(shader.ID, "aInstanceVector");
  glEnableVertexAttribArray(aInstanceVecLoc);
  glVertexAttribDivisor(aInstanceVecLoc, 1);

  resetCamera = [&] {
    needRender = true;
    camera.yaw = 0.0;
    camera.pitch = 3.1415 / 10.;
    camera.target = glm::vec3((field->gridsize.x - 1) / 2.0,
                              (field->gridsize.y - 1) / 2.0, 0.0f);
  };
  resetCamera();

  updateFieldAttributes = [&] {
    needRender = true;
    field->updateVBOs();

    glBindBuffer(GL_ARRAY_BUFFER, field->positionVBO);
    glVertexAttribPointer(aInstancePosLoc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, field->vectorsVBO);
    glVertexAttribPointer(aInstanceVecLoc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), (void*)0);
  };

  updateFieldAttributes();

  // Arrow model triangles
  int aPosLoc = glGetAttribLocation(shader.ID, "aPos");
  glEnableVertexAttribArray(aPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, arrow->VBO());
  glVertexAttribPointer(aPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)0);

  // Arrow model normals
  int aNormalLoc = glGetAttribLocation(shader.ID, "aNormal");
  glEnableVertexAttribArray(aNormalLoc);
  glBindBuffer(GL_ARRAY_BUFFER, arrow->VBO());
  glVertexAttribPointer(aNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)(sizeof(glm::vec3)));

  float previousTime = glfwGetTime();
  int fps = 0;

  loop = [&] {
    float time = glfwGetTime();
    fps++;

    if (time - previousTime > 1.0) {
      std::cout << "fps: " << fps << std::endl;
      std::cout << "ncells: " << field->ncells() << std::endl;
      fps = 0;
      previousTime = time;
    }

    if (needRender) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      int width, height;
      glfwGetWindowSize(window, &width, &height);
      float aspect = static_cast<float>(width) / static_cast<float>(height);
      glm::mat4 projection = glm::infinitePerspective(fovy, aspect, nearCut);

      shader.setFloat("ArrowScalingsFactor", arrowScalingsFactor);
      shader.setMat4("view", camera.viewMatrix());
      shader.setVec3("viewPos", camera.position());
      shader.setMat4("projection", projection);

      glDrawArraysInstanced(GL_TRIANGLES, 0, arrow->nVertices(),
                            field->ncells());
    }
    needRender = false;
  };

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, true);
#else
  while (!glfwWindowShouldClose(window)) {
    loop();
    glfwSwapBuffers(window);
    glfwPollEvents();  // alternative: glfwWaitEvents();
  }
#endif

  delete field;

  glDeleteBuffers(1, &VAO);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}