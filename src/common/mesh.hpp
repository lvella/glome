#pragma once

#include <stdio.h>

#include "shader.hpp"
#include "vector3.hpp"

class Mesh
{
public:
	enum class Type {HUNTER, DESTROYER, UFO, ICOSPHERE, UVSPHERE, MESH_COUNT};

	~Mesh();

	void draw(Camera& c);

	static Mesh* get_mesh(Type type);
	static void release_mesh(Mesh* m);

private:
	void load_from_file(const char* name);
	void generate_uvsphere();
	void generate_icosphere();

	struct VertexData {
		Vector3 pos;
		Vector4 color;
	};
	void fill_VBO(const std::vector<VertexData>& vdata, float scale=1.0f);

	// a reference counter for shapes
	unsigned int ref_count;

	union {
		struct {
			GLuint ibo, vbo;
		};
		GLuint bufobjs[2];
	};
	size_t len;
	GLenum primitive_type;
	bool has_colorbuf;

	Mesh(Type type);

	#ifdef STATS_TUNING
	std::vector<VertexData> mesh_data;
public:
	void rescale(float scale) {
		fill_VBO(mesh_data, scale);
	}
private:
	#endif
};

