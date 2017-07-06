#ifndef XWIN_H
#define XWIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <X11/keysym.h>
//#include <GL/gl.h>
#include <GL/glx.h>
//#include <GL/glu.h>
#include "log.h"
#include "fonts.h"
#include "xwin.h"

extern Display *dpy;
extern Window win;
extern GLXContext glc;
extern int xres, yres;
extern void cleanupXWindows(void);
extern void set_title(void);
extern void setup_screen_res(const int w, const int h);
extern void initXWindows(void);

#endif
