#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <stdlib.h>
#include <vector>
#include "Vector3f.h"
#include "Camera.h" 

#define GLUT_KEY_ESCAPE 27

using namespace std;

struct Shape;
const int SKYBOX_BOUNDARY = 10;	
const float GAME_SPEED = 0.1;

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";


vector<Shape> obstacles;

struct Shape {
	float x;
	int lane;

	Shape(float x,int lane) {
		this->x = x,this->lane = lane;
	};
};

int cameraZoom = 0;

Camera camera = Camera(1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f);

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

// Textures
GLTexture tex_ground;
GLTexture tex_wood;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
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
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
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
	glNormal3f(normal.x,normal.y,normal.z);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-1, 0, -1);
	glTexCoord2f(5, 0);
	glVertex3f(1, 0, -1);
	glTexCoord2f(5, 5);
	glVertex3f(1, 0, 1);
	glTexCoord2f(0, 5);
	glVertex3f(-1,0, 1);
	glEnd();
	glPopMatrix();
}

void renderObstacle(float x,float lane)
{
	//glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(1,1,1);

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_wood.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glTranslated(x, 0, lane);

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
	glTranslated(0,1, 0);
	renderFace(Vector3f(1,0, 0));
	glPopMatrix();

	// Right Face
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslated(0,-1, 0);
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
	obstacles.push_back(Shape(SKYBOX_BOUNDARY,lane));
}

void destroyAtIndex(int index, vector<Shape> &shapes)
{
	// Swap this element with the last one to pop from the vector
	Shape tmp = shapes[shapes.size() - 1];
	shapes[shapes.size() - 1] = shapes[index];
	shapes[index] = tmp;
	shapes.pop_back();
}


//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	//RenderGround();

	// Draw Tree Model
	/*glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();*/

	// Draw house Model
	/*glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	model_house.Draw();
	glPopMatrix();*/

	glPushMatrix();
	renderObstacle(0,3);
	renderObstacle(0,-3);
	glPopMatrix();

	for (unsigned i = 0; i < obstacles.size();i++)
	{
		renderObstacle(obstacles[i].x, obstacles[i].lane);
	}

	//sky box
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



	glutSwapBuffers();
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_wood.Load("Textures/wood.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Animation Function
//=======================================================================
void anime()
{
	for (int i = 0; i < obstacles.size();i++)
	{
		obstacles[i].x-=GAME_SPEED;
	}

	for (int i = 0; i < 1e7; i++);
	glutPostRedisplay();
}



void Keyboard(unsigned char key, int x, int y) {
	float d = 0.8;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 4.0;

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

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	addObstacle(0);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutIdleFunc(anime);

	glutKeyboardFunc(Keyboard);

	glutSpecialFunc(Special);

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
