#include "4dmath.hpp"
#include "projectile.hpp"
#include "minimap.hpp"
#include "meridian.hpp"
#include "shader.hpp"

#include "world.hpp"

static GLuint program;

void World::initialize()
{
#include "world_proj.glsl.hpp"
  program = setup_vshader(world_proj_glsl, world_proj_glsl_len);
}

World::World():
  kbi(&ship),
  cam_hist(10, Matrix4::IDENTITY)
{}

bool World::update()
{
  bool run;
  // Treat events
  run = kbi.handle();

  cube.update();
  Projectile::update_all();
  ship.update();

  return run;
}

void World::draw()
{
  const Matrix4 offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));
  Matrix4 center = ship.transformation().transpose();

  // Camera transform
  (offset * cam_hist.front()).loadToGL();
  cam_hist.pop_front();
  cam_hist.push_back(center);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(program);
  draw_meridians();

  cube.draw();
  Projectile::draw_all();
  glUseProgram(program);
  ship.draw();
  MiniMap::draw(*this, center);
}

void World::fill_minimap() {
  cube.draw_in_minimap();
}
