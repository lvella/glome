#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>

#ifdef WIN32
#include <windows.h>
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include "init_gl.hpp"

using namespace std;

void init_gl()
{
  // Using GLEW to get the OpenGL functions
  GLenum err = glewInit();
  if(err != GLEW_OK) {
    cerr << "Unable to initialize GLEW:\n"
   << glewGetErrorString(err) << endl;
    exit(1);
  }

  if(! GLEW_VERSION_2_1)
  {
    const char *msg = "Navigna requires at least OpenGL 2.1";
#ifdef WIN32
    MessageBoxA(NULL, msg, NULL, MB_OK);
#else
    cerr << msg << endl;
#endif
    exit(1);
  }

#ifdef WIN32
  if(WGLEW_EXT_swap_control){
    wglSwapIntervalEXT(1);
  }
#else
  if(GLXEW_SGI_swap_control){
    glXSwapIntervalSGI(1);
  }
#endif 
}
