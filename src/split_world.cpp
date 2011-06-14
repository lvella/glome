#include <iostream>

#include "4dmath.hpp"
#include "projectile.hpp"
#include "minimap.hpp"
#include "meridian.hpp"
#include "shader.hpp"
#include "input.hpp"
#include "kbinput.hpp"
#include "jsinput.hpp"

#include "split_world.hpp"

static GLuint program;

void SplitWorld::initialize()
{
#include "world_proj.glsl.hpp"
  program = setup_vshader(world_proj_glsl, world_proj_glsl_len);
}

SplitWorld::SplitWorld()
{
  points[0] = points[1] = 0;

  ship[1].setTransformation(yw_matrix(M_PI));

  Input::Kb::set_ship(&ship[0]);
  Input::Js::set_ship(&ship[1]);

  cam_hist[0].resize(10, Matrix4::IDENTITY);
  cam_hist[1].resize(10, Matrix4::IDENTITY);
}

bool SplitWorld::update()
{
  bool run;
  // Treat events
  run = Input::handle();

  Projectile::update_all();

  Vector4 c = cube.transformation().position();

  for(int i = 0; i < 2; ++i) {
    ship[i].update();
    if(Projectile::collide(&ship[i]))
      ship[i].setTransformation(cube.transformation() * yw_matrix(M_PI));

    if((c - ship[i].transformation().position()).squared_length() < (0.03f * 0.03f)) {
      cube.randomize();
      std::cout << "Ship " << i << " scored " << ++points[i] << " points!" << std::endl;
    }
  }

  return run;
}

void SplitWorld::draw()
{
  extern const int WIDTH, HEIGHT;
  const Matrix4 offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(iter = 0; iter < 2; ++iter)
    {
      int wstart = iter * WIDTH/2;
      glViewport(wstart, 0, WIDTH/2, HEIGHT);
      Matrix4 center = ship[iter].transformation().transpose();

      // Camera transform
      (offset * cam_hist[iter].front()).loadToGL();
      cam_hist[iter].pop_front();
      cam_hist[iter].push_back(center);

      glUseProgram(program);
      draw_meridians();

      cube.draw();
      Projectile::draw_all();
      glUseProgram(program);
      ship[0].draw();
      ship[1].draw();
      MiniMap::draw(wstart, this, center);
    }
}

void SplitWorld::fill_minimap() {
  cube.draw_in_minimap();
  ship[(iter+1) & 1].draw_in_minimap();
}
