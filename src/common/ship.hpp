#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "ai_controller.hpp"
#include "engine.hpp"
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
	void load_guns(Mesh::Types type);
	void set_controller(ShipController* pctrl);
	ShipController* ctrl;
	AiController* aux;

protected:
	Mesh* mesh;

	Matrix4 r_canon, l_canon;
	ship_gun gun_l;
	ship_gun gun_r;
	uint16_t nguns;
	Engine* engine;

	Fire fx_engine;
};

