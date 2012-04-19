#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <map>
#include <set>
#include <utility>

#include "config.hpp"
#include "vector4.hpp"
#include "world.hpp"
#include "random.hpp"
#include "mesh.hpp"

using namespace std;

static Mesh* mesh_list[Mesh::MESH_COUNT] = {NULL};

const char* mesh_filename[Mesh::MESH_COUNT] =
  {
    "hunter",
    "destroyer",
    "ufo",
    NULL, // icosphere
    NULL, // uvsphere
  };

Mesh::~Mesh()
{
  glDeleteBuffers(2, bufobjs);
}

Mesh::Mesh(Types type):
  ref_count(1)
{
	assert(size_t(type) < MESH_COUNT);

	const char* name = mesh_filename[int(type)];

	glGenBuffers(2, bufobjs);

	if(name)
		load_from_file(name);
	else {
		switch (type) { // when there are more than one procedural type
		case ICOSPHERE:
			generate_icosphere();
			break;
		case UVSPHERE:
			generate_uvsphere();
			break;
		default:
			assert(0 && "Invalid mesh type.");
		}
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
		/* Pointer file to mesh position */
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
  has_colorbuf = true;
}

// Not a full uv sphere: open in the poles so it looks cool in minimap
void Mesh::generate_uvsphere()
{
	const uint8_t SEGMENTS = 16;
	const uint8_t RINGS = 17; // must be odd

	Vector4 v[SEGMENTS * RINGS];
	uint16_t e[3 * SEGMENTS + SEGMENTS * (RINGS - 1)][2];

	uint16_t e_idx = 0;
	uint16_t v_idx = 0;

	// Create vertices and link segments
	for(int i = 0; i < SEGMENTS; ++i)
	{
		float a = i * (2.0 * M_PI / SEGMENTS);
		float x, y;

		x = sin(a);
		y = cos(a);

		for(int j = 0; j < RINGS; ++j)
		{
			if(j > 0) {
				e[e_idx][0] = v_idx - 1;
				e[e_idx][1] = v_idx;
				++e_idx;
			}

			float b = 0.15 + j * ((M_PI - 0.3) / (RINGS - 1));
			float sb = sin(b);

			v[v_idx++] = Vector4(sb * x, sb * y, cos(b), 0.0f);
		}
	}

	// Link only 3 rings, for style sake...
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < SEGMENTS; ++j) {
			e[e_idx][0] = j * RINGS + i * (RINGS / 2);
			e[e_idx][1] = (j + 1) % SEGMENTS * RINGS + i * (RINGS / 2);
			++e_idx;
		}
	}

	assert(e_idx == sizeof(e) / 4);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(e), e, GL_STATIC_DRAW);

  len = 2 * e_idx;
  primitive_type = GL_LINES;
  has_colorbuf = false;
}

// based on http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
void Mesh::generate_icosphere()
{
	struct Builder {
		uint16_t e[480][2];
		uint16_t faces[320][3];
		Vector4 v[12 + 30 + 120];
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
			const float VERTS[12][3] =
			{
					{-1,  P, 0},
					{ 1,  P, 0},
					{-1, -P, 0},
					{ 1, -P, 0},

					{0, -1,  P},
					{0,  1,  P},
					{0, -1, -P},
					{0,  1, -P},

					{ P, 0, -1},
					{ P, 0,  1},
					{-P, 0, -1},
					{-P, 0,  1},
			};
			for(int i = 0; i < 12; ++i) {
				v[iv++] = Vector4(VERTS[i][0], VERTS[i][1], VERTS[i][2], 0.0f).normalized();
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
				face_subdivide(SUB, FACES[i][2], FACES[i][1], FACES[i][0]);
			}

		  assert(iv == sizeof(v) / sizeof(Vector4));
		  assert(ie == sizeof(e) / 4);
		  assert(ifaces == sizeof(faces) / 6);
		}

		uint16_t middle_vert(uint16_t a, uint16_t b)
		{
			if(a < b)
				swap(a, b);
			auto key = make_pair(a, b);
			auto outcome = vertices.insert(make_pair(key, iv));
			if(outcome.second) {
				v[iv++] = ((v[a] + v[b]) * 0.5).normalized();
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
				// Recursion base case:
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

  has_colorbuf = false;
}

void
Mesh::draw(Camera& c)
{
	const Shader *s = c.getShader();
	int stride;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	if(has_colorbuf) {
		stride = 32;
		glEnableVertexAttribArray(s->colorAttr());
		glVertexAttribPointer(s->colorAttr(), 4, GL_FLOAT, GL_FALSE, 32, (void*) (4 * sizeof(float)));
	}
	else {
		stride = 0;
	}

	glVertexAttribPointer(s->posAttr(), 4, GL_FLOAT, GL_FALSE, stride, NULL);

	glDrawElements(primitive_type, len, GL_UNSIGNED_SHORT, NULL);

	if(has_colorbuf)
		glDisableVertexAttribArray(s->colorAttr());
}

Mesh*
Mesh::get_mesh(Types type)
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
