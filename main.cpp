#define GL_GLEXT_PROTOTYPES
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <deque>
#include <SDL.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <png.h>

#include "4dmath.hpp"
#include "input.hpp"
#include "matrix4.hpp"
#include "minimap.hpp"
#include "ship.hpp"
#include "projectile.hpp"

using namespace std;

const int HEIGHT = 600;
const int WIDTH = 800;

double s[360];
double c[360];
double z[360];

// Camera transform historic
std::deque<Matrix4> cam_hist(10, Matrix4::IDENTITY);

Ship ship;
Input input(&ship);
MiniMap minimap;

GLuint program;
GLuint tex_minimap;

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
  minimap.draw();

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
  unsigned char texture[256 * 256];
  cx = cy = 127.5;
  tex_h = tex_w = 256;
  tex_r = 128;
  tex_r_lim = 113;

  for(i = 0; i < tex_h; ++i)
  {
    for(j = 0; j < tex_w; ++j)
    {
      d = sqrt(((i - cx) * (i - cx)) + ((j - cy) * (j - cy)));
      texture[(i * tex_w) + j] = (d > tex_r) ? 0 : ((d < tex_r_lim) ? 142 : ((x *= (x = ((tex_r - d) / (tex_r - tex_r_lim)))) * 142));
    }
    // (((tex_r - d) / (tex_r - tex_r_lim)) * ((tex_r - d) / (tex_r - tex_r_lim)))
  }

  glGenTextures(1, &tex_minimap);
  glBindTexture(GL_TEXTURE_2D, tex_minimap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tex_w, tex_h, 0,
  GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

/*
void
load_textures()
{
    //header for testing if it is a png
   png_byte header[8];
 
   //open file as binary
   FILE *fp = fopen("map160.png", "rb");
   if (!fp) {
     exit(1);
   }
 
   //read the header
   fread(header, 1, 8, fp);
 
   //test if png
   int is_png = !png_sig_cmp(header, 0, 8);
   if (!is_png) {
     fclose(fp);
     exit(1);
   }
 
   //create png struct
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
       NULL, NULL);
   if (!png_ptr) {
     fclose(fp);
     exit(1);
   }
 
   //create png info struct
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
     png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
     fclose(fp);
     exit(1);
   }
 
   //create png info struct
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
     fclose(fp);
     exit(1);
   }
 
   //png error stuff, not sure libpng man suggests this.
   if (setjmp(png_jmpbuf(png_ptr))) {
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     exit(1);
   }
 
   //init png reading
   png_init_io(png_ptr, fp);
 
   //let libpng know you already read the first 8 bytes
   png_set_sig_bytes(png_ptr, 8);
 
   // read all the info up to the image data
   png_read_info(png_ptr, info_ptr);
 
   //variables to pass to get info
   int bit_depth, color_type;
   png_uint_32 twidth, theight;
 
   // get info about png
   png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
       NULL, NULL, NULL);
 
   //update width and height based on png info
   int width = 160;
   int height = 160;

   width = twidth;
   height = theight;
 
   // Update the png info struct.
   png_read_update_info(png_ptr, info_ptr);
 
   // Row size in bytes.
   int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
 
   // Allocate the image_data as a big block, to be given to opengl
   png_byte *image_data = new png_byte[rowbytes * height];
   if (!image_data) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     exit(1);
   }
 
   //row_pointers is for pointing to image_data for reading the png with libpng
   png_bytep *row_pointers = new png_bytep[height];
   if (!row_pointers) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     exit(1);
   }
   // set the individual row_pointers to point at the correct offsets of image_data
   for (int i = 0; i < height; ++i)
     row_pointers[height - 1 - i] = image_data + i * rowbytes;
 
   //read the png into image_data through row_pointers
   png_read_image(png_ptr, row_pointers);
 
   //Now generate the OpenGL texture object
   glGenTextures(1, &tex_2d);
   glBindTexture(GL_TEXTURE_2D, tex_2d);
   glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,
       GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 
   //clean up memory and close stuff
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   delete[] image_data;
   delete[] row_pointers;
   fclose(fp);
}
*/

int main(int argc, char **argv)
{
  // SDL startup
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    cerr << "Unable to initialize SDL: " << SDL_GetError() << '\n';
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL);
  SDL_WM_SetCaption("Navigna", NULL);

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
  gluPerspective(45, double(WIDTH) / double(HEIGHT), 0.001, 5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // 4D to 3D projection
  initialize_shader();

  initialize_vars();

  load_textures();

  Ship::initialize();

  main_loop();

  SDL_Quit();
}

