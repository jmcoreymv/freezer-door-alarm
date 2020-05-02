#include <Arduino.h>

#ifndef LOG_H
#define LOG_H

void logSetup();
void log(String logMsg);
void logFlush();

#endif /* LOG_H */