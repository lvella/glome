#include <list>

#include "ship.hpp"
#include "randomcube.hpp"

#include "drawable.hpp"

static std::list<Drawable*> drawable_objs;
static GLuint tex_object;

void
Drawable::draw_in_minimap()
{
  std::list<Drawable*>::iterator it = drawable_objs.begin();
  for(; it != drawable_objs.end(); ++it)
  {
    glPushMatrix();
    (*it)->transformation().multToGL();

    glBindTexture(GL_TEXTURE_2D, tex_object);
    glColor3ub(255, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(-1, -1);
    glVertex2i(1, -1);
    glVertex2i(1, 1);
    glVertex2i(-1, 1);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}  

Drawable*
Drawable::create_ship()
{
  Ship* d = new Ship();
  drawable_objs.push_front(d);
  d->initialize();
  return d;
}

Drawable*
Drawable::create_random_cube()
{
  RandomCube* d = new RandomCube();
  drawable_objs.push_front(d);
  return d;
}

void
Drawable::update_all()
{
  std::list<Drawable*>::iterator it = drawable_objs.begin();
  for(; it != drawable_objs.end(); ++it)
  {
    if((*it)->dead())
      drawable_objs.erase(it);
    else
      (*it)->update();
  }
}

void
Drawable::draw_all()
{
  std::list<Drawable*>::iterator it = drawable_objs.begin();
  for(; it != drawable_objs.end(); ++it)
    (*it)->draw();
}

