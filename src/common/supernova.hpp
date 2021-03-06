#pragma once

#include "vector2.hpp"
#include "updatable.hpp"
#include "drawable.hpp"
#include "collidable.hpp"
#include "mesh.hpp"
#include <memory>

// Supernova that expands and eats the whole world!
class Supernova final: public Glome::Drawable, public Updatable, public Collidable
{
public:
	Supernova();

	bool update(float dt, UpdatableAdder &) override;
	void draw(Camera &c) override;
	void minimap_draw(Camera &c) override;

	DrawSpecsBase& get_draw_specs() const override;

	void collided_with(const Collidable& other, float) override;

	bool is_transparent() const override;

private:
	Vector2 slerp;

	// Stuff that should be static:
	std::shared_ptr<Mesh> map_mesh;
	SpaceShader map_shader;
	Uniform map_slerp_arc;
};
