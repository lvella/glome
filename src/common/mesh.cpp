#include <GL/glew.h>
#include <iostream>
#include <sstream>

#include "world.hpp"
#include "mesh.hpp"
#include "config.hpp"

using namespace std;

static Mesh* mesh_list[MESH_COUNT] = {NULL};

const char* mesh_filename[MESH_COUNT] =
  {
    "hunter",
    "destroyer",
    "ufo"
  };

Mesh::~Mesh()
{
  glDeleteBuffers(2, bufobjs);
}

Mesh::Mesh(MeshTypes type):
  ref_count(1)
{
  uint16_t ilen, vlen;

  int ret;
  FILE *fd;

  const char* name = mesh_filename[int(type)];

  std::cout << "Loading new mesh named " << name << '.' << std::endl;

  // Load mesh file and put it into the list of meshs if was not exist
  {
		unsigned int mesh_pos;
    std::stringstream dir;
    dir << DATA_DIR << "/models/" << name << ".wire";
    fd = fopen(dir.str().c_str(), "rb");
    /* Read header of file */
		fread(&mesh_pos, sizeof(unsigned int), 1, fd);
		/* Poiter file to mesh position */
    fseek(fd, mesh_pos, SEEK_SET);
    assert(fd != NULL);
  }

  glGenBuffers(2, bufobjs);
  {
    // Reading 4-D vertex coordinates(16bytes) and colorRGBA values(16bytes)
    // format: <x, y, z, w> <r, g, b, a>
    ret = fread(&vlen, sizeof(vlen), 1, fd);
    assert(ret == 1);
    // Create vertex buffer
    uint16_t vbolen = vlen * 2 * 4 * sizeof(float);
    float vdata[vbolen];
    ret = fread(vdata, 2 * 4 * sizeof(float), vlen, fd);
    assert(ret == vlen);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vbolen, vdata, GL_STATIC_DRAW);
  }

  {
    // Reading 4-D edges coordinates (8bytes)
    // format:  <v_index0 , v_index1>
    ret = fread(&ilen, sizeof(ilen), 1, fd);
    assert(ret == 1);
    // Create index buffer
    uint16_t ibolen = ilen * 2 * sizeof(uint16_t);
    uint16_t idata[ibolen];
    ret = fread(idata, 2 * sizeof(uint16_t), ilen, fd);
    assert(ret == ilen);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibolen, idata, GL_STATIC_DRAW);
  }

  len = ilen * 2;

  fclose(fd);
}

void
Mesh::draw(const Shader& s)
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glEnableVertexAttribArray(s.colorAttr());

  glVertexAttribPointer(s.posAttr(), 4, GL_FLOAT, GL_FALSE, 32, NULL);
  glVertexAttribPointer(s.colorAttr(), 4, GL_FLOAT, GL_FALSE, 32, (void*) (4 * sizeof(float)));

  glDrawElements(GL_LINES, len, GL_UNSIGNED_SHORT, NULL);
  glDisableVertexAttribArray(s.colorAttr());
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
