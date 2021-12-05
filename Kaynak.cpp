
/********
PROBLEMS: the game initializes 3 aliens at the top of the screen and in the same place for some reason, so when killed, ammo drops by 3
and the amount of killed targets increase by 3 i didnt know how to fix this
----------
FEATURES: there is a start screen, you can choose your difficulty by pressing one of "e" "h" and "n" and if another key is pressed the
the game gives an error sound. If hard mode is chosen health increases by 5 and ammo drops by 5. target speed changes according to the gamemode. The
health, the gamemode, the amount of ammo left, and the amount of aliens left is displayed during gameplay, health goes down every time an ailen
reaches the left side of the screen and if health becomes 0 the game ends if you run out of ammo the game ends, you get a different game over screens
according to why you died, when you win, you get a win screen. There are no boundary checks so when the player reaches the very bottom or top of the
screen, it teleports to the oppsite side, so it gets easier to move from very top to the bottom or vice versa
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <dos.h>


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define  TIMER_PERIOD  16 // 60fps
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_RADIUS 80

int status = 0;
//0 is start screen 1 is game screen 2 is game over screen

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false, s = false, e = false, n = false, h = false;
int  winWidth, winHeight; // current Window width and height

int count = 0;
int health = 15;
int ammo = 60;
int targetAmount = 0;
int difficulty = 0;
bool powerUp = false;
int powerCount = 0;


typedef struct {
	float x, y;
} point_t;

typedef struct {
	point_t pos;
	float   angle;
	float   r;
} player_t;

typedef struct {
	point_t pos;
	float angle;
	bool active;
} fire_t;

typedef struct {
	int r, g, b;
} color_t;

typedef struct {
	point_t center;
	color_t color;
	float radius;
	float speed = 3;
} target_t;

typedef struct {
	point_t center;
	float radius = 20;
	float speed = 3;
}power_t;

#define MAX_FIRE 20 // 20 fires at a time.
#define FIRE_RATE 8 // after 8 frames you can throw another one.

player_t p = { {-310,0}, 360, 20 };
fire_t   fr[MAX_FIRE];
int fire_rate = 0;
target_t target[3];
power_t power;



void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}


// display text with variables.
void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void s1yellow(int x, int y)
{
	glBegin(GL_POLYGON);
	glColor3ub(252, 239, 121);
	glVertex2f(x, y + 20);
	glVertex2f(x + 7, y);
	glVertex2f(x, y - 20);
	glVertex2f(x - 7, y);
	glVertex2f(x, y + 20);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3ub(252, 239, 121);
	glVertex2f(x + 3, y + 5);
	glVertex2f(x + 10, y);
	glVertex2f(x + 3, y - 5);

	glVertex2f(x - 3, y - 5);
	glVertex2f(x - 10, y);
	glVertex2f(x - 3, y + 5);
	glEnd();
}

void s2white(int x, int y)
{
	glColor3ub(250, 244, 192);
	glRectf(x - 2, y - 2, x + 2, y + 2);

	glBegin(GL_TRIANGLES);
	glVertex2f(x + 2, y + 2);
	glVertex2f(x, y + 10);
	glVertex2f(x - 2, y + 2);

	glVertex2f(x + 2, y + 2);
	glVertex2f(x + 10, y);
	glVertex2f(x + 2, y - 2);

	glVertex2f(x + 2, y - 2);
	glVertex2f(x, y - 10);
	glVertex2f(x - 2, y - 2);

	glVertex2f(x - 2, y + 2);
	glVertex2f(x - 10, y);
	glVertex2f(x - 2, y - 2);
	glEnd();
}

void drawBg() {
	//background and stars
	glBegin(GL_POLYGON);
	glColor3ub(39, 32, 97);
	glVertex2f(-400, 400);
	glVertex2f(400, 400);
	glColor3ub(62, 38, 82);
	glVertex2f(400, -400);
	glVertex2f(-400, -400);
	glEnd();

	s1yellow(-258, -134);
	s1yellow(11, 134);
	s1yellow(-67, 100);
	s1yellow(110, 23);
	s2white(-350, -10);
	s2white(130, 330);
	s2white(46, -300);
	s2white(300, -39);
	s2white(-200, 380);
	s2white(-150, -90);

	//planet
	glColor3ub(187, 149, 199);
	circle(400, -370, 300);
	glColor3ub(181, 125, 172);
	circle(290, -310, 40);
	circle(210, -270, 30);
	circle(180, -370, 25);
	circle(380, -240, 25);
	circle(405, -140, 35);
	circle(320, -150, 20);
}

void drawPlayer(player_t tp) {
	glColor3ub(242, 165, 70);
	glRectf(tp.pos.x - 20, tp.pos.y - 10, tp.pos.x + 20, tp.pos.y - 10);
	circle(tp.pos.x + 20, tp.pos.y, 10);
	glColor3ub(166, 164, 162);
	glRectf(tp.pos.x - 20, tp.pos.y + 10, tp.pos.x + 5, tp.pos.y + 15);
	glRectf(tp.pos.x - 20, tp.pos.y - 15, tp.pos.x + 5, tp.pos.y - 10);
	circle(tp.pos.x + 5, tp.pos.y + 12.5, 5);
	circle(tp.pos.x + 5, tp.pos.y - 12.5, 5);
	glBegin(GL_TRIANGLES);

	glColor3ub(242, 242, 242);
	glVertex2f(tp.pos.x, tp.pos.y);
	glVertex2f(tp.pos.x - 40, tp.pos.y - 10);
	glVertex2f(tp.pos.x - 40, tp.pos.y + 10);

	glVertex2f(tp.pos.x - 20, tp.pos.y);
	glVertex2f(tp.pos.x - 50, tp.pos.y - 20);
	glVertex2f(tp.pos.x - 50, tp.pos.y + 20);
	glEnd();

	glColor3ub(255, 165, 70);
	glRectf(tp.pos.x - 45, tp.pos.y - 10, tp.pos.x + 30, tp.pos.y + 10);

	circle(tp.pos.x + 25, tp.pos.y, 10);
}

void drawFires() {
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active) {
			glColor3ub(148, 219, 145);
			circle(fr[i].pos.x, fr[i].pos.y, 3);
		}
	}
}

void drawTarget(target_t target[], int i) {
	glColor3ub(193, 230, 161);
	circle(target[i].center.x, target[i].center.y + 10, 13);

	glColor3ub(target[i].color.r, target[i].color.g, target[i].color.b);
	glRectf(target[i].center.x - 15, target[i].center.y - 50, target[i].center.x + 15, target[i].center.y);
	glColor3ub(255, 255, 255);
	glRectf(target[i].center.x - 10, target[i].center.y - 36, target[i].center.x + 10, target[i].center.y - 30);
	glRectf(target[i].center.x - 8, target[i].center.y - 25, target[i].center.x + 8, target[i].center.y - 22);
	glRectf(target[i].center.x - 8, target[i].center.y - 42, target[i].center.x + 8, target[i].center.y - 39);


	glColor3ub(target[i].color.r, target[i].color.g, target[i].color.b);
	glRectf(target[i].center.x - 15, target[i].center.y - 70, target[i].center.x + 15, target[i].center.y - 50);

	glBegin(GL_TRIANGLES);
	glVertex2f(target[i].center.x - 15, target[i].center.y - 50);
	glVertex2f(target[i].center.x - 15, target[i].center.y - 70);
	glVertex2f(target[i].center.x - 25, target[i].center.y + -70);

	glVertex2f(target[i].center.x + 15, target[i].center.y - 50);
	glVertex2f(target[i].center.x + 15, target[i].center.y - 70);
	glVertex2f(target[i].center.x + 25, target[i].center.y - 70);
	glEnd();


	glColor3ub(0, 0, 0);
	circle(target[i].center.x + 7, target[i].center.y + 7, 5);
	circle(target[i].center.x - 7, target[i].center.y + 7, 5);

	glColor4ub(170, 221, 227, 75);
	circle(target[i].center.x, target[i].center.y + 15, 17);
	circle_wire(target[i].center.x, target[i].center.y, 78);
}

void drawhealth(float x, float y)
{
	glColor3ub(255, 255, 255);
	glRectf(x - 10, y - 5, x + 10, y + 5);
	glRectf(x - 5, y - 10, x + 5, y + 10);

	glColor3ub(255, 20, 0);
	glRectf(x - 7.5, y - 2.5, x + 7.5, y + 2.5);
	glRectf(x - 2.5, y - 7.5, x + 2.5, y + 7.5);

	glColor3ub(255, 255, 255);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(130, 360);
	glVertex2f(130, 375);
	glVertex2f(230, 375);
	glVertex2f(230, 360);
	glEnd();

}

void drawAmmo(float x, float y)
{
	glColor3ub(255, 255, 255);
	circle(x, y, 10);
	glColor3ub(148, 219, 145);
	circle(x, y, 7);
}

void drawAlien(float x, float y)
{
	glColor3ub(255, 255, 255);
	glLineWidth(3);
	circle_wire(x, y + 15, 14);

	glColor3ub(193, 230, 161);
	circle(x, y + 15, 10);

	glColor3ub(0, 0, 0);
	circle(x + 7, y + 12, 3);
	circle(x - 7, y + 12, 3);

	glColor4ub(170, 221, 227, 75);
	circle(x, y + 15, 14);

}


void drawAlienIcon()
{
	glColor3ub(255, 255, 255);
	glLineWidth(3);
	circle_wire(-24, 245 + 15, 17);

	glColor3ub(193, 230, 161);
	circle(-24, 245 + 15, 13);

	glColor3ub(0, 0, 0);
	circle(-24 + 7, 250 + 7, 5);
	circle(-24 - 7, 250 + 7, 5);

	glColor4ub(170, 221, 227, 75);
	circle(-24, 245 + 15, 17);
}



void display()
{

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	if (status == 0)
	{
		drawBg();
		glColor3ub(255, 255, 255);
		vprint(-110, 300, GLUT_BITMAP_TIMES_ROMAN_24, "~ALIEN HUNT~");
		vprint(-360, 168, GLUT_BITMAP_HELVETICA_18, "[To win kill 50 aliens]");
		vprint(-360, 144, GLUT_BITMAP_HELVETICA_18, "[Use your ammo carefully]");
		vprint(-360, 120, GLUT_BITMAP_HELVETICA_18, "[Don't let the aliens reach you]");
		/*vprint(-360, -72, GLUT_BITMAP_HELVETICA_18, "-> press [S] to start");*/
		vprint(-360, 96, GLUT_BITMAP_HELVETICA_18, "-> press [SPACE] to shoot");
		vprint(-360, 72, GLUT_BITMAP_HELVETICA_18, "-> press [ESC] to exit");
		vprint(-360, 48, GLUT_BITMAP_HELVETICA_18, "-> choose difficulty level to start:");
		glColor3ub(0, 255, 0);
		vprint(-60, 48, GLUT_BITMAP_HELVETICA_18, "[E]");
		glColor3ub(240, 220, 0);
		vprint(-20, 48, GLUT_BITMAP_HELVETICA_18, "[N]");
		glColor3ub(240, 50, 0);
		vprint(20, 48, GLUT_BITMAP_HELVETICA_18, "[H]");
		drawAlienIcon();

		glColor3ub(255, 255, 255);
		vprint(210, -370, GLUT_BITMAP_HELVETICA_18, "~by SILA YAPICI~");
	}
	else if (status == 1)
	{
		drawBg();
		drawFires();
		drawPlayer(p);
		for (int i = 0; i < 3; i++)
			drawTarget(target, i);
		drawAlien(-375, 354);
		glColor3ub(255, 255, 255);
		vprint(-354, 360, GLUT_BITMAP_HELVETICA_18, "ALIENS LEFT: [%d]", 50 - count);
		if (difficulty != 3) {
			/*if (health >= 9)
				glColor3ub(0, 255, 0);
			else if (health >= 3)
				glColor3ub(255, 200, 0);
			else glColor3ub(255, 0, 0);
			vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);*/

			if (health == 15)
			{
				glColor3ub(0, 255, 20);
				glRectf(130, 360, 230, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 12) {
				glColor3ub(100, 255, 20);
				glRectf(130, 360, 210, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 9)
			{
				glColor3ub(255, 255, 20);
				glRectf(130, 360, 190, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 6)
			{
				glColor3ub(255, 175, 20);
				glRectf(130, 360, 170, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 3)
			{
				glColor3ub(255, 60, 20);
				glRectf(130, 360, 150, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else {
				glColor3ub(255, 0, 0);
				glRectf(130, 360, 135, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
		}
		else {
			/*if (health >= 15)
				glColor3ub(0, 255, 0);
			else if (health >= 5)
				glColor3ub(255, 200, 0);
			else glColor3ub(255, 0, 0);
			vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);*/

			if (health == 20)
			{
				glColor3ub(0, 255, 20);
				glRectf(130, 360, 230, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 16) {
				glColor3ub(100, 255, 20);
				glRectf(130, 360, 210, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 12)
			{
				glColor3ub(255, 255, 20);
				glRectf(130, 360, 190, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 8)
			{
				glColor3ub(255, 170, 20);
				glRectf(130, 360, 170, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else if (health >= 4)
			{
				glColor3ub(255, 60, 20);
				glRectf(130, 360, 150, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
			else {
				glColor3ub(255, 0, 0);
				glRectf(130, 360, 135, 375);
				vprint(250, 360, GLUT_BITMAP_HELVETICA_18, "HEALTH: [%d]", health);
			}
		}
		drawhealth(100, 367);
		drawAmmo(-377, 327);

		if (ammo >= 15)
			glColor3ub(0, 255, 0);
		else if (ammo >= 7)
			glColor3ub(255, 200, 0);
		else
			glColor3ub(255, 0, 0);
		vprint(-359, 320, GLUT_BITMAP_HELVETICA_18, "AMMO LEFT: [%d]", ammo); //-389

		if (difficulty == 1) {
			glColor3ub(0, 255, 0);
			vprint(-389, -370, GLUT_BITMAP_HELVETICA_18, "EASY MODE");
		}
		else if (difficulty == 2) {
			glColor3ub(255, 200, 0);;
			vprint(-389, -370, GLUT_BITMAP_HELVETICA_18, "NORMAL MODE");
		}
		else {
			glColor3ub(255, 20, 0);;
			vprint(-389, -370, GLUT_BITMAP_HELVETICA_18, "HARD MODE");
		}
	}
	else if (status == 2)
	{
		drawBg();
		glColor3ub(255, 100, 0);
		vprint(-210, 0, GLUT_BITMAP_HELVETICA_18, "[YOUR HEALTH REACHED ZERO. GAME OVER]");
		vprint(-70, -24, GLUT_BITMAP_HELVETICA_12, "[Press ESC to exit]");
	}
	else if (status == 3)
	{
		drawBg();
		glColor3ub(255, 100, 0);
		vprint(-210, 0, GLUT_BITMAP_HELVETICA_18, "[YOUR RAN OUT OF AMMO. GAME OVER]");
		vprint(-70, -24, GLUT_BITMAP_HELVETICA_12, "[Press ESC to exit]");
	}
	else if (status == 4)
	{
		drawBg();
		glColor3ub(0, 255, 0);
		vprint(-80, 0, GLUT_BITMAP_HELVETICA_18, "[YOU WON!]");
		vprint(-77, -24, GLUT_BITMAP_HELVETICA_12, "[Press ESC to exit]");
	}

	glutSwapBuffers();

}


// In the fire array, check if any fire is available.
// if there is a fire that we can use for firing, return its index.
int findAvailableFire() {
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active == false) return i;
	}
	return -1;
}

void resetTarget(target_t target[], int i) {
	float yPos;
	switch (targetAmount % 11)
	{
	case 0:
		yPos = 340;
		break;
	case 1:
		yPos = 150;
		break;
	case 2:
		yPos = -320;
		break;
	case 3:
		yPos = -100;
		break;
	case 4:
		yPos = 290;
		break;
	case 5:
		yPos = -10;
		break;
	case 6:
		yPos = -190;
		break;
	case 7:
		yPos = 240;
		break;
	case 8:
		yPos = -70;
		break;
	case 9:
		yPos = 110;
		break;
	case 10:
		yPos = -320;
		break;
	}
	target[i].center = { 400, yPos };
	target[i].color.r = rand() % 256;
	target[i].color.g = rand() % 256;
	target[i].color.b = rand() % 256;
	target[i].radius = TARGET_RADIUS;
	switch (difficulty)
	{
	case 1:
		target[i].speed = 0.5 + 3.0 * (rand() % 100) / 100.0 + 0.5;
		break;
	case 2:
		target[i].speed = 2.1 + 3.0 * (rand() % 100) / 100.0 + 0.5;
		break;
	case 3:
		target[i].speed = 3.0 + 3.0 * (rand() % 100) / 100.0 + 0.5;
		break;
	}

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = true;
	if (key == 'e' && status == 0)
		e = true;
	else if (key == 'n' && status == 0)
		n = true;
	else if (key == 'h' && status == 0)
		h = true;
	else if (status == 0)
		printf("\a");
	// to refresh the window it calls display() function
	//glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = false;
	if (key == 'e' && status == 0)
	{
		e = false;
		difficulty = 1;
		status = 1;
	}
	else if (key == 'n' && status == 0)
	{
		n = false;
		difficulty = 2;
		status = 1;
	}
	else if (key == 'h' && status == 0)
	{
		h = false;
		difficulty = 3;
		health += 5;
		ammo -= 5;
		status = 1;
	}
	// to refresh the window it calls display() function
	//glutPostRedisplay() ;
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// to refresh the window it calls display() function
	//glutPostRedisplay() ;
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	// to refresh the window it calls display() function
	//glutPostRedisplay() ;
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	//glutPostRedisplay() ; 
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	//glutPostRedisplay() ;
}

void onMove(int x, int y) {
	// Write your codes here.



	 // to refresh the window it calls display() function
	 //glutPostRedisplay() ;
}


void movePlayer(player_t *tp, float speed) {

	float dy = speed;


	tp->pos.y += dy;
	float r = tp->r;


	if (tp->pos.y > (380 - r))
		tp->pos.y = -360;
	if (tp->pos.y < (-380 + r))
		tp->pos.y = 360;



}

bool testCollision(fire_t fr, target_t t[], int i) {
	float dx = t[i].center.x - fr.pos.x;
	float dy = t[i].center.y - fr.pos.y;
	float d = sqrt(dx*dx + dy * dy);
	return d <= t[i].radius;
}



#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	// Write your codes here.
	if (status == 1)
	{
		if (up) {
			movePlayer(&p, 3);
		}
		if (down) {
			movePlayer(&p, -3);
		}

		if (spacebar && fire_rate == 0) {
			int availFire = findAvailableFire();
			if (availFire != -1) {
				fr[availFire].pos = p.pos;
				fr[availFire].angle = p.angle;
				fr[availFire].active = true;
				fire_rate = FIRE_RATE;
			}
		}

		if (fire_rate > 0) fire_rate--;

		// Move all fires that are active.
		for (int i = 0; i < MAX_FIRE; i++) {
			if (fr[i].active) {
				fr[i].pos.x += 10 * cos(fr[i].angle * D2R);
				fr[i].pos.y += 10 * sin(fr[i].angle * D2R);

				if (fr[i].pos.x > 400 || fr[i].pos.x < -400 || fr[i].pos.y>400 || fr[i].pos.y < -400) {
					if (fr[i].pos.x > 400)
					{
						ammo--;
						targetAmount++;
					}
					fr[i].active = false;
				}
				for (int k = 0; k < 3; k++)
				{
					if (testCollision(fr[i], target, k)) {
						ammo--;
						count++;
						targetAmount++;
						fr[i].active = false;
						resetTarget(target, k);
					}
				}
			}
		}


		//move target
		for (int j = 0; j < 3; j++)
		{
			target[j].center.x -= target[j].speed;
			if (target[j].center.x < 400)
				targetAmount++;
			if (target[j].center.x < -400) {
				health--;
				resetTarget(target, j);
			}
		}


		if (health == 0)
			status = 2;
		if (ammo == 0)
			status = 3;
		if (50 - count == 0)
			status = 4;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}

#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int j = 0; j < 3; j++)
		resetTarget(target, j);
}


void main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("ALIEN HUNT");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}
