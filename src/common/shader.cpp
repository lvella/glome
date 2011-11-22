#include <iostream>

#include "shader.hpp"

using namespace std;

Shader::Shader():
		prog(0)
{}

Shader::Shader(unsigned char *vcode, GLint vlen, unsigned char *fcode, GLint flen)
{
	setup_shader(vcode, vlen, fcode, flen);
}

Shader::~Shader()
{
	glDeleteProgram(prog);
}

void
Shader::setup_shader(unsigned char *vcode, GLint vlen, unsigned char *fcode, GLint flen)
{
	GLuint vshader;
	GLuint fshader;

	const char *ptr;
	char err[10000];

	prog = glCreateProgram();

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

		glAttachShader(prog, vshader);
		glDeleteShader(vshader);
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

		glAttachShader(prog, fshader);
		glDeleteShader(fshader);
	}

	// We expect every shader to have a "position" attribute, to be the reference attribute
	glBindAttribLocation(prog, 0, "position");

	glLinkProgram(prog);

	uniform_transform = glGetUniformLocation(prog, "transform");

	attr_color = glGetAttribLocation(prog, "color");
	attr_texcoord = glGetAttribLocation(prog, "texcoord");
}
