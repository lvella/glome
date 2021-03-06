#include "dustfield.hpp"

#include <vector>

#include "vector4.hpp"
#include "random.hpp"
#include "shader.hpp"
#include "initialization.hpp"
#include "gl.hpp"

namespace DustField
{

struct StarPoint
{
	Vector4 pos;
	uint8_t end;
};

struct Star
{
	StarPoint endpoints[2];
};

const size_t DUST_SIZE = 200000;

static GLuint vbo;
static CamShader program;
static Uniform old_transform;

/** Each star is drawn as a line. If the ship is stopped,
 * the both ends of the line are in the same point.
 * If the ship is moving, the star is stretched by the
 * vertex shader. This attribute tells which end of the
 * the line the vertex is. */
GLint attrib_endpoint;

static RegisterInitialization ini{[] {
	std::vector<Star> dust(DUST_SIZE);

	for(auto &e: dust)
	{
		e.endpoints[0].pos = e.endpoints[1].pos = Random::point();
		e.endpoints[0].end = 0;
		e.endpoints[1].end = 1;
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, dust.size() * sizeof(Star), &dust[0], GL_STATIC_DRAW);

	program.setup_shader({
		"world/dustfield.vert",
		"world/modelview.vert",
		"common/quaternion.vert",
		"world/world.frag",
		"common/no_texture.frag",
		"world/dustfield_fog.frag",
		"world/fog.frag"
	});
	attrib_endpoint = glGetAttribLocation(program.program(), "endpoint");

	old_transform = program.getUniform("old_transform");
}};

static QRot old_cam_transform;

void draw(Camera& cam)
{
	cam.setShader(&program);
	cam.setQRot(QRot::IDENTITY());

	old_transform.set(old_cam_transform);
	old_cam_transform = cam.getBaseTransformation();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(attrib_endpoint);

	glVertexAttribPointer(Shader::ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(StarPoint), (GLvoid*) offsetof(StarPoint, pos));
	glVertexAttribIPointer(attrib_endpoint, 1, GL_UNSIGNED_BYTE, sizeof(StarPoint), (GLvoid*) offsetof(StarPoint, end));

	glDrawArrays(GL_LINES, 0, DUST_SIZE*2);
}

}
