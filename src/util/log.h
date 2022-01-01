#pragma once

void logMsg(const char* format, ...);
void logInfo(const char* format, ...);
void logWarn(const char* format, ...);
void logError(const char* format, ...);
void logDebug(const char* format, ...);

#define _LOG_STRINGIFY2(a) #a
#define _LOG_STRINGIFY(a) _LOG_STRINGIFY2(a)
#define _LOG_LINE_INFO __FILE__ " Line: " _LOG_STRINGIFY(__LINE__) ")  "
#define logMsgL(...) logMsg(_LOG_LINE_INFO __VA_ARGS__)
#define logInfoL(...) logInfo(_LOG_LINE_INFO __VA_ARGS__)
#define logWarnL(...) logWarn(_LOG_LINE_INFO __VA_ARGS__)
#define logErrorL(...) logError(_LOG_LINE_INFO __VA_ARGS__)
#define logDebugL(...) logDebug(_LOG_LINE_INFO __VA_ARGS__)

// debug break on call
void _logAssert(const char* condition);
#if 1 //def _DEBUG 
#define logAssert(condition) { if(condition) _logAssert(_LOG_LINE_INFO #condition); }
#else
#define logAssert(condition) 
#endif