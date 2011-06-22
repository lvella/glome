#pragma once

#include <boost/ref.hpp>

#include "4dmath.hpp"

enum ShipMesh {HUNTER, DESTROYER, UFO, SHIPMESH_COUNT};

class Mesh
{
public:
  ~Mesh();

  void draw(const Matrix4& t);
  inline const Matrix4& get_rcanon() { return r_canon; }
  inline const Matrix4& get_lcanon() { return l_canon; }
  static Mesh* get_mesh(ShipMesh type);
  static void release_mesh(Mesh* m);

private:
  // a reference counter for shapes
  unsigned int ref_count;

  GLuint bufobjs[2];
  int dlist;

  Matrix4 r_canon, l_canon;
  uint16_t nguns;

  Mesh(ShipMesh type);
};

