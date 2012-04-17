#include "gl.hpp"
#include "options.hpp"
#include "textures.hpp"

#include "fire.hpp"

static const int FIRE_LIFE = 8;

static CamShader program_fire;
static GLint attrib_radius;

static Vector4 rand_in_sphere(float &r)
{
	Vector4 ret(rand() - RAND_MAX / 2, rand() - RAND_MAX / 2, rand() - RAND_MAX / 2, 0.0);

	ret.normalize();
	r = r * rand() / RAND_MAX;
	ret *= r;
	ret.w = -sqrt(1 - ret.x*ret.x - ret.y*ret.y - ret.z*ret.z);
	return ret;
}

Fire::Fire(int number_of_particles, Matrix4 velocity):
	ParticleSystem(number_of_particles)
{
	create_circle_texture(32, 0.1, 0, 255, tex_particle);
	for(int i = 0; i < count; ++i)
	{
		oattrs[i].active = true;
		oattrs[i].energy = i * FIRE_LIFE / (count - 1);
		oattrs[i].velocity = Matrix4::IDENTITY;//velocity;

		rattrs[i].radius = 0.0005f;
		rattrs[i].color = Vector4(0.8, 0.8, 1.0, 0.5);//float(i) / particle_vector.size());
		float r = 0.0005f;
		rattrs[i].position = rand_in_sphere(r);
	}
}

void Fire::initialize()
{
	const char* src[] = {"fire.vert", "world.frag", "point_texture.frag", "fog.frag", NULL};
	program_fire.setup_shader(src);
	attrib_radius = glGetAttribLocation(program_fire.program(), "radius");
	
	program_fire.enable();
	program_fire.getUniform("has_tex").set(true);
	program_fire.getUniform("texbase").set(0);
	if(width > 0) {
		program_fire.getUniform("half_width").set(width / 2.0f);
	}

	// Without this in GLES
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
}

void Fire::set_width(int w)
{
	width = w;
	if(program_fire.program() != 0) {
		program_fire.getUniform("half_width").set(width / 2.0f);
	}
}

int Fire::width = -1;

void Fire::update()
{
	int ac_count = 0;
	for(int i = 0; i < count; ++i) {
		OfflineAttributes &oattr = oattrs[i];
		if(oattr.active) {
			RenderAttributes &rattr = rattrs[i];
			if(oattr.energy-- == 0) {
				float r = 0.0005f;
				rattr.position = _t * rand_in_sphere(r);
				r /= 0.0005f;
				oattr.energy = FIRE_LIFE;// * (1.0f - r);
				rattr.color = Vector4(1.0f, 0.5f, 0.0f, 0.0f) * r + Vector4(0.8, 0.8, 1.0f, 0.0f) * (1.0 - r);
			}
			rattr.color[3] = oattr.energy / float(FIRE_LIFE);
			//rattr.radius = 0.0005f / rattr.color[3];
			++ac_count;
		}
	}

	actives_count = ac_count;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// TODO: take inactive particles out of the way, and only copy relevant data
	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderAttributes) * count, rattrs, GL_STREAM_DRAW);
}

void Fire::draw(Camera& c)
{
	depthSort(c.transformation());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * actives_count, idx, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_particle);

	glEnableVertexAttribArray(program_fire.colorAttr());
	glEnableVertexAttribArray(attrib_radius);

	c.pushShader(&program_fire);

	glVertexAttribPointer(program_fire.posAttr(), 4, GL_FLOAT, GL_FALSE,  sizeof(*rattrs), 0);
	glVertexAttribPointer(program_fire.colorAttr(), 4, GL_FLOAT, GL_FALSE,  sizeof(*rattrs), (GLfloat*)(sizeof(Vector4)));
	glVertexAttribPointer(attrib_radius, 1, GL_FLOAT, GL_FALSE,  sizeof(*rattrs), (GLfloat*)(2*sizeof(Vector4)));
	glDrawElements(GL_POINTS, actives_count, GL_UNSIGNED_SHORT, 0);

	c.popShader();

	glDisableVertexAttribArray(attrib_radius);
	glDisableVertexAttribArray(program_fire.colorAttr());
	glDisable(GL_TEXTURE_2D);
}

