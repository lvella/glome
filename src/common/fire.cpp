#include "fire.hpp"
#include "gl.hpp"

static Shader program_fire;
static GLuint vbo;
static GLint attrib_size, uniform_projection, uniform_camera, uniform_coord;

Fire::Fire(int number_of_particles):
	ParticleSystem(number_of_particles)
{
	create_circle_texture(56, 0.9, 0, 255, tex_particle);
	glUniform1i(tex_particle,0);
	for(int i = 0; i < particle_vector.size(); ++i)
	{
		particle_vector[i].active = true;
		particle_vector[i].energy = 100;
		particle_vector[i].fade = 1;
		particle_vector[i].size = 100;
		particle_vector[i].color = Vector4(1,1,1,0.5);
		particle_vector[i].position = t.position();
		particle_vector[i].velocity = Matrix4::IDENTITY;
	}
}

void Fire::initialize()
{
	/* particle shape coordenates */
	static float shape[] = {
			0.0f, 0.0f, 0.0f, -1.0f
	};
	const char* src[] = {"fire.vert", "world.frag", "point_texture.frag", NULL};
	program_fire.setup_shader(src);
	attrib_size = glGetAttribLocation(program_fire.program(), "size");
	uniform_projection = glGetUniformLocation(program_fire.program(), "projection");
	uniform_camera = glGetUniformLocation(program_fire.program(), "camera");
	
	program_fire.enable();
	glUniform1i(glGetUniformLocation(program_fire.program(), "has_tex"), 1);
	glUniform1i(glGetUniformLocation(program_fire.program(), "texbase"), 0);

	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW);
	
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void Fire::ParticleSystem::draw(const Shader& cam)
{
}

void Fire::draw(Matrix4 cam, Matrix4 proj)
{
	program_fire.enable();
	glEnable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_particle);
	cam.loadTo(uniform_camera);
	proj.loadTo(uniform_projection);
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
}

