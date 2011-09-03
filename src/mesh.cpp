#include <stdio.h>
#include <GL/glew.h>
#include <iostream>
#include <sstream>

#include "mesh.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;

static Mesh* mesh_list[MESH_COUNT] = {NULL};
const char* mesh_filename[MESH_COUNT] =
    {
        "hunter",
        "destroyer",
        "ufo",
        "asteroid"
    };

Mesh::~Mesh()
{
  glDeleteLists(dlist, 1);
  glDeleteBuffers(2, bufobjs);
}

Mesh::Mesh(MeshTypes type):
  ref_count(1)
{
  GLuint ibo;
  GLuint vbo;

  uint16_t ilen;
  uint16_t vlen;

  int ret;
  FILE *fd;

  const char* name = mesh_filename[int(type)];

  std::cout << "Loading new mesh named " << name << '.' << std::endl;

  // Load mesh file and put it into the list of meshs if was not exist
  {
    std::stringstream dir;
    dir << DATA_DIR << "/" << name << ".wire";
    fd = fopen(dir.str().c_str(), "rb");
    assert(fd != NULL);
  }

  glGenBuffers(2, bufobjs);
  vbo = bufobjs[0];
  ibo = bufobjs[1];

  {
    // Reading 4-D vertex coordinates
    ret = fread(&vlen, sizeof(vlen), 1, fd);
    assert(ret == 1);
    // Create vertex buffer
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
Mesh::get_mesh(MeshTypes type)
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
