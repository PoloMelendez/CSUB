#pragma once
#include "Box2D/Box2D.h"
#include "const.h"
#include "contact.h"
#include "createObjects.h"
#include "log.h"
#include "physics.h"
#include "turret.h"
#include "button.h"
#include "door.h"
#include "goat.h"
#include <mine.h>
#include <movingPlatform.h>
#include <lens.h>

using namespace std;

extern b2World * world;
extern b2Body * toDestroy;
extern int bullet_ct;
extern b2Body * myPlayerFoot;
extern b2Body * myPlayer;
extern b2Body * myGun;
extern b2Body * myButton;
extern b2Body * myDoor;
extern b2Body * gameFloor;
extern b2Body * platform;
extern b2Body * p1;
extern b2Body * p2;
extern b2Body * gunEnemy1;
extern b2Body * gunEnemy2;
extern Turret * turrets;
extern Button * buttons;
extern Platform * platforms;
extern Door * doors;
extern Mine * mines;
extern Goat * goats;
extern Lens * lens;

void makeArena(const int & n);
void addObstacles(void);
void addStairs(void);
void init_b2d(void);
void restart(const int &);
void firstInit(void);
