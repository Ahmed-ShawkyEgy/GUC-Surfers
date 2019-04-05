#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "Vector3f.h"
#include "Camera.h" 

#define GLUT_KEY_ESCAPE 27
#define LEFT_LANE -2
#define CENTER_LANE 0
#define RIGHT_LANE 2
#define GROUND_LENGTH 10000
#define RESPAWN_POSITION 200

using namespace std;

int lanes[3] = { LEFT_LANE,CENTER_LANE,RIGHT_LANE };

struct Shape;
const int SKYBOX_BOUNDARY = 40;
const float GAME_SPEED = 0.8;

int WIDTH = 1280;
int HEIGHT = 720;
GLuint tex;
char title[] = "GUC Surfers";

float groundTransform = 0;

int coin_rotation_angle;
bool l0,l1,l2;
int light = 0;
int player_lane = 1;
int score = 0;
int virtual_score = 0;
int maxScore = 10;
int score_pos = -30;
int stop = 1;
vector<Shape> obstacles;
vector<Shape> coins;


struct Shape {
	float x;
	int lane;

	Shape(float x, int lane) {
		this->x = x, this->lane = lane;
	};
};

int cameraZoom = 0;

Camera camera = Camera(0.5f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//*******************************************************************************************//
// EYE (ex, ey, ez): defines the location of the camera.									 //
// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
//*******************************************************************************************//

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1280 / 720, 0.001, 200);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

// Model Variables
Model_3DS model_house;
Model_3DS model_car;
Model_3DS coin_model;
// Textures
GLTexture tex_ground;

GLTexture tex_surface;
GLTexture tex_wood;

void print(int x, int y, char *string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{







	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	

	GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l0Diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l0Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0Ambient[] = { .1f, 0.1f, 0.1f, 1.f };
	GLfloat l0Position[] = { 10.0f, 0.0f, 0.0f, l0};
	GLfloat l0Direction[] = { -1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);

	GLfloat l1Diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	GLfloat l1Ambient[] = { 0.1f, .1f, 0.1f, 1.0f };
	GLfloat l1Spec[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	GLfloat l1Position[] = { 0.0f, 10.0f, 0.0f, l1 };
	GLfloat l1Direction[] = { 0.0, -1.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l1Spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);

	GLfloat l2Diffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat l2Ambient[] = { 0.0f, 0.0f, .1f, 1.0f };
	GLfloat l2Spec[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat l2Position[] = { 0.0f, 0.0f, 10.0f, l2 };
	GLfloat l2Direction[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l2Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l2Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l2Spec);
	glLightfv(GL_LIGHT2, GL_POSITION, l2Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l2Direction);

	

}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}



//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with tescorexture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -3);
	glTexCoord2f(200, 0);
	glVertex3f(GROUND_LENGTH, 0, -3);
	glTexCoord2f(200, 3);
	glVertex3f(GROUND_LENGTH, 0, 3);
	glTexCoord2f(0, 3);
	glVertex3f(-20, 0, 3);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderSurface()
{
	glDisable(GL_LIGHTING);	// Disable lighting

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_surface.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-200, 0, -200);
	glTexCoord2f(100, 0);
	glVertex3f(GROUND_LENGTH, 0, -200);
	glTexCoord2f(100, 10);
	glVertex3f(GROUND_LENGTH, 0, 200);
	glTexCoord2f(0, 10);
	glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}



// Draws a unit quad
void renderFace(Vector3f normal)
{
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(normal.x, normal.y, normal.z);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-1, 0, -1);
	glTexCoord2f(1, 0);
	glVertex3f(1, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 0, 1);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 0, 1);
	glEnd();
	glPopMatrix();
}

void renderCoin(float x, float lane) {

	glDisable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	//Draw Coins
	glPushMatrix();
	glTranslatef(x + 5, 0.75 + 0.5, lane);
	glScalef(0.01, 0.015, 0.01);
	glRotatef(coin_rotation_angle, 0, 1, 0);
	coin_model.Draw();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.

}

void renderObstacle(float x, float lane)
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_wood.texture[0]);	// Bind the ground texture

	glPushMatrix();

	glTranslated(x, 1.7, lane);
	// Top Face
	glPushMatrix();
	glTranslated(0, 1, 0);
	renderFace(Vector3f(0, 1, 0));
	glPopMatrix();

	// Bottom Face
	glPushMatrix();
	glTranslated(0, -1, 0);
	renderFace(Vector3f(0, -1, 0));
	glPopMatrix();

	// Left Face
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslated(0, 1, 0);
	renderFace(Vector3f(1, 0, 0));
	glPopMatrix();

	// Right Face
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslated(0, -1, 0);
	renderFace(Vector3f(-1, 0, 0));
	glPopMatrix();

	// Front Face
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glTranslated(0, 1, 0);
	renderFace(Vector3f(1, 0, 0));
	glPopMatrix();


	// Back Face
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glTranslated(0, -1, 0);
	renderFace(Vector3f(1, 0, 0));
	glPopMatrix();



	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.

	glPopMatrix();
}

// adds an obstacle behind the skybox
void addObstacle(int lane)
{
	obstacles.push_back(Shape(RESPAWN_POSITION, lane));
}

void addCoin(int lane)
{
	coins.push_back(Shape(RESPAWN_POSITION, lane));
}



void destroyAtIndex(int index, vector<Shape> &shapes)
{
	// Swap this element with the last one to pop from the vector
	Shape tmp = shapes[shapes.size() - 1];
	shapes[shapes.size() - 1] = shapes[index];
	shapes[index] = tmp;
	shapes.pop_back();
}

// TODO implement
void onObstacleCollision()
{
	glFlush();
	groundTransform = 0;
	score = 0;
	if (virtual_score > 10) {
		virtual_score = 10;
	}
	else {
		virtual_score = 0;
	}
	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i].x -= 200;
	}
	for (int i = 0; i < coins.size(); i++)
	{
		coins[i].x -= 200;
	}
}

void onCoinCollision(int i)
{
	glFlush();
	glutSwapBuffers();

	virtual_score++;
	score++;

	if (virtual_score == 10) {
		glutSwapBuffers();
		tex_ground.Load("Textures/wood.bmp");
		tex_surface.Load("Textures/ground0.bmp");
		tex_wood.Load("Textures/marple.bmp");


		score = 0;
		maxScore = 20;
		for (int i = 0; i < obstacles.size(); i++)
		{
			obstacles[i].x -= 200;
		}
		for (int i = 0; i < coins.size(); i++)
		{
			coins[i].x -= 200;
		}
	}

	else if (virtual_score == 30) {

		stop = 0;
		exit(EXIT_SUCCESS);

	}
}

int random(int lower, int upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupCamera();
	InitLightSource();
	InitMaterial();

	/*GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);*/

	// Display Score
	char* strScore[20];

	glPushMatrix();
	if (player_lane == 0)
	{
		glTranslatef(-10, score_pos, 10);
	}
	else if (player_lane == 1)
	{

		glTranslatef(-10, score_pos, 12);

	}
	else {
		glTranslatef(-10, score_pos, 14);

	}
	glColor3f(0, 0, 0);	// Dim the ground texture a bit
	sprintf((char *)strScore, "Score = %d/%d", score, maxScore);
	print(50, 50, (char *)strScore);
	glPopMatrix();

	// Display Level

	glPushMatrix();
	glTranslated(groundTransform * 0.2, 0, 0);
	RenderSurface();
	glPopMatrix();

	// Draw Ground
	glPushMatrix();
	glTranslated(groundTransform, 0.5, 0);
	RenderGround();
	glPopMatrix();


	// Draw Player
	glPushMatrix();

	glTranslatef(0, 0.5, lanes[player_lane]);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(-90.f, 0, 1, 0);
	model_car.Draw();

	glPopMatrix();

	//Draw all Coins
	for (unsigned i = 0; i < coins.size(); i++)
	{
		renderCoin(coins[i].x, lanes[coins[i].lane]);
	}


	// Draw all obstacles
	for (unsigned i = 0; i < obstacles.size(); i++)
	{
		renderObstacle(obstacles[i].x, lanes[obstacles[i].lane]);
	}

	//sky box4
	glDisable(GL_LIGHTING);	// Disable lighting 
	glPushMatrix();

	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glPopMatrix();
	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.


	//glutSwapBuffers();
	glFlush();
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_car.Load("Models/car/ausfb.3ds");
	coin_model.Load("Models/coin/Coin Block.3ds");
	// Loading texture files
	if (score <= 2) {
		tex_ground.Load("Textures/ground.bmp");
	}
	tex_surface.Load("Textures/surface.bmp");

	tex_wood.Load("Textures/wall.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Animation Function
//=======================================================================
void anime()
{
	coin_rotation_angle += 5 * stop;
	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i].x -= GAME_SPEED * stop;

		// If player collided with obstacle
		if (obstacles[i].lane == player_lane &&
			obstacles[i].x <= 1 && obstacles[i].x >= 0)
		{
			onObstacleCollision();
		}

		// If the obstacle is way behind the player
		if (obstacles[i].x < -20)
			destroyAtIndex(i--, obstacles);
	}



	for (int i = 0; i < coins.size(); i++)
	{
		coins[i].x -= GAME_SPEED * stop;

		// If player collided with coin
		if (coins[i].lane == player_lane &&
			coins[i].x <= 0.9 && coins[i].x >= 0)
		{
			onCoinCollision(i);
			destroyAtIndex(i--, coins);
		}
	}

	for (int i = 0; i < coins.size(); i++)
	{
		// If the coin is way behind the player
		if (coins[i].x < -20 && coins.size() > 0)
			destroyAtIndex(i--, coins);
	}

	groundTransform -= GAME_SPEED * stop;

	for (int i = 0; i < 1e7 ; i++);
	glutPostRedisplay();
}



void Keyboard(unsigned char key, int x, int y) {
	float d = 0.8;
	float x_truck_cam = 2;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'd':
		if (player_lane < 2)
		{
			player_lane++;
			camera.moveX(-x_truck_cam);
		}
		break;
	case 'a':
		if (player_lane > 0)
		{
			player_lane--;
			camera.moveX(x_truck_cam);
		}
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

	case 't':
		score_pos = -48.5;
		camera = Camera(-8.0f, 7.0f, lanes[player_lane], -1.0f, 2.7f, lanes[player_lane], 0.0f, 1.0f, 0.0f);
		break;

	case 'f':
		score_pos = -30;
		camera = Camera(0.5f, 2.0f, lanes[player_lane], 1.0f, 2.0f, lanes[player_lane], 0.0f, 1.0f, 0.0f);;
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void dropObstacle(int v)
{
	boolean dropAllowed = random(0, 100) < 70;

	if (dropAllowed)
	{
		int lane = random(0, 2);
		addObstacle(lane);
	}
	glutTimerFunc(750, dropObstacle, 0);
}

void dropCoin(int v)
{
	boolean dropAllowed = random(0, 100) < 70;

	if (dropAllowed)
	{
		int lane = random(0, 2);
		addCoin(lane);
	}
	glutTimerFunc(2000, dropCoin, 0);
}

void lightAnim(int time)
{
	cout << light;
	if (light == 0) {
		l0 = 1;
		l1 = 0;
		l2 = 0;
	}
	if (light == 1) {
		l0 = 0;
		l1 = 1;
		l2 = 0;
	}
	if (light == 2) {
		l0 = 0;
		l1 = 0;
		l2 = 1;
	}
	light++;
	light %= 3;
	glutTimerFunc(1500, lightAnim, 0);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutIdleFunc(anime);

	glutTimerFunc(0, dropObstacle, 0);
	glutTimerFunc(0, dropCoin, 0);
	glutTimerFunc(0, lightAnim, 0);


	glutKeyboardFunc(Keyboard);

	glutSpecialFunc(Special);

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}


