#include <time.h>
#include <cmath>
#include <algorithm>
#include <vector>

#include "shader.hpp"
#include "math.hpp"

#include "projectile.hpp"

typedef std::vector<Projectile> SList;
static SList shots;

static void
create_spherical_texture(int size, GLuint& tex)
{
	struct elem {
		unsigned char l;
		unsigned char a;
	};

	elem* buffer = (elem *) malloc(size * size * sizeof(elem));
	float r = (float)size / 2.0;

	for(int i = 0; i < size; ++i)
	{
		for(int j = 0; j < size; ++j)
		{
			float d = hypotf(i - r, j - r);
			buffer[(i * size) + j].l = 255u;
			buffer[(i * size) + j].a = d > r ? 0u : (unsigned char)nearbyint(sqrtf(r*r - d*d) / r * 255.0);
		}
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, size, size, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(buffer);
}

static GLuint vbo;
static GLuint texture;
static GLint uniform_has_tex;
static GLint uniform_camera;
static GLint uniform_projection;
static Shader program_bullet;

void Projectile::initialize()
{
	{
		const float data[] = {
				1.0f, 0.78f, 0.59f,
				1.0f, 1.0f,

				0.59f, 1.0f, 0.59f,
				-1.0f, 1.0f,

				0.51f, 0.39f, 0.98f,
				-1.0f, -1.0f,

				1.0f, 0.59f, 0.59f,
				1.0f, -1.0f
		};

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	}

	create_spherical_texture(64, texture);

#include "projectile.vertex.glsl.hpp"
#include "world.fragment.glsl.hpp"
	program_bullet.setup_shader(projectile_vertex_glsl, projectile_vertex_glsl_len, world_fragment_glsl, world_fragment_glsl_len);
	uniform_has_tex = glGetUniformLocation(program_bullet.program(), "has_tex");
	uniform_camera = glGetUniformLocation(program_bullet.program(), "camera");
	uniform_projection = glGetUniformLocation(program_bullet.program(), "projection");
}

void Projectile::shot(Ship *s, const Matrix4& from, float speed)
{
  shots.push_back(Projectile(s, from, speed));
}

void Projectile::update_all(const Vector4& camera_pos)
{
  size_t dead_count = 0;

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
    {
      if(!i->dead())
        i->update(camera_pos);
      else
        {
          // Greater than the maximum possible squared distance (which is 9.87).
          i->order_dist = 10.0f;
          ++dead_count;
        }
    }

  std::sort(shots.begin(), shots.end());
  shots.erase(shots.end() - dead_count, shots.end());
}

void Projectile::draw_all(const Matrix4& projection, const Matrix4& camera)
{
	if(shots.size() != 0) {
		program_bullet.enable();
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glUniform1i(uniform_has_tex, 1);
		camera.loadTo(uniform_camera);
		projection.loadTo(uniform_projection);
		glBindTexture(GL_TEXTURE_2D, texture);

		glEnableVertexAttribArray(program_bullet.colorAttr());

		glVertexAttribPointer(program_bullet.posAttr(), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)0 + 3);
		glVertexAttribPointer(program_bullet.colorAttr(), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)0);

		for(SList::reverse_iterator i = shots.rbegin(); i != shots.rend(); ++i)
			i->draw(program_bullet);

		glDisableVertexAttribArray(program_bullet.colorAttr());
	}
}

void Projectile::draw_in_minimap()
{
  glBegin(GL_POINTS);
  for(SList::iterator i = shots.begin(); i != shots.end(); ++i) {
    i->transformation().position().loadVertex();
  }
  glEnd();
}

bool Projectile::collide(const Vector4& position, float radius)
{
  radius *= radius;

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
    if((position - i->transformation().position()).squared_length() < radius) {
      i->die();
      return true;
    }

  return false;
}

bool Projectile::collide(Ship *s)
{
  const float r = 0.01f * 0.01f;
  Vector4 p = s->transformation().position();

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i) {
    if(s != i->owner && (p - i->transformation().position()).squared_length() < r) {
      i->die();
      return true;
    }
  }

  return false;
}

Projectile::Projectile(Ship *s, const Matrix4& from, float speed):
  Object(from),
  ds(zw_matrix(-speed)),
  owner(s),
  ttl(0),
  max_ttl((2 * M_PI - 0.05) / speed),
  max_ttl_2(max_ttl / 2),
  alpha(255u)
{
}

void Projectile::draw(const Shader& s)
{
	s.setTransform(t);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Projectile::update(const Vector4& camera_pos)
{
  ++ttl;
  alpha = ttl < (max_ttl_2) ? 255u : 255u - (ttl - max_ttl_2) * 200 / max_ttl_2;

  t = t * ds;

  order_dist = (camera_pos - t.position()).squared_length();
}
