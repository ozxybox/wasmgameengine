#include "stringutils.h"
#include <stdlib.h>

// We can't be having two threads writing over the same space!
static /*_Thread_local*/ char* buf = 0;

// Never use for returns!
char* workingBuffer(int size)
{
    static int bufSize = 0;
    if(bufSize < size)
    {
        // Free the old buf and allocate a larger one
        if(buf)
            free(buf);
        bufSize = size;
        buf = malloc(bufSize);
    }
    return buf;
} 




int IsWhitespace(char c)
{
	return c < '!' || c > '~';
}

int IsEndLine(char c)
{
	return c == '\n' || c == '\r';
}

char* StringToVec(char* c, vec3* out)
{
	out->x = strtod(c, &c);
	out->y = strtod(c, &c);
	out->z = strtod(c, &c);
	
	return c;
}