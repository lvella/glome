#pragma once

#include "drawable.hpp"
#include "camera.hpp"
#include "math.hpp"

class Meridians final:
  public Glome::Drawable
{
public:
	static void initialize();

	Meridians()
	{
		set_radius(math::pi);
	}

	void draw(Camera& c) override;

	virtual void minimap_draw(Camera& c) override;

private:
	static GLuint vbo;
};
