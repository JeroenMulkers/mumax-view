#include <GLFW/glfw3.h>

#include <emscripten/emscripten.h>

#include "fieldcollection.hpp"
#include "viewer.hpp"

Viewer::Viewer()
    : timeIntervalTrigger(0.1), fieldCollection(this) {
  fieldRenderer.putOnScene(&scene);
  fieldBoxRenderer.putOnScene(&scene);
  setPlayDirection(PLAYFORWARD);
}

void Viewer::loop() {
  timeIntervalTrigger();
  if (scene.needRendering())
    scene.render();
  swapWindowBuffers();
  pollEvents();
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

void Viewer::frameBufferSizeCallback(int width, int height) {
  scene.ensureRendering();
  glViewport(0, 0, width, height);
}

void Viewer::keyCallBack(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (mods == 0) {
      scene.camera.setYaw(glm::pi<float>() / 2);
    } else if (mods == 1) {
      scene.camera.setYaw(-glm::pi<float>() / 2);
    }
    scene.camera.setPitch(0);

  } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    if (mods == 0) {
      scene.camera.setYaw(0);
    } else if (mods == 1) {
      scene.camera.setYaw(glm::pi<float>());
    }
    scene.camera.setPitch(0);

  } else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    if (mods == 0) {
      scene.camera.setPitch(glm::pi<float>() / 2);
    } else if (mods == 1) {
      scene.camera.setPitch(-glm::pi<float>() / 2);
    }
    scene.camera.setYaw(0);

  } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    scene.camera.reset();

  } else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    timeIntervalTrigger.start();
  } else if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
    timeIntervalTrigger.stop();

  } else if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    setPlayDirection(PLAYBACKWARD);
    timeIntervalTrigger.start();
  } else if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
    timeIntervalTrigger.stop();
    setPlayDirection(PLAYFORWARD);  // set back default play direction

  } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (timeIntervalTrigger.isActive()) {
      timeIntervalTrigger.stop();
    } else {
      timeIntervalTrigger.start();
    }
  }
}

void Viewer::scrollCallback(double xoffset, double yoffset) {
  float dist = scene.camera.distance();
  scene.camera.setDistance(
      dist * (1.0 - static_cast<float>(mouse.scrollSensitivity * yoffset)));
}

void Viewer::mouseButtonCallback(int button, int action, int mods) {
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

void Viewer::curserPosCallback(double xpos, double ypos) {

  float xoffset = xpos - mouse.lastX;
  float yoffset = mouse.lastY - ypos;
  mouse.lastX = xpos;
  mouse.lastY = ypos;

  Camera* camera = &scene.camera;

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