#include "drawable.hpp"

extern GLuint square_vbo;

void Glome::Drawable::minimap_draw(Camera& c)
{
	const Shader *s = c.getShader();
	c.pushMultQRot(get_t());

	glVertexAttrib3f(s->colorAttr(), 1.0f, 0.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
	glVertexAttribPointer(s->posAttr(), 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	c.popMat();
}

void Glome::NoMapDrawable::minimap_draw(Camera& c)
{}
