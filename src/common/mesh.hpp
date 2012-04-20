#pragma once

#include <stdio.h>
#include <boost/ref.hpp>

#include "shader.hpp"

class Mesh
{
public:
	enum Types {HUNTER, DESTROYER, UFO, ICOSPHERE, UVSPHERE, MESH_COUNT};

  ~Mesh();

  void draw(Camera& c);

  static Mesh* get_mesh(Types type);
  static void release_mesh(Mesh* m);

private:
  void load_from_file(const char* name);
  void generate_uvsphere();
  void generate_icosphere();

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

  Mesh(Types type);
};

