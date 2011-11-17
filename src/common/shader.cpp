#include <iostream>

#include "shader.hpp"

using namespace std;

GLuint setup_shader(unsigned char *vcode, GLint vlen, unsigned char *fcode, GLint flen)
{
	GLuint program;
	GLuint vshader;
	GLuint fshader;

	const char *ptr;
	char err[10000];

	program = glCreateProgram();

	if(vcode) {
		vshader = glCreateShader(GL_VERTEX_SHADER);

		ptr = (const char *)vcode;
		glShaderSource(vshader, 1, &ptr, &vlen);
		glCompileShader(vshader);

		{
			GLsizei length;
			glGetShaderInfoLog(vshader, 10000, &length, err);
			if(length)
				cout << "Vertex shader compilation log:\n" << err << '\n';
		}

		glAttachShader(program, vshader);
	}

	if(fcode) {
		fshader = glCreateShader(GL_FRAGMENT_SHADER);

		ptr = (const char *)fcode;
		glShaderSource(fshader, 1, &ptr, &flen);
		glCompileShader(fshader);

		{
			GLsizei length;
			glGetShaderInfoLog(fshader, 10000, &length, err);
			if(length)
				cout << "Fragment shader compilation log:\n" << err << '\n';
		}

		glAttachShader(program, fshader);
	}

	// We expect every shader to have a "position" attribute, to be the reference attribute
	glBindAttribLocation(program, 0, "position");

	glLinkProgram(program);
	return program;
}
