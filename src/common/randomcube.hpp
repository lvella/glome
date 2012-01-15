#pragma once

#include "updatable.hpp"
#include "drawable.hpp"

class RandomCube: public Updatable, public Glome::Drawable 
{
public:
	RandomCube();
	virtual ~RandomCube();

	void update();
	void draw(Camera& s);
	void randomize();
};
