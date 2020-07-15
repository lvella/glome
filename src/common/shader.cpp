#include "shader.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>

#include "data_file.hpp"

using namespace std::string_literals;

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

			auto fl = load_data_file("shaders/"s + name);
			fl.seekg(0, fl.end);
			len = fl.tellg();
			fl.seekg(0, fl.beg);

			std::vector<char> buff(len);
			if(!fl.read(buff.data(), buff.size()))
			{
				std::cout<<"Could not read whole file: "<<strerror(errno)<< std::endl;
			}
			fl.close();

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

	for(unsigned i = 0; i < NUM_ATTRIBUTES; ++i) {
		glBindAttribLocation(prog, i, attr_names[i]);
	}

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
			std::cout << "]:\n" << err << std::endl;
		}
	}
}

void Shader::enable() const
{
	glUseProgram(prog);
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
