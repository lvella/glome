#pragma once

#include "particle_system.hpp"
#include "updatable.hpp"
#include "drawable.hpp"

class Fire final: public ParticleSystem, public Updatable, public Glome::NoMapDrawable
{
public:
	Fire(float radius);

	bool update(float dt, UpdatableAdder&) override;
	void draw(Camera& c) override;
	bool is_transparent() const override;
	void setIntensity(float i);

	DrawSpecs& get_draw_specs() const override;

	// Little hackish: must be called if viewport ever happens to change size.
	static void set_width(int w);

private:
	float scale_radius;
	float origin_radius;
	float intensity;
	float speed;

	uint16_t target_count;
};

