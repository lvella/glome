#include <iostream>

#include "4dmath.hpp"
#include "projectile.hpp"
#include "minimap.hpp"
#include "meridian.hpp"
#include "shader.hpp"
#include "options.hpp"
#include "../sdl/input.hpp"
#include "../sdl/kbinput.hpp"
#include "../sdl/jsinput.hpp"
#include "ship.hpp"
#include "mesh.hpp"
#include "split_world.hpp"

using namespace Options;

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

void SplitWorld::update()
{
	// Treat events
	Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	
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
}

void SplitWorld::draw()
{
  const Matrix4 offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(iter = 0; iter < 2; ++iter)
    {
      int wstart = iter * width/2;

      {
        glViewport(wstart, 0, width/2, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(FOV, 0.5*double(width) / double(height), 0.001, 5);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
      }

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
  MiniMap::draw_dot(cube);
  MiniMap::draw_dot(ship[(iter+1) & 1]);
}
