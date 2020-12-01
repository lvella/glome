#pragma once

#include <memory>

#include "collidable.hpp"
#include "updatable.hpp"
#include "drawable.hpp"
#include "ai_controller.hpp"
#include "mesh.hpp"
#include "ship_controller.hpp"
#include "fire.hpp"
#include "rot_dir.hpp"

class Ship :
	public SuperObject,
	public Updatable,
	public Glome::Drawable,
	public Collidable
{
public:
	Ship(Mesh::Type type, ShipStats::shared_ptr sstats, float fire_radius=0.001f);

	void create_sub_objects(std::vector<std::weak_ptr<SubObject>>&) override;

	virtual void draw(Camera& c) override;
	virtual bool update(float dt, UpdatableAdder&) override;

	void collided_with(const Collidable& other, float) override;

	void load_guns(Mesh::Type type); //TODO: This method is similar to load_engines, change it!
	void load_engines(Mesh::Type type);
	void set_controller(const std::shared_ptr<ShipController>& pctrl);
	std::shared_ptr<ShipController> ctrl;

	static std::shared_ptr<Ship> make_shared_ship(Mesh::Type type, ShipStats::shared_ptr sstats);

	#ifdef STATS_TUNING
	float get_scale() {
		return stats->scale;
	}
	#endif

protected:
	static constexpr RotDir turn =
		qrotation(Vector3(0.0, math::sqrt1_2, math::sqrt1_2));

	std::shared_ptr<Mesh> mesh;

	// Attributes of the ship
	ShipStats::shared_ptr stats;

	// Gun properties
	QRot r_canon, l_canon;
	uint16_t nguns;

	// Engine properties
	std::shared_ptr<Fire> fx_engine;
	float fire_radius;
	float rel_speed;

	// Ship state
	bool alive = true;
};
