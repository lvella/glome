#include "meridian.hpp"

#include "gl.hpp"
#include "math.hpp"
#include "minimap.hpp"
#include "initialization.hpp"

RegisterInitialization Meridians::ini{[] {
  glGenBuffers(1, &vbo);
  float vdata[360*4];

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  for(int i = 0; i < 360; ++i)
    {
      double n = i * math::pi / 180.0;
      vdata[4*i] = cos(n);
      vdata[4*i+1] = sin(n);
      vdata[4*i+2] = 0.0f;
      vdata[4*i+3] = 0.0f;
    }

  glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), (GLvoid*)vdata, GL_STATIC_DRAW);
}};

void
Meridians::draw(Camera &c)
{
	const Shader *s = c.getShader();

	glDisableVertexAttribArray(Shader::ATTR_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glVertexAttribPointer(Shader::ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	c.setQRot(QRot::IDENTITY());
	glVertexAttrib4f(Shader::ATTR_COLOR, 1.0f, 1.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 360);

	QRot t = xz_qrot(math::pi_2);
	c.setQRot(t);
	glVertexAttrib4f(Shader::ATTR_COLOR, .0f, 1.f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 360);

	t *= yz_qrot(math::pi_2);
	c.setQRot(t);
	glVertexAttrib4f(Shader::ATTR_COLOR, 1.0f, .0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 360);

	t *= xw_qrot(-math::pi_2);
	c.setQRot(t);
	glVertexAttrib4f(Shader::ATTR_COLOR, 1.0f, .0f, .0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 360);

	t *= yw_qrot(-math::pi_2);
	c.setQRot(t);
	glVertexAttrib4f(Shader::ATTR_COLOR, .0f, .0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 360);

	t *= yz_qrot(math::pi_2);
	c.setQRot(t);
	glVertexAttrib4f(Shader::ATTR_COLOR, .0f, 1.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 360);

	glEnableVertexAttribArray(Shader::ATTR_COLOR);
}

void
Meridians::minimap_draw(Camera& c)
{
	glUniform1i(MiniMap::proj_has_tex, 0);
	draw(c);
}

GLuint Meridians::vbo;
