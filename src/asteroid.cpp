#include <iostream>
#include "4dmath.hpp"
#include "shader.hpp"
#include "asteroid.hpp"

using namespace std;

static GLuint program;

Asteroid::Asteroid():
    Object(Matrix4::IDENTITY)
{
  mesh = Mesh::get_mesh(ASTEROID);

  #include "asteroid-vert.glsl.hpp"
  #include "asteroid-frag.glsl.hpp"
  program = setup_vfshader(asteroid_vert_glsl, asteroid_frag_glsl, asteroid_vert_glsl_len, asteroid_frag_glsl_len);
}

Asteroid::~Asteroid()
{
  Mesh::release_mesh(mesh);
}

void Asteroid::draw()
{
  glUseProgram(program);
  mesh->draw(t);
  glColor3ub(255, 255, 255);
}

void Asteroid::update()
{
}

