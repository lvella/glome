#pragma once

#include <iostream>

#include "gl.hpp"

void glClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool glLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") " 
			<< gluErrorString(error) << " : "
			<< function << " " << file << ":" << line
			<< std::endl;
		return false;
	}
	return true;
}

#define glDebug(x)  \
	glClearError(); \
	x;              \
	glLogCall(#x, __FILE__, __LINE__);
