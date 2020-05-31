#pragma once

#include "vector2.hpp"
#include "updatable.hpp"
#include "drawable.hpp"
#include "collidable.hpp"
#include "mesh.hpp"

// Supernova that expands and eats the whole world!
class Supernova final: public Glome::Drawable, public Updatable, public Collidable
{
public:
	Supernova();
	~Supernova();

	bool update(float dt, UpdatableAdder &) override;
	void draw(Camera &c) override;
	void minimap_draw(Camera &c) override;

	void collided_with(const Collidable& other, float) override;

private:
	Vector2 slerp;

	// Stuff that should be static:
	GLuint bg_noise;

	Mesh* mesh;
	CamShader shader;
	Uniform slerp_arc;
	Uniform center;

	Mesh* map_mesh;
	SpaceShader map_shader;
	Uniform map_slerp_arc;
};
