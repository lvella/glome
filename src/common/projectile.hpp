#pragma once

#include <list>
#include <memory>
#include "matrix4.hpp"
#include "vector4.hpp"
#include "collidable.hpp"
#include "scorer.hpp"

class Projectile final: virtual public Object, public Collidable
{
public:
	static void shot(const std::shared_ptr<Scorer>& s,
		const QRot& from, float speed);

	static void draw_many(const std::vector<Projectile*>& shots, Camera& cam);
	static void draw_in_minimap();
	static void update_all(float dt);
	static std::vector<Collidable*> get_collision_volumes();
	static std::vector<Projectile*> cull_sort_from_camera(const Camera& cam);

	const std::shared_ptr<Scorer>& get_scorer() const
	{
		return scorer;
	}

	void collided_with(const Collidable& other, float) override
	{
		die();
	}

private:
	Projectile(const std::shared_ptr<Scorer>& s, const QRot& from, float speed);
	void draw(Camera& cam);
	void update(float dt);
	inline bool is_dead() const
	{
		// Maximum Time To Live
		return ttl >= max_ttl;
	}

	inline void die()
	{
		ttl = max_ttl;
	}

	float speed;
	float ttl;
	float max_ttl;
	float max_ttl_2;
	unsigned char alpha;

	std::shared_ptr<Scorer> scorer;

	typedef std::list<Projectile> SList;
	static SList shots;
};
