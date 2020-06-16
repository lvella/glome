#pragma once

#include "matrix4.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "vol_sphere.hpp"
#include "utils.hpp"
#include "initialization.hpp"

class DrawSpecsBase: public MandatorySingleton
{
public:
	using MandatorySingleton::MandatorySingleton;

	virtual ~DrawSpecsBase() = default;

	virtual void setup_draw_state(Camera& c) = 0;
	virtual void shutdown_draw_state(Camera& c) {};
};

template<class T>
class DrawSpecs: public DrawSpecsBase
{
public:
	using DrawSpecsBase::DrawSpecsBase;

private:
	static RegisterInitialization ini;
};

template<class T>
RegisterInitialization DrawSpecs<T>::ini {[] {
	MandatorySingleton::get_instance<T>();
}};

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

		virtual DrawSpecsBase& get_draw_specs() const;
	};

	class NoMapDrawable: public Drawable
	{
	public:
		virtual ~NoMapDrawable() {};
		void minimap_draw(Camera& c) override final;
	};
}
