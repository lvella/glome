#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <deque>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

#include "4dmath.hpp"
#include "input.hpp"
#include "matrix4.hpp"
#include "minimap.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "randomcube.hpp"

using namespace std;

extern const int HEIGHT = 600;
extern const int WIDTH = 800;

extern const float FOV = 45.0f;

double s[360];
double c[360];
double z[360];

// Camera transform historic
std::deque<Matrix4> cam_hist(10, Matrix4::IDENTITY);

Ship ship;
Input input(&ship);
MiniMap minimap;
RandomCube cube;

GLuint program;
GLuint tex_ship;

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
  GLuint shader;
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

  shader = glCreateShader(GL_VERTEX_SHADER);
  ptr = vcode;
  glShaderSource(shader, 1, &ptr,NULL);
  glCompileShader(shader);

  {
    GLsizei length;
    glGetShaderInfoLog(shader, 10000, &length, err);
    if(length)
      cout << "Shader compilation log:\n" << err << '\n';
  }

  program = glCreateProgram();
  glAttachShader(program, shader);

  glLinkProgram(program);
  glUseProgram(program);
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
  const Matrix4 offset(zw_matrix(-0.015) * yw_matrix(-0.005));

  // Camera transform
  (offset * cam_hist.front()).loadToGL();
  cam_hist.pop_front();
  cam_hist.push_back(ship.transformation().transpose());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  {
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
    Projectile::draw_all();
    cube.draw();
  }

  // Enable 2D
  // TODO: Investigate why fog is disabled and the lines become
  // thiner if texture is enabled on ATI Catalyst. Shader's fault?
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0f, 800, 600, 0.0f, -1.0f, 1.0f);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glUseProgram(0);

  minimap.draw();

  // Disable 2D
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glUseProgram(program);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  {
//    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glViewport(10, HEIGHT-170, 160, 160);
    glOrtho(-2, 2, -2, 2, -1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);

    Matrix4 t = (yz_matrix(M_PI/2) * cam_hist.back());
    t.loadToGL();

    //ship.draw();
    glPushMatrix();
    Matrix4 ts = t * ship.transformation();
    ship.transformation().multToGL();
    ts = ts.transpose();
    Vector4 v0(0.00007, -0.00007, 0, 0.9996);
    Vector4 v1(-0.00007, -0.00007, 0, 0.9996);
    Vector4 v2(-0.00007, 0.00007, 0, 0.9996);
    Vector4 v3(0.00007, 0.00007, 0, 0.9996);
    glBindTexture(GL_TEXTURE_2D, tex_ship);
    glColor3ub(255, 0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    Vector4 v(ts * v0);
    glVertex4f(v.x, v.y, v.z, v.w);
    glTexCoord2f(1, 0);
    v = ts * v1;
    glVertex4f(v.x, v.y, v.z, v.w);
    glTexCoord2f(1, 1);
    v = ts * v2;
    glVertex4f(v.x, v.y, v.z, v.w);
    glTexCoord2f(0, 1);
    v = ts * v3;
    glVertex4f(v.x, v.y, v.z, v.w);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
  //  glDisable(GL_TEXTURE_2D);

    Projectile::draw_all();

    //cube.draw();

    glPushMatrix();
    Matrix4 tc = t * cube.transformation();
    cube.transformation().multToGL();
    tc = tc.transpose();
    float a = 0.0000008;
    float w = sqrt(1 - a * a);
    Vector4 v4(a, -a, 0, w);
    Vector4 v5(-a, -a, 0, w);
    Vector4 v6(-a, a, 0, w);
    Vector4 v7(a, a, 0, w);
    glBindTexture(GL_TEXTURE_2D, tex_ship);
    glColor3ub(0, 0, 255);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    v = tc * v4;
    glVertex4f(v.x, v.y, v.z, v.w);
    glTexCoord2f(1, 0);
    v = tc * v5;
    glVertex4f(v.x, v.y, v.z, v.w);
    glTexCoord2f(1, 1);
    v = tc * v6;
    glVertex4f(v.x, v.y, v.z, v.w);
    glTexCoord2f(0, 1);
    v = tc * v7;
    glVertex4f(v.x, v.y, v.z, v.w);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);


    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, WIDTH, HEIGHT);    
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }

  SDL_GL_SwapBuffers();
}

void update()
{
  ship.update();
  Projectile::update_all();
  minimap.update();
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
      input.handle(running);
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

void
load_textures()
{
  int i, j, tex_h, tex_w;
  float cx, cy, d, tex_r, tex_r_lim, x;
  unsigned char texture[16 * 16];
  cx = cy = 7.5;
  tex_h = tex_w = 16;
  tex_r = 8;
  tex_r_lim = 12;

  for(i = 0; i < tex_h; ++i)
  {
    for(j = 0; j < tex_w; ++j)
    {
      d = sqrt(((i - cx) * (i - cx)) + ((j - cy) * (j - cy)));
      texture[(i * tex_w) + j] = (d > tex_r) ? 0 : ((d < tex_r_lim) ? 255 : ((x *= (x = ((tex_r - d) / (tex_r - tex_r_lim)))) * 255));
    }
  }

  glGenTextures(1, &tex_ship);
  glBindTexture(GL_TEXTURE_2D, tex_ship);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tex_w, tex_h, 0,
  GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
}

int main(int argc, char **argv)
{
  // SDL startup
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
    return 1;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL);
  SDL_WM_SetCaption("Navigna", NULL);
  SDL_ShowCursor(SDL_DISABLE);

  // Using GLEW to get the OpenGL functions
  {
    GLenum err = glewInit();
    if(err != GLEW_OK) {
      cerr << "Unable to initialize GLEW: %s\n"
	   << glewGetErrorString(err) << endl;
      return 1;
    }
  }

  // OpenGL nonchanging settings
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glEnableClientState(GL_VERTEX_ARRAY);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glLineWidth(1.5f);

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
  gluPerspective(FOV, double(WIDTH) / double(HEIGHT), 0.001, 5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // 4D to 3D projection
  initialize_shader();

  initialize_vars();

  load_textures();

  minimap.load_texture();

  Ship::initialize();

  main_loop();

  SDL_Quit();
}

