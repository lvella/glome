#include "gl.hpp"
#include "options.hpp"
#include "textures.hpp"
#include "math.hpp"
#include "random.hpp"

#include "fire.hpp"

static const int FIRE_LIFE = 20;

static CamShader program_fire;
static GLint attrib_radius;

static Vector4 rand_in_sphere(float &r)
{
	Vector4 ret = Random::direction();

	r = r * Random::zeroToOne();
	ret *= r;
	ret.w = -sqrt(1 - ret.x*ret.x - ret.y*ret.y - ret.z*ret.z);
	return ret;
}

Fire::Fire(float radius):
	ParticleSystem(200),
	scale_radius(radius),
	intensity(1.0f)
{
	setIntensity(0.0f);

	create_circle_texture(64, 0.1, 0, 255, tex_particle, true);
	for(int i = 0; i < count; ++i)
	{
		rattrs[i].radius = radius;
		oattrs[i].active = false;
	}
	update();
}

void Fire::initialize()
{
	const char* src[] = {"fire.vert", "world.frag", "point_texture.frag", "world_fog.frag", "fog.frag", NULL};
	program_fire.setup_shader(src);
	attrib_radius = glGetAttribLocation(program_fire.program(), "radius");

	program_fire.enable();
	program_fire.getUniform("has_tex").set(true);
	program_fire.getUniform("texbase").set(0);
	if(width > 0) {
		program_fire.getUniform("half_width").set(width / 2.0f);
	}

	glEnable(GL_POINT_SPRITE);

	// Without this in GLES
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void Fire::set_width(int w)
{
	width = w;
	if(program_fire.program() != 0) {
		program_fire.getUniform("half_width").set(width / 2.0f);
	}
}

int Fire::width = -1;

void Fire::setIntensity(float i)
{
	if(i == intensity)
		return;

	intensity = i;
	velocity = zw_matrix(i * scale_radius * 0.4);
	origin_radius = scale_radius * 0.8f + i * scale_radius * 0.3f;
	target_count = 15 + i * (count - 15);
}

void Fire::update()
{
	int new_count = actives_count;

	for(int i = 0; i < count; ++i) {
		OfflineAttributes &oattr = oattrs[i];

		if(!oattr.active && new_count < target_count) {
			oattr.active = true;
			oattr.energy = 0;
			++new_count;
		}

		if(oattr.active) {
			RenderAttributes &rattr = rattrs[i];

			if(oattr.energy-- <= 0) {
				if(new_count > target_count) {
					oattr.active = false;
					--new_count;
					continue;
				}

				float r = origin_radius;
				rattr.position = _t * rand_in_sphere(r);
				r /= origin_radius;
				oattr.energy = FIRE_LIFE * (1.0f - r);
				rattr.color = Vector4(1.0f, 0.7f, 0.0f, 0.0f) * r + Vector4(0.3f, 0.4f, 1.0f, 0.0f) * (1.0 - r);
			} else {
				rattr.position = velocity * rattr.position;
			}
			rattr.color[3] = oattr.energy / float(FIRE_LIFE);
		}
	}

	actives_count = new_count;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// TODO: take inactive particles out of the way, and only copy relevant data
	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderAttributes) * count, rattrs, GL_STREAM_DRAW);
}

void Fire::draw(Camera& c)
{
	c.pushShader(&program_fire);
	c.pushMultMat(_t);

    depthSort(c.transformation());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * actives_count, idx, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_particle);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnableVertexAttribArray(program_fire.colorAttr());
	glEnableVertexAttribArray(attrib_radius);

	glVertexAttribPointer(program_fire.posAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(*rattrs), 0);
	glVertexAttribPointer(program_fire.colorAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(*rattrs), (GLfloat*)(sizeof(Vector4)));
	glVertexAttribPointer(attrib_radius, 1, GL_FLOAT, GL_FALSE, sizeof(*rattrs), (GLfloat*)(2*sizeof(Vector4)));
	glDrawElements(GL_POINTS, actives_count, GL_UNSIGNED_SHORT, 0);

	c.popMat();
	c.popShader();

	glDisableVertexAttribArray(attrib_radius);
	glDisableVertexAttribArray(program_fire.colorAttr());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
}

