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

	void update();
	void draw(Camera &c);

private:
	Vector2 slerp;
	float t; // Expansion factor

	CamShader shader;
	Uniform slerp_arc;
	Uniform center;
	Mesh* mesh;
};
