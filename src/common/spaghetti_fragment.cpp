#include "spaghetti_fragment.hpp"

#include <cmath>
#include "random.hpp"

// Same value as in spaghetti_frag.frag:
static constexpr float BURN_LEN = 0.004;

CamShader SpaghettiFragment::shader;
Uniform SpaghettiFragment::burn_progress_uniform;
GLint SpaghettiFragment::center_dist_attr;

void SpaghettiFragment::initialize()
{
	shader.setup_shader({
		"common/quaternion.vert",
		"world/modelview.vert",
		"world/spaghetti_frag.vert",
		"world/world_fog.frag",
		"world/fog.frag",
		"world/spaghetti_frag.frag",
	});

	center_dist_attr = glGetAttribLocation(shader.program(), "center_dist");
	burn_progress_uniform = shader.getUniform("burn_progress");
}

SpaghettiFragment::SpaghettiFragment(const QRot& orig_transformation,
	const std::vector<Spaghetti::Vertex>& svdata,
	uint16_t start, uint16_t size
):
	spin_axis(Random::direction()),
	spin_speed(Random::normalDistribution(0, 20))
{
	assert(size > 0);

	std::vector<Vertex> vdata(size);
	for(uint16_t i = 0; i < vdata.size(); ++i) {
		vdata[i].sv = svdata[(i+start)%svdata.size()];
	}

	float length;
	Vector4 new_origin = center_of_mass(vdata);
	const float half_length = vdata.back().length * 0.5;

	QRot offset = rotation_between_unit_vecs(
		new_origin,
		Vector4::ORIGIN
	);

	translation_speed = 0.1 * Random::zeroToOne();

	translation_dir = RotDir(
		Vector4::ORIGIN,
		random_spread() * new_origin
	);

	translation = orig_transformation * offset.inverse();
	set_t(translation);

	float cos_radius = 1.0;
	for(auto& v: vdata) {
		v.sv.pos = offset * v.sv.pos;
		v.length -= half_length;

		float cos_dist_to_center = Vector4::ORIGIN.dot(v.sv.pos);
		if(cos_dist_to_center < cos_radius) {
			cos_radius = cos_dist_to_center;
		}
	}
	set_radius(std::acos(cos_radius));

	burn_progress = half_length + BURN_LEN;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vdata.size() * sizeof(vdata[0]),
		vdata.data(), GL_STATIC_DRAW);
	draw_size = vdata.size();
}

bool SpaghettiFragment::update(float dt, UpdatableAdder& adder)
{
	translation *= translation_dir(dt * translation_speed);

	spin += dt;
	set_t(translation * qrotation(spin, spin_axis));

	burn_progress -= dt * 0.01;
	return burn_progress > 0.0;
}

void SpaghettiFragment::draw(Camera& c)
{
	c.pushShader(&shader);
	c.pushMultQRot(get_t());

	burn_progress_uniform.set(burn_progress);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(shader.colorAttr());
	glEnableVertexAttribArray(center_dist_attr);

	glVertexAttribPointer(shader.posAttr(), 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, sv.pos));

	glVertexAttribPointer(shader.colorAttr(), 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, sv.color));

	glVertexAttribPointer(center_dist_attr, 1, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, length));

	glDrawArrays(GL_LINE_STRIP, 0, draw_size);

	c.popMat();
	c.popShader();

	glDisableVertexAttribArray(shader.colorAttr());
	glDisableVertexAttribArray(center_dist_attr);
}

// Based on http://ndp.jct.ac.il/tutorials/infitut2/node57.html
Vector4 SpaghettiFragment::center_of_mass(std::vector<Vertex>& vdata)
{
	assert(vdata.size() > 1);

	Vector4 M{0, 0, 0, 0};

	Vertex* prev = &vdata[0];
	prev->length = 0.0f;
	vdata.erase(std::remove_if(vdata.begin() + 1, vdata.end(), [&] (Vertex& v)
	{
		const float cos_len = v.sv.pos.dot(prev->sv.pos);

		if(cos_len >= 1.0) {
			return true;
		}

		const float seg_len = std::acos(cos_len);


		M += (v.sv.pos + prev->sv.pos) * (0.5 * seg_len);
		v.length = prev->length + seg_len;

		prev = &v;
		return false;
	}), vdata.end());

	return M.normalized();
}

QRot SpaghettiFragment::random_spread()
{
	float radius;
	// Ensures radius <= 1.0:
	while(std::fabs(radius = Random::normalDistribution(0.0f, 0.5f)) > 1.0f);

	const float theta = Random::arc();

	const float x = radius * std::sin(theta);
	const float y = radius * std::cos(theta);

	const Vector4 v{x, y, std::sqrt(1.0f - x*x - y*y), 0.0f};

	return rotation_between_unit_vecs(Vector4{0, 0, 1, 0}, v);
}
