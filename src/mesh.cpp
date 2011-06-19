#include <stdio.h>
#include <GL/glew.h>
#include <iostream>
#include <sstream>

#include "mesh.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;

static Mesh* mesh_list[SHIPMESH_COUNT] = {NULL};

Mesh::Mesh(ShipMesh type):
  ref_count(1)
{
  int ret;
  FILE *fd;

  switch(type)
  {
    case HUNTER:
      name = "hunter";
      break;
    case DESTROYER:
      name = "destroyer";
      break;
    case UFO:
      name = "ufo";
      break;
  }

  std::cout << "Create new ship named: " << name << " in constructor Mesh." << std::endl;

  // Load mesh file and put it into the list of shapes if was not exist
  {
    std::stringstream dir;
    dir << DATA_DIR << "/" << name << ".wire";
    path = dir.str();
    fd = fopen(dir.str().c_str(), "rb");
    assert(fd != NULL);
  }

  {
    // Reading Guns Matrix
    ret = fread(&nguns, sizeof(nguns), 1, fd);
    assert(ret == 1);

    ret = fread(&l_canon[0][0], sizeof(float), 16, fd);
    assert (ret == 16);

    ret = fread(&r_canon[0][0], sizeof(float), 16, fd);
    assert (ret == 16);
  }

  {
    // Reading 4-D vertex coordinates
    ret = fread(&vlen, sizeof(vlen), 1, fd);
    assert(ret == 1);
    // Create vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vlen * 4 * sizeof(float), NULL, GL_STATIC_DRAW);
    float *vdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    ret = fread(vdata, sizeof(float) * 4, vlen, fd);
    assert(ret == vlen);
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  {
    // Reading 4-D edges coordinates
    ret = fread(&ilen, sizeof(ilen), 1, fd);
    assert(ret == 1);
    // Create index buffer
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ilen * sizeof(uint16_t) * 2, NULL, GL_STATIC_DRAW);
    uint16_t *idata = (uint16_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    ret = fread(idata, sizeof(uint16_t) * 2, ilen, fd);
    assert(ret == ilen);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }

  fclose(fd);

  // Create the display list
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glColor3ub(80, 80, 80);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexPointer(4, GL_FLOAT, 0, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawRangeElements(GL_LINES, 0, vlen-1, ilen*2, GL_UNSIGNED_SHORT, NULL);
  glEndList();
}

void
Mesh::draw(const Matrix4& t)
{
  glPushMatrix();
  t.multToGL();
  glCallList(dlist);
  glPopMatrix();
}

Mesh*
Mesh::get_mesh(ShipMesh type)
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
