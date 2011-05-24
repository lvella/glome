#pragma once

#include <GL/glew.h>

GLuint setup_vshader(unsigned char *vcode, GLint len);
GLuint setup_vfshader(unsigned char *vcode, unsigned char *fcode, GLint vlen, GLint flen);
