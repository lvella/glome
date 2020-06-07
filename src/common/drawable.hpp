#pragma once

#include "matrix4.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "vol_sphere.hpp"
#include "utils.hpp"

class DrawSpecs: public MandatorySingleton
{
public:
	using MandatorySingleton::MandatorySingleton;

	virtual ~DrawSpecs() = default;

	virtual void setup_draw_state(Camera& c) = 0;
	virtual void shutdown_draw_state(Camera& c) {};
};

// TODO: either remove this namespace, or add it to everything else
namespace Glome
{
	class Drawable : virtual public Object, virtual public VolSphere
	{
	public:
		virtual ~Drawable() {};
		virtual void draw(Camera& c) = 0;
		virtual void minimap_draw(Camera& c);

		virtual bool is_transparent() const;

		virtual DrawSpecs& get_draw_specs() const;
	};

	class NoMapDrawable: public Drawable
	{
	public:
		virtual ~NoMapDrawable() {};
		void minimap_draw(Camera& c) override final;
	};
}
