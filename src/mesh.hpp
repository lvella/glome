#pragma once

#include <boost/ref.hpp>

#include "4dmath.hpp"

enum ShipMesh {HUNTER, DESTROYER, UFO, SHIPMESH_COUNT};

class Mesh
{
public:
  ~Mesh();

  void draw(const Matrix4& t);

  static Mesh* get_mesh(ShipMesh type);
  static void release_mesh(Mesh* m);

private:
  // a reference counter for shapes
  unsigned int ref_count;

  GLuint bufobjs[2];
  int dlist;

  Mesh(ShipMesh type);
};

