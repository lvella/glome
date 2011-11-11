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

#include <iostream>

#include "shader.hpp"

using namespace std;

GLuint setup_vfshader(unsigned char *vcode, unsigned char *fcode, GLint vlen, GLint flen)
{
  GLuint program;
  GLuint vshader;
  GLuint fshader;

  const char *ptr;
  char err[10000];

  vshader = glCreateShader(GL_VERTEX_SHADER);
  fshader = glCreateShader(GL_FRAGMENT_SHADER);

  ptr = (const char *)vcode;
  glShaderSource(vshader, 1, &ptr, &vlen);
  glCompileShader(vshader);

  {
    GLsizei length;
    glGetShaderInfoLog(vshader, 10000, &length, err);
    if(length)
      cout << "Shader compilation log:\n" << err << '\n';
  }

  ptr = (const char *)fcode;
  glShaderSource(fshader, 1, &ptr, &flen);
  glCompileShader(fshader);

  {
    GLsizei length;
    glGetShaderInfoLog(fshader, 10000, &length, err);
    if(length)
      cout << "Shader compilation log:\n" << err << '\n';
  }

  program = glCreateProgram();
  glAttachShader(program, vshader);
  glAttachShader(program, fshader);

  glLinkProgram(program);
  return program;
}
