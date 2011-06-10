#include "drawable.hpp"

Drawable::~Drawable()
{}

void
Drawable::draw_in_minimap()
{
    glPushMatrix();
    t.multToGL();

    glColor3ub(255, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(-1, -1);
    glVertex2i(1, -1);
    glVertex2i(1, 1);
    glVertex2i(-1, 1);
    glEnd();
    glPopMatrix();
}
