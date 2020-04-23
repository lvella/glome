#include "shader.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "config.hpp"

using namespace std;

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};
static std::map<const char*, GLuint, ltstr> loaded_shaders;

Shader::Shader(const char *sources[])
{
	setup_shader(sources);
}

void Shader::setup_shader(const char *sources[])
{
	char *ptr;
	const char **iter;
	const char *name;
	int ret;

	FILE *fl;
	GLint len;
	GLint type;
	GLuint shader;

	assert(sources);
	assert(*sources);
	prog = glCreateProgram();

	iter = sources;
	while(*iter != NULL)
	{
		name = *iter;
		if(loaded_shaders.find(name) == loaded_shaders.end())
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
			path += "/shaders/";
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
				char err[10000];
				glGetShaderInfoLog(shader, 10000, &length, err);
				if(length)
					cout << "Shader "<<name<<" compilation log:\n" << err << endl;
			}
			delete [] ptr;
			loaded_shaders.insert(std::pair<const char*, GLuint>(name, shader));
		}
		else
		{
			shader = loaded_shaders[name];
		}

		glAttachShader(prog, shader);
		++iter;

	}
	// We expect every shader to have a "position" attribute, to be the reference attribute
	glBindAttribLocation(prog, 0, "position");
	glLinkProgram(prog);
	{
		GLsizei length;
		char err[10000];
		glGetProgramInfoLog(prog, 10000, &length, err);
		if(length) {
			cout << "Linkage log of [" << *sources;
			for(iter = sources + 1; *iter; ++iter)
				cout << ", " << *iter;
			cout << "]:\n" << err << '\n';
		}
	}

	attr_color = glGetAttribLocation(prog, "color");
	attr_texcoord = glGetAttribLocation(prog, "texcoord");
}

Shader::~Shader()
{
	glDeleteProgram(prog);
}

Uniform Shader::getUniform(const char *name) const {
	GLint ret = glGetUniformLocation(prog, name);
	assert(ret != -1);
	if(ret == -1) {
		std::cerr << "Could not retrieve \"" << name
			<< "\" uniform: " << glGetError() << std::endl;
	}
	return Uniform{ret};
}

void SpaceShader::setup_shader(const char *sources[])
{
	Shader::setup_shader(sources);
	transform = getUniform("transform");
}

void CamShader::initialize(float aspect_ratio)
{
	proj_mat = perspective(FOV_Y, aspect_ratio, Z_NEAR, Z_FAR);
	initialized = true;
}

void CamShader::setup_shader(const char *sources[])
{
	SpaceShader::setup_shader(sources);

	assert(initialized);

	// Must be enabled in order to set values.
	enable();

	// Set immutable uniforms.
	getUniform("projection").set(proj_mat);
	getUniform("zFar").set(Z_FAR);
}

Matrix4 CamShader::proj_mat;
bool CamShader::initialized = false;
