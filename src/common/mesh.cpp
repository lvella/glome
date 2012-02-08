#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <cassert>

#include "world.hpp"
#include "mesh.hpp"
#include "config.hpp"
#include "vector4.hpp"

using namespace std;

static Mesh* mesh_list[MESH_COUNT] = {NULL};

const char* mesh_filename[MESH_COUNT] =
  {
    "hunter",
    "destroyer",
    "ufo",
    NULL
  };

Mesh::~Mesh()
{
  glDeleteBuffers(2, bufobjs);
}

Mesh::Mesh(MeshTypes type):
  ref_count(1)
{
	assert(size_t(type) < MESH_COUNT);

  const char* name = mesh_filename[int(type)];

  glGenBuffers(2, bufobjs);

  if(name)
  	load_from_file(name);
  else {
  	// switch (type) { // when there are more than one procedural type
  	generate_icosphere();
  }
}

void Mesh::load_from_file(const char* name)
{
  uint16_t ilen, vlen;

  int ret;
  FILE *fd;

  std::cout << "Loading mesh " << name << std::endl;

  // Load mesh file and put it into the list of meshs if was not exist
  {
		unsigned int mesh_pos;
    std::stringstream dir;
    dir << DATA_DIR << "/models/" << name << ".wire";
    fd = fopen(dir.str().c_str(), "rb");
    /* Read header of file */
		fread(&mesh_pos, sizeof(unsigned int), 1, fd);
		/* Poiter file to mesh position */
    fseek(fd, mesh_pos, SEEK_SET);
    assert(fd != NULL);
  }

  {
    // Reading 4-D vertex coordinates(16bytes) and colorRGBA values(16bytes)
    // format: <x, y, z, w> <r, g, b, a>
    ret = fread(&vlen, sizeof(vlen), 1, fd);
    assert(ret == 1);
    // Create vertex buffer
    uint16_t vbolen = vlen * 2 * 4 * sizeof(float);
    float vdata[vbolen];
    ret = fread(vdata, 2 * 4 * sizeof(float), vlen, fd);
    assert(ret == vlen);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vbolen, vdata, GL_STATIC_DRAW);
  }

  {
    // Reading 4-D edges coordinates (8bytes)
    // format:  <v_index0 , v_index1>
    ret = fread(&ilen, sizeof(ilen), 1, fd);
    assert(ret == 1);
    // Create index buffer
    uint16_t ibolen = ilen * 2 * sizeof(uint16_t);
    uint16_t idata[ibolen];
    ret = fread(idata, 2 * sizeof(uint16_t), ilen, fd);
    assert(ret == ilen);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibolen, idata, GL_STATIC_DRAW);
  }

  len = ilen * 2;

  fclose(fd);
}

// based on http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
void Mesh::generate_icosphere()
{
	struct Builder {
		uint16_t e[480][2];
		Vector4 v[12 + 30 + 120];
		int iv;
		int ie;

		Builder()
		{
			const float P = float((1.0 + sqrtf(5.0)) / 2.0); // golden ratio, used in building of an icosahedron
			const float R = 0.05; // shrink the ball to this radius

			iv = 0;
			ie = 0;

			// initial vertexes
			v[iv++] = Vector4(-1,  P, 0, 0);
			v[iv++] = Vector4( 1,  P, 0, 0);
			v[iv++] = Vector4( 1, -P, 0, 0);
			v[iv++] = Vector4(-1, -P, 0, 0);

			v[iv++] = Vector4(0, -1,  P, 0);
			v[iv++] = Vector4(0,  1,  P, 0);
			v[iv++] = Vector4(0,  1, -P, 0);
			v[iv++] = Vector4(0, -1, -P, 0);

			v[iv++] = Vector4( P, 0, -1, 0);
			v[iv++] = Vector4( P, 0,  1, 0);
			v[iv++] = Vector4(-P, 0,  1, 0);
			v[iv++] = Vector4(-P, 0, -1, 0);

			// recursive face subdivision
		  face_subdivide(2, 0, 11, 5);
		  face_subdivide(2, 0, 5, 1);
		  face_subdivide(2, 0, 1, 7);
		  face_subdivide(2, 0, 7, 10);
		  face_subdivide(2, 0, 10, 11);

		  // 5 adjacent faces
		  face_subdivide(2, 1, 5, 9);
		  face_subdivide(2, 5, 11, 4);
		  face_subdivide(2, 11, 10, 2);
		  face_subdivide(2, 10, 7, 6);
		  face_subdivide(2, 7, 1, 8);

		  // 5 faces around point 3
		  face_subdivide(2, 3, 9, 4);
		  face_subdivide(2, 3, 4, 2);
		  face_subdivide(2, 3, 2, 6);
		  face_subdivide(2, 3, 6, 8);
		  face_subdivide(2, 3, 8, 9);

		  // 5 adjacent faces
		  face_subdivide(2, 4, 9, 5);
		  face_subdivide(2, 2, 4, 11);
		  face_subdivide(2, 6, 2, 10);
		  face_subdivide(2, 8, 6, 7);
		  face_subdivide(2, 9, 8, 1);

		  assert(iv == sizeof(v) / sizeof(float));
		  assert(ie == sizeof(e) / 4);

		  // Scale all vertices to radius
		  // Project them to 4-D
		  for(int i = 0; i < iv; ++i) {
		  	v[i].normalize();
		  	v[i] *= R;
		  	v[i].calc_norm_w();

		  	std::cout << v[i].dot(Vector4::CANONICAL);
		  }
		}

		void face_subdivide(size_t iter, size_t a, size_t b, size_t c)
		{
			if(iter) {
				int x, y, z;

				v[x = iv++] = (v[a] + v[b]) * 0.5;
				v[y = iv++] = (v[b] + v[c]) * 0.5;
				v[z = iv++] = (v[c] + v[a]) * 0.5;

				--iter;
				face_subdivide(iter, a, x, z);
				face_subdivide(iter, x, b, y);
				face_subdivide(iter, z, y, c);
				face_subdivide(iter, x, y, z);
			}
			else {
				e[ie][0] = a;
				e[ie][1] = b;
				++ie;

				e[ie][0] = b;
				e[ie][1] = c;
				++ie;

				e[ie][0] = c;
				e[ie][1] = a;
				++ie;
			}
		}
	} b;

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(b.v), b.v, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(b.e), b.e, GL_STATIC_DRAW);
}

void
Mesh::draw(Camera& c)
{
	const Shader *s = c.getShader();

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glEnableVertexAttribArray(s->colorAttr());

  glVertexAttribPointer(s->posAttr(), 4, GL_FLOAT, GL_FALSE, 32, NULL);
  glVertexAttribPointer(s->colorAttr(), 4, GL_FLOAT, GL_FALSE, 32, (void*) (4 * sizeof(float)));

  glDrawElements(GL_LINES, len, GL_UNSIGNED_SHORT, NULL);
  glDisableVertexAttribArray(s->colorAttr());
}

Mesh*
Mesh::get_mesh(MeshTypes type)
{
  Mesh *&m = mesh_list[int(type)];
  if(m)
    ++m->ref_count;
  else
    m = new Mesh(type);
  return m;
}

void
Mesh::release_mesh(Mesh* m)
{
  --m->ref_count;
  if(!m->ref_count)
    delete(m);
}
