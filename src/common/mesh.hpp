#pragma once

#include <cstdio>
#include <memory>

#include "shader.hpp"
#include "vector3.hpp"
#include "camera.hpp"

class Mesh
{
public:
	enum class Type {HUNTER, DESTROYER, UFO, ICOSPHERE, UVSPHERE, MESH_COUNT};

	~Mesh();

	float get_radius()
	{
		return radius;
	}

	void draw(Camera& c);

	static std::shared_ptr<Mesh> get_mesh(Type type);

private:
	void load_from_file(const char* name);
	void generate_uvsphere();
	void generate_icosphere();

	struct VertexData {
		Vector3 pos;
		Vector4 color;
	};
	void fill_VBO(const std::vector<VertexData>& vdata, float scale=1.0f);

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

	float radius;

#ifdef STATS_TUNING
	std::vector<VertexData> mesh_data;
public:
	void rescale(float scale) {
		fill_VBO(mesh_data, scale);
	}
private:
	#endif
};

