// For some weird reason, the implementation of glText is inside the header,
// and is expanded when GLT_IMPLEMENTATION is defined.
//
// So, this is the compilation unit where glText is compiled.
// Do not define GLT_IMPLEMENTATION when using gltext.h from other files.

#include "gl.hpp"

#define GLT_IMPLEMENTATION
#include <gltext.h>
