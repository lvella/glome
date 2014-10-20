#include <vector>
#include "vector4.hpp"
#include "random.hpp"
#include "gl.hpp"

#include "dustfield.hpp"

namespace DustField
{

std::vector<Vector4> dust;
GLuint vbo;

void initialize()
{
    dust.resize(1000);

    for(auto &e: dust)
    {
        e = Random::point();
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, dust.size() * sizeof(Vector4), &dust[0], GL_STATIC_DRAW);
}

void draw(Camera& cam)
{
    const Shader *s = cam.getShader();
    glDisable(GL_PROGRAM_POINT_SIZE);
    glPointSize(1.0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(s->colorAttr());
    glVertexAttribPointer(s->posAttr(), 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glVertexAttrib4f(s->colorAttr(), 1.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_POINTS, 0, dust.size());
}

}
