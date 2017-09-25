#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

class TimerManager {
public:
  TimerManager();
  int registerTimer(uint64_t delay);
  int isTimerPassed(uint64_t token);
  void loop();
  void resetTimer(uint64_t token);
};
#endif
