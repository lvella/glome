#define GL_GLEXT_PROTOTYPES
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>

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

  glGetShaderInfoLog(s, 10000, NULL, err);
  cout << "Shader compilation log:\n" << err << '\n';

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

void vertex_conv(float x, float y, float z)
{
  x /= 10;
  y /= 10;
  z /= 10;
  float d = 1+x*x+y*y+z*z;
  float n[4] = {2*x/d, 2*y/d, 2*z/d, (-1+x*x+y*y+z*z)/d};
  glVertex4fv(n);
}

void draw_ship()
{
  //float scale[] = {0.1,0,0,0,  0,0.1,0,0,  0,0,0.1,0,  0,0,0,0.1};

  /*glPushMatrix();
  glMultMatrix(scale);*/

  glColor3f(1.0, 1.0, 1.0);

  glBegin(GL_LINE_STRIP);
  vertex_conv(.0f, .0f, -.7f);
  vertex_conv(.0f, -.1f, .0f);
  vertex_conv(-.08f, .0f, .0f);
  vertex_conv(.08f, .0f, .0f);
  vertex_conv(.0f, -.1f, .0f);
  glEnd();
  glBegin(GL_LINE_STRIP);
  vertex_conv(-.08f, .0f, -.0f);
  vertex_conv(.0f, .0f, -.7f);
  vertex_conv(.08f, .0f, -.0f);
  glEnd();
  // Wings
  glBegin(GL_LINE_LOOP);
  vertex_conv(.0f, -.025f, -.04f);
  vertex_conv(.4f, -.03f, .01f);
  vertex_conv(.47f, 0.02f, 0.08f);
  vertex_conv(.42f, -.033f, -.11f);
  vertex_conv(.0f, -.0f, -.44f);
  vertex_conv(-.42f, -.033f, -.11f);
  vertex_conv(-.47f, 0.02f, 0.08f);
  vertex_conv(-.4f, -.03f, .01f);
  glEnd();
  glBegin(GL_LINES);
  vertex_conv(.4f, -.03f, .01f);
  vertex_conv(.42f, -.033f, -.11f);
  vertex_conv(-.4f, -.03f, .01f);
  vertex_conv(-.42f, -.033f, -.11f);
  glEnd();

  // Cannons
  glBegin(GL_LINE_STRIP);
  vertex_conv(.3f, -.03f, -.36f);
  vertex_conv(.3f, -.05f, -.1f);
  vertex_conv(.27f, -.03f, -.1f);
  vertex_conv(.33f, -.03f, -.1f);
  vertex_conv(.30f, -.05f, -.1f);
  glEnd();
  glBegin(GL_LINE_LOOP);
  vertex_conv(.3f, -.03f, -.36f);
  vertex_conv(.3f, -.05f, -.405f);
  vertex_conv(.3f, -.03f, -.45f);
  vertex_conv(.3f, -.01f, -.405f);
  vertex_conv(.3f, -.03f, -.36f);
  vertex_conv(.28f, -.03f, -.405f);
  vertex_conv(.3f, -.03f, -.45f);
  vertex_conv(.32f, -.03f, -.405f);
  glEnd();
  glBegin(GL_LINE_LOOP);
  vertex_conv(.3f, -.05f, -.405f);
  vertex_conv(.32f, -.03f, -.405f);
  vertex_conv(.3f, -.01f, -.405f);
  vertex_conv(.28f, -.03f, -.405f);
  glEnd();

  glBegin(GL_LINE_STRIP);
  vertex_conv(-.3f, -.03f, -.36f);
  vertex_conv(-.3f, -.05f, -.1f);
  vertex_conv(-.27f, -.03f, -.1f);
  vertex_conv(-.33f, -.03f, -.1f);
  vertex_conv(-.30f, -.05f, -.1f);
  glEnd();
  glBegin(GL_LINE_LOOP);
  vertex_conv(-.3f, -.03f, -.36f);
  vertex_conv(-.3f, -.05f, -.405f);
  vertex_conv(-.3f, -.03f, -.45f);
  vertex_conv(-.3f, -.01f, -.405f);
  vertex_conv(-.3f, -.03f, -.36f);
  vertex_conv(-.28f, -.03f, -.405f);
  vertex_conv(-.3f, -.03f, -.45f);
  vertex_conv(-.32f, -.03f, -.405f);
  glEnd();
  glBegin(GL_LINE_LOOP);
  vertex_conv(-.3f, -.05f, -.405f);
  vertex_conv(-.32f, -.03f, -.405f);
  vertex_conv(-.3f, -.01f, -.405f);
  vertex_conv(-.28f, -.03f, -.405f);
  glEnd();

  /*glPopMatrix();*/
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

  draw_ship();

  glutSwapBuffers();
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

void update(int nulo)
{
  glutTimerFunc(40, update, 0);

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

  glutPostRedisplay();
}

void mouse_motion(int x, int y)
{
  x -= WIDTH / 2;
  y -= HEIGHT / 2;

  dy = double(x) / double(WIDTH / 2);
  dx = double(y) / double(HEIGHT / 2);
}

void mouse_button(int button, int state, int x, int y)
{
  if(button == GLUT_LEFT_BUTTON)
    {
      speed = (state == GLUT_DOWN) ? -0.03 : 0.0;
    }
}

int main(int argc, char **argv)
{
  int window;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutInitWindowSize(WIDTH, HEIGHT);
  window = glutCreateWindow("Navinha");
  glutSetWindow(window);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Fancy fog
  {
    float fcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fcolor);
    glFogf(GL_FOG_DENSITY, 0.55f);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogf(GL_FOG_END, M_PI);
    glFogf(GL_FOG_START, 2.0f);
    glEnable(GL_FOG);
  }

  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, double(WIDTH) / double(HEIGHT), 0.01, 5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  initialize_shader();
  initialize_vars();

  glutDisplayFunc(draw);
  glutMotionFunc(mouse_motion);
  glutPassiveMotionFunc(mouse_motion);
  glutMouseFunc(mouse_button);
  update(0);

  glutMainLoop();
}
