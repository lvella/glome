#include <iostream>

#include "shader.hpp"
#include <string.h>
#include <string>
#include <stdio.h>
#include <errno.h>
#include "config.hpp"

using namespace std;

static std::map<const char*, GLuint> load_shaders;

Shader::Shader():
	prog(0)
{}

Shader::Shader(const char *sources[])
{
	setup_shader(sources);
}

Shader::~Shader()
{
	glDeleteProgram(prog);
}

void Shader::setup_shader(const char *sources[])
{

	char *ptr;
	char err[10000];
	const char *name;
	int ret;

	FILE *fl;
	GLint len;
	GLint type;
	GLuint shader;

	assert(sources);
	assert(*sources);
	prog = glCreateProgram();

	while(*sources != NULL)
	{
		name = *sources;
		if(load_shaders.find(name) == load_shaders.end())
		{
			if(strrchr(name, '.')[1] == 'v')
			{
				type = GL_VERTEX_SHADER;
			}
			else
			{
				type = GL_FRAGMENT_SHADER;
			}

			string path(DATA_DIR);
			path += '/';
			path += name;

			fl = fopen(path.c_str(), "r");
			if(!fl)
			{
				cout <<"Could not open file \""<<name<<"\": "<<strerror(errno)<<endl;
				break;
			}
			fseek(fl, 0, SEEK_END);
			len = ftell(fl);
			fseek(fl, 0, SEEK_SET);
			ptr = new char [len];

			ret = fread(ptr, 1, len, fl);
			if(ret != len)
			{
				cout<<"Could not read whole file: "<<strerror(errno)<< endl;
			}
			fclose(fl);
			shader = glCreateShader(type);
			glShaderSource(shader, 1, (const GLchar**)&ptr, &len);
			glCompileShader(shader);
			{
				GLsizei length;
				glGetShaderInfoLog(shader, 10000, &length, err);
				if(length)
					cout << "Shader "<<name<<" compilation log:\n" << err << endl;
			}
			delete [] ptr;
			load_shaders.insert(std::pair<const char*, GLuint>(name, shader));
		}
		else
		{
			shader = load_shaders[name];
		}

		glAttachShader(prog, shader);
		++sources;

	}
	// We expect every shader to have a "position" attribute, to be the reference attribute
	glBindAttribLocation(prog, 0, "position");
	glLinkProgram(prog);
	uniform_transform = glGetUniformLocation(prog, "transform");
	attr_color = glGetAttribLocation(prog, "color");
	attr_texcoord = glGetAttribLocation(prog, "texcoord");
}
