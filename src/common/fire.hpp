#pragma once

#include "particle_system.hpp"
#include "updatable.hpp"
#include "drawable.hpp"

class Fire final: public ParticleSystem, public Updatable, public Glome::Drawable
{
public:
	Fire(float radius);

	void update(float dt) override;
	void draw(Camera& c) override;
	void minimap_draw(Camera& c) override;
	void setIntensity(float i);

	static void initialize();

	// Little hackish: must be called if viewport ever happens to change size.
	static void set_width(int w);

private:
	float scale_radius;
	float origin_radius;
	float intensity;

	uint16_t target_count;

	static int width;
};

