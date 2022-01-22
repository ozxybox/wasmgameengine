#include "linalg.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// Pointer versions of all the normal vector functions
// Generates *much* less asm on debug and release
// On release, most stuff gets inlined away, but on debug, this should make things a lot faster
// Main downside of these is we can't nest them like we can with the normal functions
// So, second file!
//
// "o" arguments are the vector being worked on
// "b" is only ever read 
//


// Vector 2 //
void addv2_p(vec2* o, vec2* b);
void subv2_p(vec2* o, vec2* b);
void addfv2_p(vec2* o, float b);
void subfv2_p(vec2* o, float b);
void scalev2_p(vec2* o, float b);
void normalizev2_p(vec2* o);
float dotv2_p(vec2* o, vec2* b);
float magv2_p(vec2* o);

// Vector 3 //
void crossv3_p(vec3* o, vec3* b);
void addv3_p(vec3* o, vec3* b);
void subv3_p(vec3* o, vec3* b);
void addfv3_p(vec3* o, float b);
void subfv3_p(vec3* o, float b);
void scalev3_p(vec3* o, float b);
void normalizev3_p(vec3* o);
float dotv3_p(vec3* o, vec3* b);
float magv3_p(vec3* o);

// Vector 4 //
void addv4_p(vec4* o, vec4* b);
void subv4_p(vec4* o, vec4* b);
void addfv4_p(vec4* o, float b);
void subfv4_p(vec4* o, float b);
void scalev4_p(vec4* o, float b);
void normalizev4_p(vec4* o);
float dotv4_p(vec4* o, vec4* b);
float magv4_p(vec4* o);
