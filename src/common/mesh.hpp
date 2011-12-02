#pragma once

#include <stdio.h>

#include <boost/ref.hpp>

#include "shader.hpp"

enum MeshTypes {HUNTER, DESTROYER, UFO, MESH_COUNT};

class Mesh
{
public:
  ~Mesh();

  void draw(const Shader& s);

  fpos_t get_current_pos() {
    return current_pos_instream;
  }
  void set_current_pos(fpos_t cpos) {
    current_pos_instream = cpos;
  }

  static Mesh* get_mesh(MeshTypes type);
  static void release_mesh(Mesh* m);

private:
  // a reference counter for shapes
  unsigned int ref_count;
  
  /* File position indicator associated with stream to be used to load guns, engines, etc. */
  fpos_t current_pos_instream;

  union {
  	struct {
  		GLuint ibo, vbo;
  	};
  	GLuint bufobjs[2];
  };
  size_t len;

  Mesh(MeshTypes type);
};

