// For some weird reason, the implementation of glText is inside the header,
// and is expanded when GLT_IMPLEMENTATION is defined.
//
// So, this is the compilation unit where glText is compiled.

#include "gl.hpp"

#define GLT_IMPORTS
#define GLT_IMPLEMENTATION
#include <gltext.h>
