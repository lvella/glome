#pragma once

#include "mesh.hpp"
#include "ship_controller.hpp"

class ship_controller;

class ship : public updatable, public drawable
{
public:
	ship(MeshTypes type = HUNTER);

	void load_guns(MeshTypes type);

	void set_controller(ship_controller* pctrl) { ctrl = pctrl; }

protected:
	Mesh* mesh;
	ship_controller* ctrl;
	int nguns;
	ship_gun gun_l;
	ship_gun gun_r;
	ship_engine engine;
};

