#define GL_GLEXT_PROTOTYPES
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <SDL.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "ship.hpp"

using namespace std;

const int HEIGHT = 600;
const int WIDTH = 800;

double s[360];
double c[360];
double z[360];

// Rotation
double dx = 0, dy = 0;

// Speed
double speed = 0;

Ship ship;

void initialize_vars()
{
  for(int i = 0; i < 360; ++i)
    {
      double n = i * M_PI / 180;
      c[i] = cos(n);
      s[i] = sin(n);
      z[i] = 0.0;
    }
}

void initialize_shader()
{
  GLuint s, p;
  const char vcode[] = 
"void main() \
{ \
  vec4 tmp = gl_ModelViewMatrix * gl_Vertex; \
  tmp.xyz = tmp.xyz / (1.0 - tmp.w); \
  tmp.w = 1.0; \
  gl_Position = gl_ProjectionMatrix * tmp; \
  gl_FrontColor = gl_Color; \
  gl_FogFragCoord = length(gl_Position); \
}";

  const char *ptr;

  char err[10000];

  s = glCreateShader(GL_VERTEX_SHADER);
  ptr = vcode;
  glShaderSource(s, 1, &ptr,NULL);
  glCompileShader(s);

  {
    GLsizei length;
    glGetShaderInfoLog(s, 10000, &length, err);
    if(length)
      cout << "Shader compilation log:\n" << err << '\n';
  }

  p = glCreateProgram();
  glAttachShader(p,s);

  glLinkProgram(p);
  glUseProgram(p);
}

void draw_meridian(const double *a, const double *b, const double *c, const double *d)
{
  glBegin(GL_LINES);
  for(int i = 0; i < 360; ++i)
    {
      glVertex4d(a[i], b[i], c[i], d[i]);
    }
  glEnd();
}

void draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(.0f, .0f, 1.0f);
  draw_meridian(z, z, s, c);
  glColor3f(.0f, 1.f, .0f);
  draw_meridian(z, s, z, c);
  glColor3f(1.0f, .0f, .0f);
  draw_meridian(s, z, z, c);
  glColor3f(1.0f, 1.0f, .0f);
  draw_meridian(s, c, z, z);
  glColor3f(1.0f, .0f, 1.0f);
  draw_meridian(s, z, c, z);
  glColor3f(.0f, 1.0f, 1.0f);
  draw_meridian(z, s, c, z);

  ship.draw();

  SDL_GL_SwapBuffers();
}

void mul_mat_vec(const float *m, const float *v, float *r)
{
  for(int i = 0; i < 4; ++i)
    r[i] = m[i*4+0]*v[0] + m[i*4+1]*v[1] + m[i*4+2]*v[2] + m[i*4+3]*v[3];
}

void normalize_mat(float *f)
{
  for(int i = 0; i < 4; ++i)
    {
      float d = sqrt(f[i*4]*f[i*4]
		     + f[i*4+1]*f[i*4+1]
		     + f[i*4+2]*f[i*4+2]
		     + f[i*4+3]*f[i*4+3]);

      for(int j = 0; j < 4; ++j)
	f[i*4+j] /= d;
    }
}

void transpose_mat(const float *m0, float *m1)
{
  for(int i = 0; i < 4; ++i)
    for(int j = 0; j < 4; ++j)
      m1[j*4+i] = m0[i*4+j];
}

void printm(const float *m)
{
  for(int i = 0; i < 4; ++i)
      printf("%01.02f %01.02f %01.02f %01.02f\n",
	     m[i], m[4+i], m[8+i], m[12+i]);
  std::cout << std::endl;
}

void update()
{
  const float c = cos(speed);
  const float s = sin(speed);
  const float ahead[] = {1,0,0,0,  0,1,0,0,  0,0,c,-s,  0,0,s,c};

  float t[16], tt[16];
  float m1[16], m2[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, t);

  glLoadIdentity();
  glRotated(dx, 1, 0, 0);
  glRotated(dy, 0, 1, 0);
  glMultMatrixf(ahead);
  glMultMatrixf(t);
}

void mouse_motion(int x, int y)
{
  x -= WIDTH / 2;
  y -= HEIGHT / 2;

  dy = double(x) / double(WIDTH / 2);
  dx = double(y) / double(HEIGHT / 2);
}

void mouse_button(int button, int state)
{
  if(button == SDL_BUTTON_LEFT)
    {
      speed = (state == SDL_PRESSED) ? -0.01 : 0.0;
    }
}

void main_loop()
{
  const int FPS = 60;
  uint64_t frame_count = 0;
  bool running = true;
  Uint32 ticks = SDL_GetTicks();

  while(running) {
    // Treat events
    {
      SDL_Event e;
      while(SDL_PollEvent(&e)) {
	switch(e.type) {
	case SDL_QUIT:
	  running = false;
	  break;
	case SDL_MOUSEMOTION:
	  mouse_motion(e.motion.x, e.motion.y);
	  break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	  mouse_button(e.button.button, e.button.state);
	  break;
	}
      }
    }

    update();
    draw();

    // Fix framerate
    {
      // TODO: maybe clk_div is useful here...
      const int period = 1000/60;
      Uint32 now = SDL_GetTicks();
      int delay = period - int(now - ticks);
      if(delay > 0)
	SDL_Delay(delay);
      ticks = now;
    }
    ++frame_count;
  }

  cout << frame_count << " frames rendered." << endl;
}

int main(int argc, char **argv)
{
  // SDL startup
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    cerr << "Unable to initialize SDL: " << SDL_GetError() << '\n';
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL);
  SDL_WM_SetCaption("Navinha", NULL);

  // OpenGL nonchanging settings
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  //glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Fancy OpenGL fog
  glFogi(GL_FOG_MODE, GL_EXP);
  {
    const float fcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glFogfv(GL_FOG_COLOR, fcolor);
  }
  glFogf(GL_FOG_DENSITY, 0.55f);
  glHint(GL_FOG_HINT, GL_NICEST);
  glFogf(GL_FOG_END, M_PI);
  glFogf(GL_FOG_START, 2.0f);
  glEnable(GL_FOG);

  // 3D to 2D projection
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, double(WIDTH) / double(HEIGHT), 0.01, 5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // 4D to 3D projection
  initialize_shader();

  initialize_vars();

  Ship::initialize();

  main_loop();

  SDL_Quit();
}
