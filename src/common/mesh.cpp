#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <map>
#include <set>
#include <utility>

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

  fclose(fd);

  len = ilen * 2;
  primitive_type = GL_LINES;
}

// based on http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
void Mesh::generate_icosphere()
{
	struct Builder {
		uint16_t e[480][2];
		uint16_t faces[320][3];
		struct {
			Vector4 p;
			Vector4 c;
		} v[12 + 30 + 120];
		map<pair<uint16_t, uint16_t>, uint16_t> vertices;
		set<pair<uint16_t, uint16_t>> edges;
		uint16_t iv;
		uint16_t ie;
		uint16_t ifaces;

		Builder()
		{
			iv = 0;
			ie = 0;
			ifaces = 0;

			const float P = float((1.0 + sqrtf(5.0)) / 2.0); // golden ratio, used in building of an icosahedron
			const unsigned char SUB = 2; // number of subdivisions, maximum 2

			// initial vertexes
			const float VERTS[12][4] =
			{
					{-1,  P, 0, 0},
					{ 1,  P, 0, 0},
					{-1, -P, 0, 0},
					{ 1, -P, 0, 0},

					{0, -1,  P, 0},
					{0,  1,  P, 0},
					{0, -1, -P, 0},
					{0,  1, -P, 0},

					{ P, 0, -1, 0},
					{ P, 0,  1, 0},
					{-P, 0, -1, 0},
					{-P, 0,  1, 0},
			};
			for(int i = 0; i < 12; ++i) {
				v[iv++].p = Vector4(VERTS[i][0], VERTS[i][1], VERTS[i][2], VERTS[i][3]);
			}

			// initial faces for recursive subdivision
			const uint16_t FACES[20][3] =
			{
					// 5 faces around point 0
					{0, 11, 5},
					{0, 5, 1},
					{0, 1, 7},
					{0, 7, 10},
					{0, 10, 11},

					// 5 adjacent faces
					{1, 5, 9},
					{5, 11, 4},
					{11, 10, 2},
					{10, 7, 6},
					{7, 1, 8},

					// 5 faces around point 3
					{3, 9, 4},
					{3, 4, 2},
					{3, 2, 6},
					{3, 6, 8},
					{3, 8, 9},

					// 5 adjacent faces
					{4, 9, 5},
					{2, 4, 11},
					{6, 2, 10},
					{8, 6, 7},
					{9, 8, 1},
			};
			for(int i = 0; i < 20; ++i) {
				face_subdivide(SUB, FACES[i][0], FACES[i][1], FACES[i][2]);
			}

		  assert(iv == sizeof(v) / (2 * sizeof(Vector4)));
		  assert(ie == sizeof(e) / 4);
		  assert(ifaces == sizeof(faces) / 6);

		  // Scale all vertices to radius
		  // Project them to 4-D
		  // Assign random colors
		  for(int i = 0; i < iv; ++i) {
		  	v[i].p.normalize();

		  	v[i].c = Vector4(
		  			rand() / float(RAND_MAX),
		  			rand() / float(RAND_MAX),
		  			rand() / float(RAND_MAX),
		  			1.0f);
		  }
		}

		uint16_t middle_vert(uint16_t a, uint16_t b)
		{
			if(a < b)
				swap(a, b);
			auto key = make_pair(a, b);
			auto outcome = vertices.insert(make_pair(key, iv));
			if(outcome.second) {
				v[iv++].p = (v[a].p + v[b].p) * 0.5;
			}
			return outcome.first->second;
		}

		void insert_edge(uint16_t a, uint16_t b)
		{
			if(a < b)
				swap(a, b);
			auto outcome = edges.insert(make_pair(a, b));
			if(outcome.second) {
				e[ie][0] = a;
				e[ie][1] = b;
				++ie;
			}
		}

		void face_subdivide(size_t iter, size_t a, size_t b, size_t c)
		{
			if(iter) {
				size_t x, y, z;

				x = middle_vert(a, b);
				y = middle_vert(b, c);
				z = middle_vert(c, a);

				--iter;
				face_subdivide(iter, a, x, z);
				face_subdivide(iter, x, b, y);
				face_subdivide(iter, z, y, c);
				face_subdivide(iter, x, y, z);
			}
			else {
				insert_edge(a, b);
				insert_edge(b, c);
				insert_edge(c, a);

				faces[ifaces][0] = a;
				faces[ifaces][1] = b;
				faces[ifaces][2] = c;
				++ifaces;
			}
		}
	} b;

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(b.v), b.v, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // TODO: one of the two kind of primitives data being generated is useless...
  // maybe remove it when we are satisfied with the result?

  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(b.e), b.e, GL_STATIC_DRAW);
  //len = 2 * b.ie;
  //primitive_type = GL_LINES;

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(b.faces), b.faces, GL_STATIC_DRAW);
  len = 3 * b.ifaces;
  primitive_type = GL_TRIANGLES;
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

  glDrawElements(primitive_type, len, GL_UNSIGNED_SHORT, NULL);
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
