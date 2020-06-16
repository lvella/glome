#include "fire.hpp"

#include "gl.hpp"
#include "options.hpp"
#include "textures.hpp"
#include "math.hpp"
#include "random.hpp"

namespace {

constexpr float FIRE_LIFE = 1.0f/3.0f; // s

CamShader program_fire;
GLint attrib_radius;
int width = -1;

class FireSpecs final: public DrawSpecs<FireSpecs>
{
public:
	template<class Token> FireSpecs(const Token& t):
		DrawSpecs(t)
	{
		program_fire.setup_shader({
			"world/fire.vert",
			"world/modelview.vert",
			"common/quaternion.vert",
			"world/world.frag",
			"world/point_texture.frag",
			"world/world_fog.frag",
			"world/fog.frag"
		});
		attrib_radius = glGetAttribLocation(program_fire.program(), "radius");

		program_fire.enable();
		program_fire.getUniform("has_tex").set(true);
		program_fire.getUniform("texbase").set(0);
		if(width > 0) {
			program_fire.getUniform("half_width").set(width / 2.0f);
		}

		// Without this in GLES
		glEnable(GL_PROGRAM_POINT_SIZE);

		create_circle_texture(64, 0.1, 0, 255, tex_particle, true);
	}

	void setup_draw_state(Camera& c) override
	{
		c.setShader(&program_fire);

		glBindTexture(GL_TEXTURE_2D, tex_particle);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnableVertexAttribArray(program_fire.colorAttr());
		glEnableVertexAttribArray(attrib_radius);
	}

	void shutdown_draw_state(Camera&) override
	{
		glDisableVertexAttribArray(attrib_radius);
		glDisableVertexAttribArray(program_fire.colorAttr());
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	GLuint tex_particle;
};

}

static Vector4 rand_in_sphere(float &r)
{
	Vector4 ret = Random::direction();

	r = r * Random::zeroToOne();
	ret *= r;
	ret.w = -sqrt(1 - ret.x*ret.x - ret.y*ret.y - ret.z*ret.z);
	return ret;
}

Fire::Fire(ParentRef&& parent, float radius):
	ParticleSystem(200),
	SubObject(std::move(parent)),
	scale_radius(radius),
	intensity(1.0f)
{
	set_intensity(0.0f);

	for(int i = 0; i < count; ++i)
	{
		rattrs[i].radius = radius;
		oattrs[i].active = false;
	}
}

void Fire::set_width(int w)
{
	width = w;
	if(program_fire.program() != 0) {
		program_fire.getUniform("half_width").set(width / 2.0f);
	}
}

void Fire::set_intensity(float i)
{
	if(i == intensity)
		return;

	intensity = i;
	origin_radius = scale_radius * 0.7f + i * scale_radius * 0.3f;
	target_count = 15 + i * (count - 15);

	speed = intensity * scale_radius * 24.0;

	const float radius = std::max(scale_radius, speed * FIRE_LIFE);
	set_radius(radius);
}

void Fire::update(float dt)
{
	int new_count = actives_count;

	auto velocity = zw_qrot(speed * dt);
	for(int i = 0; i < count; ++i) {
		OfflineAttributes &oattr = oattrs[i];

		if(!oattr.active && new_count < target_count) {
			oattr.active = true;
			oattr.energy = 0;
			++new_count;
		}

		if(oattr.active) {
			RenderAttributes &rattr = rattrs[i];

			if(oattr.energy <= 0.0) {
				if(new_count > target_count) {
					oattr.active = false;
					--new_count;
					continue;
				}

				float r = origin_radius;
				rattr.position = rand_in_sphere(r);
				r /= origin_radius;
				oattr.energy = FIRE_LIFE * (1.0f - r);
				rattr.color = Vector4(1.0f, 0.7f, 0.0f, 0.0f) * r
					+ Vector4(0.3f, 0.4f, 1.0f, 0.0f) * (1.0 - r);
			} else {
				oattr.energy -= dt;
				rattr.position = velocity * rattr.position;
			}
			rattr.color[3] = oattr.energy / FIRE_LIFE;
		}
	}

	actives_count = new_count;
}

void Fire::draw(Camera& c)
{
	depthSort(c.setQRot(get_world_t()));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * actives_count, idx);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(RenderAttributes) * count, rattrs); /// TEST

	glVertexAttribPointer(program_fire.posAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(*rattrs), 0);
	glVertexAttribPointer(program_fire.colorAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(*rattrs), (GLfloat*)(sizeof(Vector4)));
	glVertexAttribPointer(attrib_radius, 1, GL_FLOAT, GL_FALSE, sizeof(*rattrs), (GLfloat*)(2*sizeof(Vector4)));
	glDrawElements(GL_POINTS, actives_count, GL_UNSIGNED_SHORT, 0);
}

bool Fire::is_transparent() const
{
	return true;
}

DrawSpecsBase& Fire::get_draw_specs() const
{
	return DrawSpecsBase::get_instance<::FireSpecs>();
}
