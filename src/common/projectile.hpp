#pragma once

#include "matrix4.hpp"
#include "vector4.hpp"
#include "object.hpp"
#include "ship_controller.hpp"

class Projectile: public Object
{
public:
	static void initialize();
	static void shot(ShipController *s, const Matrix4& from, float speed);
	static void draw_many(const std::vector<Projectile*>& shots, Camera& cam);
	static void draw_in_minimap();
	static void update_all();
	static std::vector<Projectile*> cull_sort_from_camera(const Camera& cam);
	static bool collide(const Vector4& position, float radius);
	static bool collide(ShipController *s);

private:
	Projectile(ShipController *s, const Matrix4& from, float speed);
	void draw(Camera& cam);
	void update();
	inline bool dead()
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
};
