//cs335 Spring 2014
//
//program: lab2_asteroids.c
//author:  Gordon Griesel
//date:    2014
//
//This program is a simple Asteroids game.
//One problem is that linked-list nodes are not deallocated.
//Please try to fix this.
//
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "ppm.h"
#include "fonts.h"

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
			     (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void check_mouse(XEvent *e);
void check_keys(XEvent *e);
void init();
void init_sounds(void);
void physics(void);
void render(void);

//-----------------------------------------------------------------------------
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
double timeDiff(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec ) +
	(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
    memcpy(dest, source, sizeof(struct timespec));
}
//-----------------------------------------------------------------------------

int done=0;
int xres=800, yres=600;

typedef struct t_ship {
    Vec dir;
    Vec pos;
    Vec vel;
    float angle;
    float color[3];
} Ship;
Ship ship;

typedef struct t_bullet {
    Vec pos;
    Vec vel;
    float color[3];
    struct timespec time;
    struct t_bullet *prev;
    struct t_bullet *next;
} Bullet;
Bullet *bhead=NULL;
struct timespec bulletTimer;

typedef struct t_asteroid {
    Vec pos;
    Vec vel;
    int nverts;
    Flt radius;
    Vec vert[8];
    float angle;
    float rotate;
    float color[3];
    struct t_asteroid *prev;
    struct t_asteroid *next;
} Asteroid;
Asteroid *ahead=NULL;

int keys[65536];

static float pos[3]={20.0,200.0,0.0};
static float vel[3]={5.0,0.0,0.0};
static float redpos[3]={20.0,400.0,0.0};
static float redvel[3]={5.0,0.0,0.0};

int main(void)
{
    logOpen();
    initXWindows();
    init_opengl();
    init();
    srand(time(NULL));
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
    while(!done) {
	while(XPending(dpy)) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    check_resize(&e);
	    check_mouse(&e);
	    check_keys(&e);
	}
	clock_gettime(CLOCK_REALTIME, &timeCurrent);
	timeSpan = timeDiff(&timeStart, &timeCurrent);
	timeCopy(&timeStart, &timeCurrent);
	physicsCountdown += timeSpan;
	while(physicsCountdown >= physicsRate) {
	    physics();
	    physicsCountdown -= physicsRate;
	}
	render();
	glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    cleanup_fonts();
    logClose();
    return 0;
}

void cleanupXWindows(void)
{
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "CS335 - Asteroids template");
}

void setup_screen_res(const int w, const int h)
{
    xres = w;
    yres = h;
}

void initXWindows(void)
{
    Window root;
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
    XVisualInfo *vi;
    Colormap cmap;
    XSetWindowAttributes swa;

    setup_screen_res(xres, yres);
    dpy = XOpenDisplay(NULL);
    if(dpy == NULL) {
	printf("\n\tcannot connect to X server\n\n");
	exit(EXIT_FAILURE);
    }
    root = DefaultRootWindow(dpy);
    vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
	printf("\n\tno appropriate visual found\n\n");
	exit(EXIT_FAILURE);
    } 
    //else {
    //	// %p creates hexadecimal output like in glxinfo
    //	printf("\n\tvisual %p selected\n", (void *)vi->visualid);
    //}
    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
	StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
	    vi->depth, InputOutput, vi->visual,
	    CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void reshape_window(int width, int height)
{
    //window has been resized.
    setup_screen_res(width, height);
    //
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, xres, 0, yres, -1, 1);
    set_title();
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, xres, yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, xres, 0, yres, -1, 1);
    //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //
    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
}

void check_resize(XEvent *e)
{
    //The ConfigureNotify is sent by the
    //server if the window is resized.
    if (e->type != ConfigureNotify)
	return;
    XConfigureEvent xce = e->xconfigure;
    if (xce.width != xres || xce.height != yres) {
	//Window size did change.
	reshape_window(xce.width, xce.height);
    }
}

Asteroid *buildAsteroid(Flt radius) {
    int i;
    Asteroid *a = (Asteroid *)malloc(sizeof(Asteroid));
    a->nverts = 8;
    a->radius = radius;
    Flt r2 = radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)a->nverts;
    for (i=0; i<a->nverts; i++) {
	a->vert[i][0] = sin(angle) * (r2 + rnd() * radius);
	a->vert[i][1] = cos(angle) * (r2 + rnd() * radius);
	angle += inc;
    }
    a->pos[0] = (Flt)(rand() % xres);
    a->pos[1] = (Flt)(rand() % yres);
    a->pos[2] = 0.0f;
    a->angle = 0.0;
    a->rotate = rnd() * 4.0 - 2.0;
    a->color[0] = 0.8;
    a->color[1] = 0.8;
    a->color[2] = 0.7;
    a->vel[0] = (Flt)(rnd()*2.0-1.0);
    a->vel[1] = (Flt)(rnd()*2.0-1.0);
    return a;
}


void init() {
    int j;
    VecZero(ship.dir);
    ship.pos[0] = (Flt)(xres/2);
    ship.pos[1] = (Flt)(yres/2);
    ship.pos[2] = 0.0f;
    VecZero(ship.vel);
    ship.angle = 270.0;
    ship.color[0] = 1.0;
    ship.color[1] = 1.0;
    ship.color[2] = 1.0;
    //
    pos[0] = (Flt)(xres/2);
    pos[1] = (Flt)(yres/2);
    //
    for (j=0; j<10; j++) {
	//build an asteroid...
	Asteroid *a = buildAsteroid(8.0 + rnd() * 80);
	//put asteroid into linked list
	a->prev = NULL;
	a->next = ahead;
	if (ahead != NULL)
	    ahead->prev = a;
	ahead = a;
    }
    clock_gettime(CLOCK_REALTIME, &bulletTimer);
    memset(keys, 0, 65536);
}

void normalize(Vec v) {
    Flt len = v[0]*v[0] + v[1]*v[1];
    if (len == 0.0f) {
	v[0] = 1.0;
	v[1] = 0.0;
	return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

void check_mouse(XEvent *e)
{
    //Did the mouse move?
    //Was a mouse button clicked?
    static int savex = 0;
    static int savey = 0;
    //
    if (e->type == ButtonRelease) {
	return;
    }
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    //Left button is down
	}
	if (e->xbutton.button==3) {
	    //Right button is down
	}
    }
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
	//Mouse moved
	savex = e->xbutton.x;
	savey = e->xbutton.y;
    }
}

void check_keys(XEvent *e)
{
    //keyboard input?
    static int shift=0;
    int key = XLookupKeysym(&e->xkey, 0);
    //Log("key: %i\n", key);
    if (e->type == KeyRelease) {
	keys[key]=0;
	if (key == XK_Shift_L || key == XK_Shift_R)
	    shift=0;
	return;
    }
    if (e->type == KeyPress) {
	keys[key]=1;
	if (key == XK_Shift_L || key == XK_Shift_R) {
	    shift=1;
	    return;
	}
    } else {
	return;
    }
    if (shift){}
    switch(key) {
	case XK_Escape:
	    done=1;
	    break;
	case XK_f:
	    break;
	case XK_s:
	    break;
	case XK_Down:
	    break;
	case XK_equal:
	    break;
	case XK_minus:
	    break;
    }
}

void deleteBullet(Bullet *node)
{
    //Same requirements as deleteAsteroid()

    if(node->prev == NULL && node->next == NULL){
	bhead = NULL;
    }
    else if(node->prev == NULL){
	bhead = node->next;
	bhead->prev = NULL;
    }
    else if(node->next == NULL){
	node->prev->next = NULL;
    }
    else {
	node->prev->next = node->next;
	node->next->prev = node->prev;
    }
    free(node);
    node = NULL;

}

void deleteAsteroid(Asteroid *node)
{
    //remove a node from linked list
    //
    //You should add code here.
    //
    //Check for the following special conditions:
    //
    //1. Only 1 item in list.
    //2. At beginning of list.
    //3. At end of list.
    //4. In middle of list.
    //
    //Adjust the prev and next pointers appropriately.
    //
    //Free the node memory with free(node);
    //Set the node to null;
    if(node->prev == NULL && node->next == NULL){
	ahead = NULL;
    }
    else if(node->prev == NULL){
	ahead = node->next;
	ahead->prev = NULL;
    }
    else if(node->next == NULL){
	node->prev->next = NULL;
    }
    else {
	node->prev->next = node->next;
	node->next->prev = node->prev;
    }
    free(node);
    node = NULL;

}

int checkCollision(){
    return(!(redpos[0]+40.0f <= pos[0]-20.0f || redpos[0]-40.0f >= pos[0]+20.0f || redpos[1]+40.0f <= pos[1]-20.0f || redpos[1]-40.0f >= pos[1]+20.0f));
}



void physics(void)
{
    int addgrav = 1;
    redpos[0] += redvel[0];
    redpos[1] += redvel[1];
    if ((redpos[0] < 0.0          && redvel[0] < 0.0) ||
	    (redpos[0] >= (float)xres && redvel[0] > 0.0)) {
	redvel[0] = -redvel[0];
	addgrav = 0;
    }
    if ((redpos[1] < 0.0          && redvel[1] < 0.0) ||
	    (redpos[1] >= (float)yres && redvel[1] > 0.0)) {
	redvel[1] = -redvel[1];
	addgrav = 0;
    }
/*
    if(!(redpos[0]+40.0f <= pos[0]-20.0f || redpos[0]-40.0f >= pos[0]+20.0f ||
	    redpos[1]+40.0f <= pos[1]-20.0f || redpos[1]-40.0f >= pos[1]+20.0f)){
	redvel[1] = -redvel[1];
	redvel[0] = -redvel[0];
	addgrav = 0;
    }*/
    if(checkCollision()){
	redvel[1] = -redvel[1];
	redvel[0] = -redvel[0];
	while(checkCollision()){
	    redpos[0] += redvel[0];
	    redpos[1] += redvel[1];
	}
	addgrav = 0;
    }
    
    //Apply gravity
    if (addgrav)
	redvel[1] -= 0.5;




    Flt d0,d1,dist;
    //Update ship position
    ship.pos[0] += ship.vel[0];
    ship.pos[1] += ship.vel[1];
    //Check for collision with window edges
    if (ship.pos[0] < 0.0) {
	ship.pos[0] += (Flt)xres;
    }
    else if (ship.pos[0] > (Flt)xres) {
	ship.pos[0] -= (Flt)xres;
    }
    else if (ship.pos[1] < 0.0) {
	ship.pos[1] += (Flt)yres;
    }
    else if (ship.pos[1] > (Flt)yres) {
	ship.pos[1] -= (Flt)yres;
    }
    //
    //
    //Update bullet positions
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    Bullet *b = bhead;
    while(b) {
	double ts = timeDiff(&b->time, &bt);
	if (ts > 2.5) {
	    Bullet *saveb = b->next;
	    deleteBullet(b);
	    b = saveb;
	    continue;
	}
	b->pos[0] += b->vel[0];
	b->pos[1] += b->vel[1];
	//Check for collision with window edges
	if (b->pos[0] < 0.0) {
	    b->pos[0] += (Flt)xres;
	}
	else if (b->pos[0] > (Flt)xres) {
	    b->pos[0] -= (Flt)xres;
	}
	else if (b->pos[1] < 0.0) {
	    b->pos[1] += (Flt)yres;
	}
	else if (b->pos[1] > (Flt)yres) {
	    b->pos[1] -= (Flt)yres;
	}
	b = b->next;
    }
    //
    //Update asteroid positions
    Asteroid *a = ahead;
    while(a) {
	a->pos[0] += a->vel[0];
	a->pos[1] += a->vel[1];
	//Check for collision with window edges
	if (a->pos[0] < -100.0) {
	    a->pos[0] += (Flt)xres+200;
	}
	else if (a->pos[0] > (Flt)xres+100) {
	    a->pos[0] -= (Flt)xres+200;
	}
	else if (a->pos[1] < -100.0) {
	    a->pos[1] += (Flt)yres+200;
	}
	else if (a->pos[1] > (Flt)yres+100) {
	    a->pos[1] -= (Flt)yres+200;
	}
	a->angle += a->rotate;
	//
	//is there a bullet within radius?
	//
	Bullet *b = bhead;
	while(b) {
	    d0 = b->pos[0] - a->pos[0];
	    d1 = b->pos[1] - a->pos[1];
	    dist = d0*d0+d1*d1;
	    if (dist < (a->radius*a->radius)) {
		//this asteroid is hit.
		if (a->radius > 20.0) {
		    //break it into pieces.
		    int i;
		    int k = rand()%2 + 2;
		    for (i=0; i<k; i++) {
			Asteroid *na = buildAsteroid(a->radius/2.0);
			na->pos[0] = a->pos[0] + a->vert[i*2][0];
			na->pos[1] = a->pos[1] + a->vert[i*2][1];
			na->vel[0] = a->vel[0] + rnd() - 0.5;
			na->vel[1] = a->vel[1] + rnd() - 0.5;
			//put asteroid into linked list
			na->prev = NULL;
			na->next = ahead;
			if (ahead != NULL)
			    ahead->prev = na;
			ahead = na;
		    }
		}
		//delete the asteroid
		Asteroid *sa = a->next;
		deleteAsteroid(a);
		a = sa;
		//delete the bullet
		Bullet *sb = b->next;
		deleteBullet(b);
		b = sb;
		break;
	    }
	    b = b->next;
	}
	if (a==NULL)
	    break;
	a = a->next;
    }
    //
    //check keys that are currently held down
    if (keys[XK_Up]) {
	if(ship.angle < 356.0f && ship.angle >= 180.0f){
	    ship.angle += 4.0;
	    //if (ship.angle >= 360.0f)
	    //ship.angle = 360.0f;
	}
	if(ship.angle > 4.0f && ship.angle <= 180.0f)
	    ship.angle -= 4.0;
    }
    if (keys[XK_Down]) {
	if(ship.angle <= 360.0f && ship.angle > 184.0f){
	    ship.angle -= 4.0;
	    //if (ship.angle < 0.0f)
	    //ship.angle += 360.0f;
	}
	if(ship.angle >= 0.0f && ship.angle < 176.0f)
	    ship.angle += 4.0;

    }
    /*
       if (keys[XK_Up]) {
//apply thrust
//convert ship angle to radians
Flt rad = ((ship.angle+90.0) / 360.0f) * PI * 2.0;
//convert angle to a vector
Flt xdir = cos(rad);
Flt ydir = sin(rad);
ship.vel[0] += xdir*0.02f;
ship.vel[1] += ydir*0.02f;
Flt speed = sqrt(ship.vel[0]*ship.vel[0]+ship.vel[1]*ship.vel[1]);
if (speed > 10.0f) {
speed = 10.0f;
normalize(ship.vel);
ship.vel[0] *= speed;
ship.vel[1] *= speed;
}
}*/
if(keys[XK_Left]){
    pos[0] -= vel[0];
    ship.pos[0] -= vel[0];
    if(ship.angle >= 180.0)
	ship.angle = 360.0 - ship.angle;
}
if(keys[XK_Right]){
    pos[0] += vel[0];
    ship.pos[0] += vel[0];
    if(ship.angle <= 180.0)
	ship.angle = 360.0 - ship.angle;
}
if (keys[XK_space]) {
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    double ts = timeDiff(&bulletTimer, &bt);
    if (ts > 0.15) {
	timeCopy(&bulletTimer, &bt);
	//shoot a bullet...
	Bullet *b = (Bullet *)malloc(sizeof(Bullet));
	timeCopy(&b->time, &bt);
	b->pos[0] = ship.pos[0];
	b->pos[1] = ship.pos[1];
	b->vel[0] = ship.vel[0];
	b->vel[1] = ship.vel[1];
	//convert ship angle to radians
	Flt rad = ((ship.angle+90.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	b->pos[0] += xdir*20.0f;
	b->pos[1] += ydir*20.0f;
	b->vel[0] += xdir*6.0f + rnd()*0.1;
	b->vel[1] += ydir*6.0f + rnd()*0.1;
	b->color[0] = 1.0f;
	b->color[1] = 1.0f;
	b->color[2] = 1.0f;
	//put bullet into linked list
	b->prev = NULL;
	b->next = bhead;
	if (bhead != NULL)
	    bhead->prev = b;
	bhead = b;
    }
}
}

void render(void)
{
    Rect r;
    glClear(GL_COLOR_BUFFER_BIT);
    //
    r.bot = yres - 20;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ff0000, "cs335 - Asteroids");
    //draw a square
    {
	float wid = 40.0f;
	glColor3ub(30,60,90);
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glBegin(GL_QUADS);
	glVertex2i(-wid, -wid);
	glVertex2i(-wid, wid);
	glVertex2i(wid, wid);
	glVertex2i(wid, -wid);
	glEnd();
	glPopMatrix();
    }

    //draw a red square
    {
	float dwid = 20.0f;
	glColor3ub(255,0,0);
	glPushMatrix();
	glTranslatef(redpos[0], redpos[1], redpos[2]);
	glBegin(GL_QUADS);
	glVertex2i(-dwid, -dwid);
	glVertex2i(-dwid, dwid);
	glVertex2i(dwid, dwid);
	glVertex2i(dwid, -dwid);
	glEnd();
	glPopMatrix();
    }
    //-------------------------------------------------------------------------
    //Draw the ship
    glColor3fv(ship.color);
    glPushMatrix();
    glTranslatef(ship.pos[0], ship.pos[1], ship.pos[2]);
    glRotatef(ship.angle, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    //ship made of 1 triangle
    //glVertex2f(-10.0f, -10.0f);
    //glVertex2f(  0.0f, 20.0f);
    //glVertex2f( 10.0f, -10.0f);
    //ship made of 2 triangles
    glVertex2f(-12.0f, -10.0f);
    glVertex2f(  0.0f, 20.0f);
    glVertex2f(  0.0f, -6.0f);
    glVertex2f(  0.0f, -6.0f);
    glVertex2f(  0.0f, 20.0f);
    glVertex2f( 12.0f, -10.0f);
    /*glVertex2f(pos[0], pos[1]);
      glVertex2f( pos[0], pos[1]);
      glVertex2f( pos[0], pos[1]);
      glVertex2f( pos[0], pos[1]);
      glVertex2f( pos[0], pos[1]);
      glVertex2f( pos[0], pos[1]);*/
    glEnd();
    glPopMatrix();
    /*
       if (keys[XK_Up]) {
       int i;
    //draw thrust
    Flt rad = ((ship.angle+90.0) / 360.0f) * PI * 2.0;
    //convert angle to a vector
    Flt xdir = cos(rad);
    Flt ydir = sin(rad);
    Flt xs,ys,xe,ye,r;
    glBegin(GL_LINES);
    for (i=0; i<16; i++) {
    xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
    ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
    r = rnd()*40.0+40.0;
    xe = -xdir * r + rnd() * 18.0 - 9.0;
    ye = -ydir * r + rnd() * 18.0 - 9.0;
    glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
    glVertex2f(ship.pos[0]+xs,ship.pos[1]+ys);
    glVertex2f(ship.pos[0]+xe,ship.pos[1]+ye);
    }
    glEnd();
    }*/
    //-------------------------------------------------------------------------
    //Draw the asteroids
    {
	int i;
	glLineWidth(3.0);
	Asteroid *a = ahead;
	while(a) {
	    glColor3fv(a->color);
	    glPushMatrix();
	    glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
	    glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
	    glBegin(GL_LINE_LOOP);
	    for (i=0; i<a->nverts; i++) {
		glVertex2f(a->vert[i][0], a->vert[i][1]);
	    }
	    glEnd();
	    glPopMatrix();
	    glColor3f(1.0f, 0.0f, 0.0f);
	    glBegin(GL_POINTS);
	    glVertex2f(a->pos[0], a->pos[1]);
	    glEnd();
	    a = a->next;
	}
	glLineWidth(1.0);
    }
    //-------------------------------------------------------------------------
    //Draw the bullets
    {
	Bullet *b = bhead;
	while(b) {
	    glColor3f(1.0, 1.0, 1.0);
	    glBegin(GL_POINTS);
	    glVertex2f(b->pos[0],      b->pos[1]);
	    glVertex2f(b->pos[0]-1.0f, b->pos[1]);
	    glVertex2f(b->pos[0]+1.0f, b->pos[1]);
	    glVertex2f(b->pos[0],      b->pos[1]-1.0f);
	    glVertex2f(b->pos[0],      b->pos[1]+1.0f);
	    glColor3f(0.8, 0.8, 0.8);
	    glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
	    glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
	    glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
	    glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
	    glEnd();
	    b = b->next;
	}
    }
    /*
    //draw a square
    {
    float wid = 40.0f;
    glColor3ub(30,60,90);
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glBegin(GL_QUADS);
    glVertex2i(-wid, -wid);
    glVertex2i(-wid, wid);
    glVertex2i(wid, wid);
    glVertex2i(wid, -wid);
    glEnd();
    glPopMatrix();
    }
    */
}



