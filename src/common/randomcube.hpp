#pragma once

#include "updatable.hpp"
#include "drawable.hpp"

class RandomCube: public Updatable, public drawable::Drawable 
{
public:
	RandomCube();
	~RandomCube();

	void update();
	void draw(const Shader& s);
	void randomize();
};
