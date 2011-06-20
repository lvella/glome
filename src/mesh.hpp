#pragma once

#include <string>
#include <boost/ref.hpp>

#include "4dmath.hpp"

enum ShipMesh {HUNTER, DESTROYER, UFO, SHIPMESH_COUNT};

class Mesh
{
public:
  void draw(const Matrix4& t);
  inline std::string get_name() { return name; }
  inline const Matrix4& get_rcanon() { return r_canon; }
  inline const Matrix4& get_lcanon() { return l_canon; }
  static Mesh* get_mesh(ShipMesh type);
  static void release_mesh(Mesh* m);

private:
  // a reference counter for shapes
  unsigned int ref_count;

  std::string name;
  std::string path;

  GLuint vbo;
  GLuint ibo;
  uint16_t ilen;
  uint16_t vlen;
  uint16_t nguns;

  int dlist;
  Matrix4 r_canon, l_canon;

  Mesh(ShipMesh type);
};

