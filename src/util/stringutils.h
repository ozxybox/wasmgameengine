#pragma once

#include "linalg.h"

// Retuns a buffer of the given size. May or may not allocate new memory. 
// NEVER USE THIS POINTER FOR RESULTS!!
char* workingBuffer(int size);
// NEVER USE THIS POINTER FOR RESULTS!!


int IsWhitespace(char c);
int IsEndLine(char c);
char* StringToVec(char* c, vec3* out);