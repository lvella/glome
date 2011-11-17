#pragma once

#include <boost/ref.hpp>

#include "4dmath.hpp"

enum MeshTypes {HUNTER, DESTROYER, UFO, ASTEROID, MESH_COUNT};

class Mesh
{
public:
  ~Mesh();

  void draw(const Matrix4& t);

  static Mesh* get_mesh(MeshTypes type);
  static void release_mesh(Mesh* m);

private:
  // a reference counter for shapes
  unsigned int ref_count;

  GLuint ibo, vbo;
  uint16_t ilen, vlen;

  Mesh(MeshTypes type);
};

