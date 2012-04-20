#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "ai_controller.hpp"
#include "mesh.hpp"
#include "ship_controller.hpp"
#include "fire.hpp"

class Ship : public Updatable, public Glome::Drawable 
{
public:
	Ship(Mesh::Types type = Mesh::HUNTER);
	virtual ~Ship() {};
	virtual void draw(Camera& c);
	virtual void update();
	void load_guns(Mesh::Types type); //TODO: This method is similar to load_engines, change it!
	void load_engines(Mesh::Types type);
	void set_controller(ShipController* pctrl);
	ShipController* ctrl;
	AiController* aux;

protected:
	Mesh* mesh;

	// Gun properties
	Matrix4 r_canon, l_canon;
	uint16_t nguns;
	float shot_speed; /* Speed of shot relative to the ship */
	int shot_power; /* Damage done and heat generated */
	int max_canon_heat; /* Maximum heat supported by canons */
	int canon_cooldown_rate; /* Heat cooldown per frame */
	int cold_fire_rate; /* Shots per second when cold */

	// Engine properties
	float rel_speed;
	Matrix4 velocity;
	uint16_t nengines;
	Fire fx_engine;

	// Shield properties
	uint16_t life;

};

