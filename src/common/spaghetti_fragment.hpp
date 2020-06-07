#pragma once

#include "rot_dir.hpp"
#include "updatable.hpp"
#include "drawable.hpp"
#include "spaghetti.hpp"

class SpaghettiFragment final:
	public Updatable,
	public Glome::NoMapDrawable
{
public:
	SpaghettiFragment(const QRot& orig_transformation,
		const std::vector<Spaghetti::Vertex>& vdata,
		uint16_t start, uint16_t size);

	bool update(float dt, UpdatableAdder& adder) override;
	void draw(Camera& c) override;

	DrawSpecs& get_draw_specs() const override;

private:
	struct Vertex {
		Spaghetti::Vertex sv;
		float length;
	};

	static Vector4 center_of_mass(std::vector<Vertex>& vdata);

	// Returns a acute angle random rotation around ORIGIN.
	static QRot random_spread();

	BufferObject vbo;
	GLsizei draw_size;

	QRot translation;
	RotDir translation_dir;
	float translation_speed;

	Vector3 spin_axis;
	float spin = 0;
	float spin_speed;

	float burn_progress;
};

