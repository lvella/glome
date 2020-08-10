#pragma once

#include <memory>

#include "updatable.hpp"
#include "drawable.hpp"
#include "ai_controller.hpp"
#include "mesh.hpp"
#include "ship_controller.hpp"
#include "fire.hpp"
#include "rot_dir.hpp"

class Ship : public SuperObject, public Updatable, public Glome::Drawable
{
public:
	Ship(Mesh::Types type, ShipStats::shared_ptr sstats);
	~Ship() = default;

	void create_sub_objects(std::vector<std::weak_ptr<SubObject>>&) override;

	virtual void draw(Camera& c) override;
	virtual bool update(float dt, UpdatableAdder&) override;
	void load_guns(Mesh::Types type); //TODO: This method is similar to load_engines, change it!
	void load_engines(Mesh::Types type);
	void set_controller(ShipController* pctrl);
	ShipController* ctrl;

	#ifdef STATS_TUNING
	float get_scale() {
		return stats->scale;
	}
	#endif

protected:
	static constexpr RotDir turn =
		qrotation(Vector3(0.0, math::sqrt1_2, math::sqrt1_2));

	Mesh* mesh;

	// Attributes of the ship
	ShipStats::shared_ptr stats;

	// Gun properties
	QRot r_canon, l_canon;
	uint16_t nguns;

	// Engine properties
	std::shared_ptr<Fire> fx_engine;
	float rel_speed;
	uint16_t nengines;

	// Shield properties
	uint16_t life;
};
