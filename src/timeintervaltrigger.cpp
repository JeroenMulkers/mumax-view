#include <GLFW/glfw3.h>
#include <cmath>
#include <functional>

#include "timeintervaltrigger.hpp"

TimeIntervalTrigger::TimeIntervalTrigger(double timeInterval)
    : action_([] {}), active_(false), timeInterval_(timeInterval) {}

double TimeIntervalTrigger::timeInterval() const {
  return timeInterval_;
}

void TimeIntervalTrigger::setAction(std::function<void()> action) {
  action_ = action;
}

void TimeIntervalTrigger::setTimeInterval(double timeInterval) {
  timeInterval_ = timeInterval;
}

void TimeIntervalTrigger::start() {
  active_ = true;
  previousTime_ = glfwGetTime();
}

void TimeIntervalTrigger::stop() {
  active_ = false;
}

void TimeIntervalTrigger::operator()() {
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

bool TimeIntervalTrigger::isActive() const {
  return active_;
}
