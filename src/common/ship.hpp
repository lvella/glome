#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "ai_controller.hpp"
#include "engine.hpp"
#include "mesh.hpp"
#include "ship_controller.hpp"

class Ship : public Updatable, public Glome::Drawable 
{
public:
	Ship(MeshTypes type = HUNTER);
	~Ship() {};
	void draw(Camera& c);
	void update();
	void load_guns(MeshTypes type);
	void set_controller(ShipController* pctrl);

protected:
	Mesh* mesh;
	ShipController* ctrl;
	Matrix4 r_canon, l_canon;
	ship_gun gun_l;
	ship_gun gun_r;
	uint16_t nguns;
	Engine* engine;

};

