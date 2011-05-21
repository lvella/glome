#include <iostream>

#include "shader.hpp"

using namespace std;

GLuint setup_vshader(unsigned char *vcode, GLint len)
{
  GLuint program;
  GLuint shader;

  const char *ptr;
  char err[10000];

  shader = glCreateShader(GL_VERTEX_SHADER);
  ptr = (const char *)vcode;
  glShaderSource(shader, 1, &ptr, &len);
  glCompileShader(shader);

  {
    GLsizei length;
    glGetShaderInfoLog(shader, 10000, &length, err);
    if(length)
      cout << "Shader compilation log:\n" << err << '\n';
  }

  program = glCreateProgram();
  glAttachShader(program, shader);

  glLinkProgram(program);
  return program;
}
