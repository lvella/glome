#pragma once

#include "particle_system.hpp"
#include "updatable.hpp"
#include "drawable.hpp"
#include "sub_object.hpp"

class Fire final: public ParticleSystem, public SubObject, public Glome::NoMapDrawable
{
public:
	Fire(ParentRef&& parent, float radius);

	void draw(Camera& c) override;
	bool is_transparent() const override;
	void set_intensity(float i);
	float get_intensity() const
	{
		return intensity;
	}

	void update(float dt);

	DrawSpecsBase& get_draw_specs() const override;

	// Little hackish: must be called if viewport ever happens to change size.
	static void set_width(int w);

private:
	float scale_radius;
	float origin_radius;
	float intensity;
	float speed;

	uint16_t target_count;
};

