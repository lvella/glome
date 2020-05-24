#pragma once

#include "vector2.hpp"
#include "updatable.hpp"
#include "drawable.hpp"
#include "mesh.hpp"

// Supernova that expands and eats the whole world!
class Supernova: public Glome::Drawable, public Updatable
{
public:
	Supernova();
	virtual ~Supernova();

	void update() override;
	void draw(Camera &c) override;
	void minimap_draw(Camera &c) override;

private:
	Vector2 slerp;
	float size; // Expansion factor

	// Stuff that should be static:
	GLuint bg_noise;

	Mesh* mesh;
	CamShader shader;
	Uniform slerp_arc;
	Uniform center;

	Mesh* map_mesh;
	CamShader map_shader;
	Uniform map_slerp_arc;
};
