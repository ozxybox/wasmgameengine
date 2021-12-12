#include "log.h"
#include "stringutils.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <emscripten.h>

static char* tvprintf(const char* format, va_list args)
{
    // Get the size without printing
    int size = vsnprintf(0, 0, format, args);
    if(size < 0) return 0;
    size++; // Null terminator

    // Get space for the print and write to it
    char* buf = workingBuffer(size);
    vsnprintf(buf, size, format, args);
    return buf;
}
static char* tprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* msg = tvprintf(format, args);
    va_end(args);
    return msg;
}

// Front facing user functions of the logger
void logMsg(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* msg = tvprintf(format, args);
    va_end(args);

    emscripten_log(EM_LOG_CONSOLE, msg);
}
void logInfo(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* msg = tvprintf(format, args);
    va_end(args);

    emscripten_log(EM_LOG_CONSOLE | EM_LOG_INFO, msg);
}
void logWarn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* msg = tvprintf(format, args);
    va_end(args);

    emscripten_log(EM_LOG_CONSOLE | EM_LOG_WARN, msg);
}
void logError(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* msg = tvprintf(format, args);
    va_end(args);

    emscripten_log(EM_LOG_CONSOLE | EM_LOG_ERROR, msg);
}
void logDebug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* msg = tvprintf(format, args);
    va_end(args);

    emscripten_log(EM_LOG_CONSOLE | EM_LOG_DEBUG, msg);
}
void _logAssert(const char* condition)
{
    char* msg = tprintf(condition);
    
    emscripten_log(EM_LOG_CONSOLE | EM_LOG_DEBUG, msg);
    emscripten_debugger();
}