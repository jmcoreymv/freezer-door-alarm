#include <Arduino.h>
#include "config.h"
#include "log.h"


RTC_DATA_ATTR int bootCount = 0;


void logSetup() {
    #ifdef DEBUG_MODE
        bootCount++;
        Serial.begin(CONSOLE_BAUD_RATE);
        delay(1000); //Take some time to open up the Serial Monitor 
        log(("Boot number: " + String(bootCount)));  
    #endif
}

void log(String logMsg) {
    #ifdef DEBUG_MODE
        Serial.println(logMsg.c_str());
    #endif
}

void logFlush() {
    #ifdef DEBUG_MODE
        delay(1000);
        Serial.flush(); 
    #endif
}