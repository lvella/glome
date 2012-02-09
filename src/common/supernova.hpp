#pragma once

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
	Vector4 slerp_s;
	float slerp_m;
	float t; // Expansion factor

	CamShader shader;
	Uniform slerp_sum;
	Uniform slerp_mult;
	Mesh* mesh;
};
