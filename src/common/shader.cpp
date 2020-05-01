#include "shader.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "config.hpp"

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};
static std::map<const char*, GLuint, ltstr> loaded_shaders;

Shader::Shader(const SourceVector& sources)
{
	setup_shader(sources);
}

void Shader::setup_shader(const SourceVector& sources)
{
	int ret;

	GLint len;
	GLint type;
	GLuint shader;

	prog = glCreateProgram();

	for(const char* name: sources) {
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

			std::string path(DATA_DIR);
			path += "/shaders/";
			path += name;

			FILE* fl = fopen(path.c_str(), "r");
			if(!fl)
			{
				std::cout <<"Could not open file \""<<name<<"\": "<<strerror(errno)<<std::endl;
				break;
			}
			fseek(fl, 0, SEEK_END);
			len = ftell(fl);
			fseek(fl, 0, SEEK_SET);
			std::vector<char> buff(len);

			ret = fread(buff.data(), 1, len, fl);
			if(ret != len)
			{
				std::cout<<"Could not read whole file: "<<strerror(errno)<< std::endl;
			}
			fclose(fl);
			shader = glCreateShader(type);
			{
				char *ptr = buff.data();
				glShaderSource(shader, 1, (const GLchar**)&ptr, &len);
			}
			glCompileShader(shader);
			{
				GLsizei length;
				char err[10000];
				glGetShaderInfoLog(shader, 10000, &length, err);
				if(length)
					std::cout << "Shader "<<name<<" compilation log:\n" << err << std::endl;
			}
			loaded_shaders.insert(std::pair<const char*, GLuint>(name, shader));
		}
		else
		{
			shader = loaded_shaders[name];
		}

		glAttachShader(prog, shader);
	}
	// We expect every shader to have a "position" attribute, to be the reference attribute
	glBindAttribLocation(prog, 0, "position");
	glLinkProgram(prog);
	{
		GLsizei length;
		char err[10000];
		glGetProgramInfoLog(prog, 10000, &length, err);
		if(length) {
			std::cout << "Linkage log of [";
			const char* sep = "";
			for(auto name: sources) {
				std::cout << sep << name;
				sep = ", ";
			}
			std::cout << "]:\n" << err << '\n';
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

void SpaceShader::setup_shader(const SourceVector& sources)
{
	Shader::setup_shader(sources);
	transform = getUniform("transformation");
}

void CamShader::initialize(float aspect_ratio)
{
	proj_mat = perspective(FOV_Y, aspect_ratio, Z_NEAR, Z_FAR);
	initialized = true;
}

void CamShader::setup_shader(const SourceVector& sources)
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
