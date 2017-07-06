//timing.h
#ifndef TIMING_H
#define TIMING_H

extern const double physicsRate;
extern const double oobillion;
extern struct timespec timeStart, timeCurrent;
extern struct timespec timePause;
extern double physicsCountdown;
extern double timeSpan;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

#endif
