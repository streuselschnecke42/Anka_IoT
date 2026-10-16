#include "Arduino.h"
#include "logger.h"

void logInfo(const char* module, const char* event) {
    Serial.print("INFO:");
    Serial.print(module);
    Serial.print(":");
    Serial.println(event);
}

void logInfo(const char* module, const char* event, int value) {
    Serial.print("INFO:");
    Serial.print(module);
    Serial.print(":");
    Serial.print(event);
    Serial.print(":");
    Serial.println(value);
}

void logInfo(const char* module, const char* event, float value) {
    Serial.print("INFO:");
    Serial.print(module);
    Serial.print(":");
    Serial.print(event);
    Serial.print(":");
    Serial.println(value);
}

void logWarning(const char* module, const char* event) {
    Serial.print("WARNING:");
    Serial.print(module);
    Serial.print(":");
    Serial.println(event);
}

void logError(const char* module, const char* event) {
    Serial.print("ERROR:");
    Serial.print(module);
    Serial.print(":");
    Serial.println(event);
}