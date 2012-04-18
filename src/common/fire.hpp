#pragma once

#include "particle_system.hpp"

class Fire : public ParticleSystem
{
public:
	Fire(float radius);
	virtual ~Fire(){}

	void update();
	void draw(Camera& c);
	void setIntensity(float i);

	static void initialize();

	// Little hackish: must be called if viewport ever happens to change size.
	static void set_width(int w);

private:
	Matrix4 velocity;
	float scale_radius;
	float origin_radius;
	uint16_t target_count;

	static int width;
};

