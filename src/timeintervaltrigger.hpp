#pragma once

#include <functional>

class TimeIntervalTrigger {
 public:
  TimeIntervalTrigger(double timeInterval);

  void setAction(std::function<void()> action);
  void setTimeInterval(double timeInterval);
  void start();
  void stop();
  void operator()();

  double timeInterval() const;
  bool isActive() const;

 private:
  double previousTime_;
  double timeInterval_;
  bool active_;
  std::function<void()> action_;
};