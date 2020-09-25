#include "drawable.hpp"

#include "shader.hpp"
#include "camera.hpp"

extern GLuint square_vbo;

namespace {

class DefaultDrawSpec final: public DrawSpecs<DefaultDrawSpec>
{
public:
	template<typename Token>
	DefaultDrawSpec(const Token& token):
		DrawSpecs(token)
	{
		shader.setup_shader({
			"world/world.vert", "world/modelview.vert",
			"common/quaternion.vert", "world/world.frag",
			"world/world_fog.frag", "common/no_texture.frag",
			"world/fog.frag"
		});
	}

	void setup_draw_state(Camera& c)
	{
		c.setShader(&shader);
		glEnableVertexAttribArray(Shader::ATTR_COLOR);
	}

private:
	CamShader shader;
};

}

void Glome::Drawable::minimap_draw(Camera& c)
{
	const Shader *s = c.getShader();
	c.setQRot(get_t());

	glVertexAttrib3f(Shader::ATTR_COLOR, 1.0f, 0.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
	glVertexAttribPointer(Shader::ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

bool Glome::Drawable::is_transparent() const
{
	return false;
}

void Glome::NoMapDrawable::minimap_draw(Camera& c)
{}

DrawSpecsBase& Glome::Drawable::get_draw_specs() const
{
	return DrawSpecsBase::get_instance<::DefaultDrawSpec>();
}
