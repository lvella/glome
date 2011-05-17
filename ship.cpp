#define GL_GLEXT_PROTOTYPES
#include <map>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdint.h>

#include "4dmath.hpp"
#include "ship.hpp"
#include "projectile.hpp"

using namespace std;

static const float vdesc[][3] = {
  //GL_LINE_STRIP
  {.0f, .0f, -.7f},
  {.0f, -.1f, .0f},
  {-.08f, .0f, .0f},
  {.08f, .0f, .0f},
  {.0f, -.1f, .0f}, // 5

  //GL_LINE_STRIP
  {-.08f, .0f, .0f},
  {.0f, .0f, -.7f},
  {.08f, .0f, .0f}, // 3

  // Wings
  //GL_LINE_LOOP
  {.0f, -.025f, -.04f},
  {.4f, -.03f, .01f},
  {.47f, 0.02f, 0.08f},
  {.42f, -.033f, -.11f},
  {.0f, -.0f, -.44f},
  {-.42f, -.033f, -.11f},
  {-.47f, 0.02f, 0.08f},
  {-.4f, -.03f, .01f}, // 8

  //GL_LINES
  {.4f, -.03f, .01f},
  {.42f, -.033f, -.11f},
  {-.4f, -.03f, .01f},
  {-.42f, -.033f, -.11f}, // 4
  
  // Cannons
  //GL_LINE_STRIP
  {.3f, -.03f, -.36f},
  {.3f, -.05f, -.1f},
  {.27f, -.03f, -.1f},
  {.33f, -.03f, -.1f},
  {.30f, -.05f, -.1f}, // 5

  //GL_LINE_LOOP
  {.3f, -.03f, -.36f},
  {.3f, -.05f, -.405f},
  {.3f, -.03f, -.45f},
  {.3f, -.01f, -.405f},
  {.3f, -.03f, -.36f},
  {.28f, -.03f, -.405f},
  {.3f, -.03f, -.45f},
  {.32f, -.03f, -.405f}, // 8

  //GL_LINE_LOOP
  {.3f, -.05f, -.405f},
  {.32f, -.03f, -.405f},
  {.3f, -.01f, -.405f},
  {.28f, -.03f, -.405f}, // 4

  //GL_LINE_STRIP
  {-.3f, -.03f, -.36f},
  {-.3f, -.05f, -.1f},
  {-.27f, -.03f, -.1f},
  {-.33f, -.03f, -.1f},
  {-.30f, -.05f, -.1f}, // 5

  //GL_LINE_LOOP
  {-.3f, -.03f, -.36f},
  {-.3f, -.05f, -.405f},
  {-.3f, -.03f, -.45f},
  {-.3f, -.01f, -.405f},
  {-.3f, -.03f, -.36f},
  {-.28f, -.03f, -.405f},
  {-.3f, -.03f, -.45f},
  {-.32f, -.03f, -.405f}, // 8

  //GL_LINE_LOOP
  {-.3f, -.05f, -.405f},
  {-.32f, -.03f, -.405f},
  {-.3f, -.01f, -.405f},
  {-.28f, -.03f, -.405f}}; // 4

struct v3 {
  v3(const float *val) {
    v[0] = val[0];
    v[1] = val[1];
    v[2] = val[2];
  }
  bool operator<(const v3 &o) const {
    return v[0] < o.v[0]
      || (v[0] == o.v[0]
	  && (v[1] < o.v[1]
	      || (v[1] == o.v[1] && v[2] < o.v[2])));
  }
  float v[3];
};

static int dlist;

static void vertex_conv(const float *in, float scale, float *out)
{
  float x = in[0] * scale;
  float y = in[1] * scale;
  float z = in[2] * scale;
  float d = 1 + x*x + y*y + z*z;

  out[0] = 2*x/d;
  out[1] = 2*y/d;
  out[2] = 2*z/d;
  out[3] = (-1 + x*x + y*y + z*z) / d;
}

void Ship::initialize()
{
  const uint16_t ilen = sizeof(vdesc) / sizeof(float[3]);
  uint16_t vlen = 0;
  GLuint vbo;
  GLuint ibo;

  // Create index buffer
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ilen * sizeof(uint16_t), NULL, GL_STATIC_DRAW);
  uint16_t *idata = (uint16_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);

  // Remove redundant vertices while filling the index buffer
  typedef map<v3, uint16_t> vmap;
  typedef vmap::iterator iter;
  vmap m;
  for(int i = 0; i < ilen; ++i) {
    pair<iter, bool> res = m.insert(vmap::value_type(vdesc[i], vlen));
    idata[i] = res.first->second;
    if(res.second)
      ++vlen;
  }
  assert(vlen == m.size());

  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  idata = NULL;

  // Create vertex buffer
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vlen * 4 * sizeof(float), NULL, GL_STATIC_DRAW);
  float *vdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  // Convert to 4D coordinates and fill the buffer
  for(iter i = m.begin(); i != m.end(); ++i) {
    vertex_conv(i->first.v, 0.01f, &vdata[i->second * 4]);
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);
  vdata = NULL;

  // Create the display list
  dlist = glGenLists (1);
  glNewList(dlist, GL_COMPILE);
  {
    uint16_t *offset = NULL;
    glColor3ub(255, 255, 255);
    glVertexPointer(4, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawRangeElements(GL_LINE_STRIP, 0, ilen-1, 5, GL_UNSIGNED_SHORT, offset);
    glDrawRangeElements(GL_LINE_STRIP, 0, ilen-1, 3, GL_UNSIGNED_SHORT, offset+=5);
    glDrawRangeElements(GL_LINE_LOOP, 0, ilen-1, 8, GL_UNSIGNED_SHORT, offset+=3);
    glDrawRangeElements(GL_LINES, 0, ilen-1, 4, GL_UNSIGNED_SHORT, offset+=8);
    glDrawRangeElements(GL_LINE_STRIP, 0, ilen-1, 5, GL_UNSIGNED_SHORT, offset+=4);
    glDrawRangeElements(GL_LINE_LOOP, 0, ilen-1, 8, GL_UNSIGNED_SHORT, offset+=5);
    glDrawRangeElements(GL_LINE_LOOP, 0, ilen-1, 4, GL_UNSIGNED_SHORT, offset+=8);
    glDrawRangeElements(GL_LINE_STRIP, 0, ilen-1, 5, GL_UNSIGNED_SHORT, offset+=4);
    glDrawRangeElements(GL_LINE_LOOP, 0, ilen-1, 8, GL_UNSIGNED_SHORT, offset+=5);
    glDrawRangeElements(GL_LINE_LOOP, 0, ilen-1, 4, GL_UNSIGNED_SHORT, offset+=8);
  }
  glEndList();
}

Ship::Ship():
    Object(Matrix4::IDENTITY),
    v_tilt(0.0f),
    h_tilt(0.0f),
    speed(0.0f),
    speed_v(0.0f),
    speed_h(0.0f),
    speed_s(0.0f),
    sps(13),
    last_shot(SDL_GetTicks())
{}

void Ship::draw()
{
  glPushMatrix();
  t.multToGL();
  glCallList(dlist);
  glPopMatrix();

}

void Ship::update()
{
  /* Maximum turning delta per frame in radians. */
  const float MAXR = 0.03;

  /* Maximum speed, in radians per second. */
  const float MAXS = 0.0015;
  const float MAXS_V = 0.0004;
  const float MAXS_H = 0.0004;
  const float MAXS_S = 0.02;

  float accel_v = 0.f, accel_h = 0.f, accel_s = 0.f;

  /* Turning */
  float h = h_tilt - h_req;
  float v = v_tilt - v_req;

  /* Limit the turning speed to MAXD rads per frame. */
  if(h > MAXR)
    h = MAXR;
  else if(h < -MAXR)
    h = -MAXR;
  if(v > MAXR)
    v = MAXR;
  else if(v < -MAXR)
    v = -MAXR;

  h_tilt -= h;
  v_tilt -= v;

  /* Handle input commands */
  handle_commands(up, down, speed_v, accel_v, 0.00004);
  handle_commands(right, left, speed_h, accel_h, 0.00004);
  handle_commands(spinl, spinr, speed_s, accel_s, 0.002);

  /* Accelerating */
  accelerate(speed, accel, MAXS);
  accelerate(speed_v, accel_v, MAXS_V);
  accelerate(speed_h, accel_h, MAXS_H);
  accelerate(speed_s, accel_s, MAXS_S);

  /* Shooting */
  if(sh)
    this->do_shot();

  t = t * zw_matrix(speed) * yw_matrix(speed_v) * xw_matrix(speed_h) * xy_matrix(speed_s) * yz_matrix(v_tilt) * rotation(-h_tilt, 0.0, M_SQRT2/2.0, M_SQRT2/2.0);
}

void
Ship::accelerate(float& speed, float& accel, float max)
{
  speed += accel;
  if(speed > max)
    speed = max;
  else if(speed < -max)
    speed = -max;
}

void
Ship::handle_commands(bool k0, bool k1, float& speed, float& accel, float max_accel)
{
  if(k0 || k1)
  {
    accel = max_accel;
    if(k1)
      accel = -accel;
  }
  else
  {
    if(speed > 0.f)
    {
      accel = -max_accel;
      if((speed + accel) < 0.f)
      {
        speed = 0.f;
        accel = 0.f;
      }
    }
    else if(speed < 0.f)
    {
      accel = max_accel;
      if((speed + accel) > 0.f)
      {
        speed = 0.f;
        accel = 0.f;
      }
    }
  }
}

void Ship::do_shot()
{
  const int p = 1000 / sps;
  Uint32 now = SDL_GetTicks();
  if(int(now - last_shot) >= p)
  {
    Projectile::shot(t, zw_matrix(-0.05 + speed));
    last_shot = now;
  }
}
