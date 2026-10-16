#pragma once  //PREVENT DOUBLE IMPORTS

#ifndef LOGGER_H
#define LOGGER_H

void logInfo(const char* module, const char* event);
void logInfo(const char* module, const char* event, int value);
void logInfo(const char* module, const char* event, float value);

void logWarning(const char* module, const char* event);
void logError(const char* module, const char* event);

#endif