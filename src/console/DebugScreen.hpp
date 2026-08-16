#ifndef CONSOLE_DEBUG_SCREEN_HPP
#define CONSOLE_DEBUG_SCREEN_HPP

#include "console/Types.hpp"

#define DEBUG_SCREEN_MAX_LINES 32
#define DEBUG_SCREEN_MAX_LABEL 32
#define DEBUG_SCREEN_MAX_TEXT 160

extern float s_debugScreenFontHeight;
extern float s_debugScreenLeft;
extern float s_debugScreenTop;

void DebugScreenInitialize();

void DebugScreenSet(const char* label, const char* format, ...);

void DebugScreenSetColored(COLOR_T colorType, const char* label, const char* format, ...);

void DebugScreenRemove(const char* label);

void DebugScreenClear();

#endif
