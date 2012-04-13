#include "fire.hpp"
#include "gl.hpp"

static CamShader program_fire;
static GLuint vbo;
static GLint attrib_size;

Fire::Fire(int number_of_particles, Matrix4 velocity):
	ParticleSystem(number_of_particles)
{
	create_circle_texture(56, 0.9, 0, 255, tex_particle);
	for(int i = 0; i < particle_vector.size(); ++i)
	{
		particle_vector[i].active = true;
		particle_vector[i].energy = 100;
		particle_vector[i].fade = 1;
		particle_vector[i].size = 50;
		particle_vector[i].color = Vector4(1,0,0,0.5);
		particle_vector[i].position = Vector4::ORIGIN;//t.position();
		particle_vector[i].velocity = Matrix4::IDENTITY;//velocity;
	}
}

void Fire::initialize()
{
	const char* src[] = {"fire.vert", "world.frag", "point_texture.frag", "fog.frag", NULL};
	program_fire.setup_shader(src);
	attrib_size = glGetAttribLocation(program_fire.program(), "size");
	
	program_fire.enable();
	program_fire.getUniform("has_tex").set(true);
	program_fire.getUniform("texbase").set(0);

	glGenBuffers(1,&vbo);

	// Without this in GLES
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
}

void Fire::update()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 16, particle_vector[0].position.getVertex(), GL_STATIC_DRAW);
}

void Fire::draw(Camera& c)
{
	c.pushShader(&program_fire);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_particle);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//TODO: Pass parameters to shader in a better way
	for(int i = 0; i < /*particle_vector.size()*/1; ++i)
	{
		if(particle_vector[i].active)
		{
			particle_vector[i].position = particle_vector[i].velocity * particle_vector[i].position;
			glVertexAttrib1f(attrib_size, particle_vector[i].size);
			glVertexAttrib4fv(program_fire.colorAttr(),particle_vector[i].color.getVertex());
			glVertexAttribPointer(program_fire.posAttr(), 4, GL_FLOAT, GL_FALSE,  0, NULL);
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}
	glDisable(GL_TEXTURE_2D);

	c.popShader();
}

