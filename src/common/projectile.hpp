#pragma once

#include <list>
#include "matrix4.hpp"
#include "vector4.hpp"
#include "vol_sphere.hpp"
#include "ship_controller.hpp"

class Projectile: virtual public Object, public VolSphere
{
public:
	static void initialize();
	static void shot(ShipController *s, const Matrix4& from, float speed);
	static void draw_many(const std::vector<Projectile*>& shots, Camera& cam);
	static void draw_in_minimap();
	static void update_all();
	static std::vector<VolSphere*> get_collision_volumes();
	static std::vector<Projectile*> cull_sort_from_camera(const Camera& cam);

	void collided_with(const VolSphere& other, float) override
	{
		die();
	}

private:
	Projectile(ShipController *s, const Matrix4& from, float speed);
	void draw(Camera& cam);
	void update();
	inline bool is_dead() const
	{
		// Maximum Time To Live
		return ttl >= max_ttl;
	}

	inline void die()
	{
		ttl = max_ttl;
	}

	Matrix4 ds;
	ShipController *owner;
	unsigned short ttl;
	unsigned short max_ttl;
	unsigned short max_ttl_2;
	unsigned char alpha;

	typedef std::list<Projectile> SList;
	static SList shots;
};
