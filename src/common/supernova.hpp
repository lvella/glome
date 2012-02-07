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

private:
	// Expansion factor
	float t;

	CamShader shader;
	Mesh* mesh;
	// TODO: minimap mesh
};
