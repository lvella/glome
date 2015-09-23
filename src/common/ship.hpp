#pragma once

#include <memory>

#include "updatable.hpp"
#include "drawable.hpp"
#include "ai_controller.hpp"
#include "mesh.hpp"
#include "ship_controller.hpp"
#include "fire.hpp"

class Ship : public Updatable, public Glome::Drawable
{
public:
	Ship(Mesh::Types type, ShipStats::shared_ptr sstats);
	virtual ~Ship() {};
	virtual void draw(Camera& c);
	virtual void update();
	void load_guns(Mesh::Types type); //TODO: This method is similar to load_engines, change it!
	void load_engines(Mesh::Types type);
	void set_controller(ShipController* pctrl);
	ShipController* ctrl;
	AiController* aux;

	#ifdef STATS_TUNING
	float get_scale() {
		return stats->scale;
	}
	#endif

protected:
	Mesh* mesh;

	// Attributes of the ship
	ShipStats::shared_ptr stats;

	// Gun properties
	Matrix4 r_canon, l_canon;
	uint16_t nguns;

	// Engine properties
	float rel_speed;
	Matrix4 velocity;
	uint16_t nengines;
	Fire fx_engine;

	// Shield properties
	uint16_t life;
};

