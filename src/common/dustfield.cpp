#include <vector>
#include <cstddef>
#include "vector4.hpp"
#include "random.hpp"
#include "shader.hpp"
#include "gl.hpp"

#include "dustfield.hpp"

namespace DustField
{

struct StarPoint
{
    Vector4 pos;
    float end;
};

struct Star
{
    StarPoint endpoints[2];
};

const size_t DUST_SIZE = 200000;

GLuint vbo;
CamShader program;
Uniform old_transform;

/** Each star is drawn as a line. If the ship is stopped,
 * the both ends of the line are in the same point.
 * If the ship is moving, the star is stretched by the
 * vertex shader. This attribute tells which end of the
 * the line the vertex is. */
GLint attrib_endpoint;

void initialize()
{
    std::vector<Star> dust(DUST_SIZE);

    for(auto &e: dust)
    {
        e.endpoints[0].pos = e.endpoints[1].pos = Random::point();
        e.endpoints[0].end = 0.0f;
        e.endpoints[1].end = 1.0f;
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, dust.size() * sizeof(Star), &dust[0], GL_STATIC_DRAW);

	const char* src[] = {"dustfield.vert", "world.frag", "no_texture.frag", "fog.frag", NULL};
	program.setup_shader(src);
	attrib_endpoint = glGetAttribLocation(program.program(), "endpoint");

	old_transform = program.getUniform("old_transform");
}

void draw(Camera& cam, const Matrix4 &old_cam_transform)
{
    cam.pushShader(&program);

    old_transform.set(old_cam_transform);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(attrib_endpoint);

    glVertexAttribPointer(program.posAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(StarPoint), (GLvoid*) offsetof(StarPoint, pos));
    glVertexAttribPointer(attrib_endpoint, 1, GL_FLOAT, GL_FALSE, sizeof(StarPoint), (GLvoid*) offsetof(StarPoint, end));

    glDrawArrays(GL_LINES, 0, DUST_SIZE*2);

    cam.popShader();
}

}
