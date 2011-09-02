#include <iostream>
#include "4dmath.hpp"
#include "asteroid.hpp"

using namespace std;

Asteroid::Asteroid():
    Drawable(Matrix4::IDENTITY)
{
  mesh = Mesh::get_mesh(ASTEROID);
}

Asteroid::~Asteroid()
{
  Mesh::release_mesh(mesh);
}

void Asteroid::draw()
{
  mesh->draw(t);
}

void Asteroid::update()
{
}

