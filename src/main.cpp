#include <functional>

#include <emscripten/bind.h>
#include <emscripten/emscripten.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ovf.hpp"
#include "viewer.hpp"

std::string getFieldName() {
  Viewer* viewer = Viewer::get();
  return viewer->fieldCollection.selectedFieldName();
}

void loadConfigFile(std::string filename) {
  Viewer* viewer = Viewer::get();
  viewer->fieldCollection.load(filename);
}

EMSCRIPTEN_BINDINGS(myModule) {
  emscripten::function("getFieldName", &getFieldName);
  emscripten::function("loadConfigFile", &loadConfigFile);
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void setScrollSensitivity(double sensitivity) {
  Viewer* viewer = Viewer::get();
  viewer->mouse.scrollSensitivity = sensitivity;
}

EMSCRIPTEN_KEEPALIVE
void useArrowGlyph() {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setGlyphType(ARROW);
}

EMSCRIPTEN_KEEPALIVE
void useCuboidGlyph() {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setGlyphType(CUBOID);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowShaftRadius(float r) {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.arrow.setShaftRadius(r);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRadius(float r) {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.arrow.setHeadRadius(r);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowHeadRatio(float r) {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.arrow.setHeadRatio(r);
}

EMSCRIPTEN_KEEPALIVE
void updateArrowScalingsFactor(float s) {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setArrowScalingsFactor(s);
}

EMSCRIPTEN_KEEPALIVE
void updateCuboidScalingsFactor(float s) {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setCuboidScalingsFactor(s);
}

EMSCRIPTEN_KEEPALIVE
void updateRelativeRange(float xmin,
                         float ymin,
                         float zmin,
                         float xmax,
                         float ymax,
                         float zmax) {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setRelativeRange({xmin, ymin, zmin},
                                         {xmax, ymax, zmax});
}

EMSCRIPTEN_KEEPALIVE
void emptyFieldCollection() {
  Viewer* viewer = Viewer::get();
  return viewer->fieldCollection.emptyCollection();
}

EMSCRIPTEN_KEEPALIVE
int fieldCollectionSize() {
  Viewer* viewer = Viewer::get();
  return viewer->fieldCollection.size();
}

EMSCRIPTEN_KEEPALIVE
int fieldCollectionSelected() {
  Viewer* viewer = Viewer::get();
  return viewer->fieldCollection.selectedFieldIdx();
}

EMSCRIPTEN_KEEPALIVE
void fieldCollectionSelect(int idx) {
  Viewer* viewer = Viewer::get();
  viewer->fieldCollection.select(idx);
}

EMSCRIPTEN_KEEPALIVE
void setTimeInterval(double timeInterval) {
  Viewer* viewer = Viewer::get();
  viewer->timeIntervalTrigger.setTimeInterval(timeInterval);
}

EMSCRIPTEN_KEEPALIVE
void startTimeIntervalTrigger() {
  Viewer* viewer = Viewer::get();
  viewer->timeIntervalTrigger.start();
}

EMSCRIPTEN_KEEPALIVE
void stopTimeIntervalTrigger() {
  Viewer* viewer = Viewer::get();
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
  int width = canvas_get_width();
  int height = canvas_get_height();
  Viewer* viewer = Viewer::get();
  viewer->setWindowSize(width, height);
}

EMSCRIPTEN_KEEPALIVE
void setBackgroundColor(float r, float g, float b) {
  Viewer* viewer = Viewer::get();
  viewer->scene.setBackgroundColor(r, g, b);
}

EMSCRIPTEN_KEEPALIVE
void setOutlineVisibility(bool visible) {
  Viewer* viewer = Viewer::get();
  viewer->fieldBoxRenderer.setVisibility(visible);
}

EMSCRIPTEN_KEEPALIVE
void setOutlineColor(float r, float g, float b) {
  Viewer* viewer = Viewer::get();
  viewer->fieldBoxRenderer.setColor(r, g, b);
}

EMSCRIPTEN_KEEPALIVE
void setAmbientLighting(float intensity) {
  Viewer* viewer = Viewer::get();
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
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setGradientColorScheme(
      glm::mat3(r1, g1, b1, r2, g2, b2, r3, g3, b3));
}

EMSCRIPTEN_KEEPALIVE
void setMumaxColorScheme() {
  Viewer* viewer = Viewer::get();
  viewer->fieldRenderer.setMumaxColorScheme();
}
}

int main(int argc, char** argv) {
  Viewer* viewer = Viewer::get();

  viewer->updateFieldCallback = []() { EM_ASM({ updateFieldSelector(); }); };
  viewer->updateFieldCallback();

  viewer->fieldCollection.add(
      NamedField(testField(glm::ivec3(50, 50, 1)), "example"));

  updateCanvasSize();

  emscripten_set_main_loop([]() { Viewer::get()->loop(); }, 0, true);

  return 0;
}
