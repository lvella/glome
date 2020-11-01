#pragma once

#include <list>
#include <memory>
#include "drawable.hpp"
#include "matrix4.hpp"
#include "updatable.hpp"
#include "vector4.hpp"
#include "collidable.hpp"
#include "scorer.hpp"

class Projectile final:
	public Updatable,
	public Glome::NoMapDrawable,
	public Collidable
{
public:
	Projectile(const std::shared_ptr<Scorer>& s, const QRot& from, float speed);

	const std::shared_ptr<Scorer>& get_scorer() const
	{
		return scorer;
	}

	void collided_with(const Collidable& other, float) override;
	bool update(float dt, UpdatableAdder&) override;

	DrawSpecsBase& get_draw_specs() const override;

private:
	void draw(Camera& cam) override;

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
