#include <cstdio>
#include <sstream>

#include "engine.hpp"
#include "config.hpp"
#include "fire.hpp"

extern const char* mesh_filename[MESH_COUNT];

Engine::Engine(MeshTypes type)
{
  load_location(type);
  FX_engine = new Fire(100, velocity);
}

Engine::~Engine()
{
	delete FX_engine;
}

void 
Engine::load_location(MeshTypes type)
{
  int ret;
  FILE *fd;

  const char* name = mesh_filename[int(type)];
  // Load .gun file
  {
		unsigned int engine_pos;
    std::stringstream dir;
    dir << DATA_DIR << "/models/" << name << ".wire";
    fd = fopen(dir.str().c_str(), "rb");
    /* Read header of file */
		fseek(fd, 8, SEEK_SET);
		fread(&engine_pos, sizeof(unsigned int), 1, fd);
		/* Poiter file to engine position */
    fseek(fd, engine_pos, SEEK_SET);
    assert(fd != NULL);
  }

  {
    // Reading Engine Matrix
    ret = fread(&nengines, sizeof(nengines), 1, fd);
    assert(ret == 1);

    ret = fread(&velocity[0][0], sizeof(float), 16, fd);
    assert (ret == 16);
  }
  fclose(fd);
}

void Engine::draw(const Shader& cam)
{}

void Engine::draw(Matrix4 cam, Matrix4 proj)
{
  FX_engine->draw(cam,proj);
}
