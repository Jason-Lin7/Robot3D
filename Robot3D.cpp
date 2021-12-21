/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "cube.h"
#include "QuadMesh.h"
#include <gl\freeglut.h>

const int vWidth  = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 8.0;
float robotBodyLength = 10.0;
float robotBodyDepth = 6.0;
float headWidth = 0.4*robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength;
float upperArmWidth = 0.125*robotBodyWidth;
float gunLength = upperArmLength / 4.0;
float gunWidth = upperArmWidth;
float gunDepth = upperArmWidth;
float stanchionLength = robotBodyLength;
float stanchionRadius = 0.1*robotBodyDepth;
float baseWidth = 2 * robotBodyWidth;
float baseLength = 0.25*stanchionLength;

// Control Robot body rotation on base
float robotAngle = 0.0;

// Control arm rotation
float shoulderAngle = -40.0;
float gunAngle = -25.0;
float bodyRotate = 10.0;
float armAngle = 0.0;
float moveForward = 0.0;

// Spin Cube Mesh
float cubeAngle = 0.0;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat robotBody_mat_ambient[] = { 1.0f,1.0f,1.0f,2.0f };
GLfloat robotBody_mat_specular[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat robotBody_mat_diffuse[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat robotBody_mat_shininess[] = { 32.0F };


GLfloat robotArm_mat_ambient[] = { 1.0f,1.0f,1.0f,2.0f };
GLfloat robotArm_mat_diffuse[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat robotArm_mat_specular[] = { 0.25f, 0.25f, 0.25f, 2.0f };
GLfloat robotArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

GLfloat robotLowerBody_mat_ambient[] = { 1.0f,1.0f,1.0f,2.0f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat robotLowerBody_mat_specular[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat robotLowerBody_mat_shininess[] = { 76.8F };

GLfloat robotEye_mat_ambient[] = { 1.0f,0.25f,0.25f,2.0f };
GLfloat robotEye_mat_diffuse[] = { 1.0f,0.25f,0.25f,2.0f };
GLfloat robotEye_mat_specular[] = { 1.0f,0.25f,0.25f,2.0f };
GLfloat robotEye_mat_shininess[] = { 76.8F };

GLfloat button_mat_ambient[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat button_mat_diffuse[] = { 0.0f,0.0f,0.0f,2.0f };
GLfloat button_mat_specular[] = { 0.25f,0.25f,0.25f,2.0f };
GLfloat button_mat_shininess[] = { 76.8F };


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

// A template cube mesh
CubeMesh *cubeMesh = createCubeMesh();

// A flat open mesh
QuadMesh *groundMesh = NULL;

// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
    VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void drawRobot();
void drawBody();
void drawHead();
void drawLowerBody();
void drawLeftShoulder();
void drawLeftArm();
void drawRightArm();
void drawRightShoulder();
void drawLeftConnector();
void drawRightConnector();
void drawHeadConnector();
void drawHeadCamera();
void drawWheel();
void drawEye();

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	
	//Changes background Color to grey for Bot 3.
	glClearColor(0.1F, 0.1F, 0.1F, 0.1F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-16.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw Robot

	// Apply modelling transformations M to move robot
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV
	drawRobot();

	// Example of drawing a mesh (closed cube mesh)
	glPushMatrix();
	// spin cube
	glTranslatef(8.0, 0, 3.0);
	glRotatef(cubeAngle, 0.0, 1.0, 0.0);
	glTranslatef(-8.0, 0, -3.0);
	// position and draw cube
	glTranslatef(8.0, 0, 3.0); 
	drawCubeMesh(cubeMesh);
	glPopMatrix();

	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -20.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}

void drawRobot()
{
	glPushMatrix();
	 // spin robot on base. 
	 glRotatef(robotAngle, 0.0, 1.0, 0.0);
	 glPushMatrix();
		glPushMatrix();
			glTranslatef(0, 0, moveForward);
			drawHead();
			drawBody();
			drawHeadConnector();
			drawLeftArm();
			drawRightArm();
			drawLeftShoulder();
			drawRightShoulder();
			drawLeftConnector();
			drawRightConnector();
			drawHeadCamera();
			drawWheel();
			drawEye();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
	
	// don't want to spin fixed base
	drawLowerBody();

	glPopMatrix();
}


void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	glTranslatef(0, -6, 0);
	glScalef(stanchionRadius, stanchionLength, stanchionRadius);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(gluNewQuadric(), 4.5, 4.5, 1.05, 10, 10);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, button_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, button_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, button_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, button_mat_shininess);

	glPushMatrix();
	glutSolidCylinder(0.7, 2.8, 10, 10);
	glPopMatrix();
}

void drawHeadCamera()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(0, 10.5, 0);
	glRotatef(robotAngle, 0.0, 1.0, 0.0);
	// build arm
	glPushMatrix();
	glutSolidSphere(1.1, 100, 100);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10.5, 0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	gluCylinder(gluNewQuadric(), 0.6, 0.6, 2, 10, 10);
	glPopMatrix();
}

void drawHeadConnector()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(0, 0.6 * robotBodyLength + 0.5 * headLength,0);
	// build arm
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(gluNewQuadric(), 0.7, 0.7, 2.5, 10, 10);
	glPopMatrix();
	glPopMatrix();
}

void drawHead()
{
	// Set robot material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	// Position head with respect to parent (body)
	glTranslatef(0, 0.45*robotBodyLength+0.5*headLength, 0); // this will be done last
	glRotatef(robotAngle, 0.0, 1.0, 0.0);
	// Build Head
	glPushMatrix();
	glScalef(0.4*robotBodyWidth, 0.4*robotBodyWidth, 0.4*robotBodyWidth);
	glutSolidSphere(0.95,100,100);
	glPopMatrix();

	glPopMatrix();
}

void drawEye()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotEye_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotEye_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotEye_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotEye_mat_shininess);

	glPushMatrix();
	// Position head with respect to parent (body)
	glTranslatef(0, 0.45 * robotBodyLength + 0.5 * headLength, 3); // this will be done last
	glRotatef(robotAngle, 0.0, 1.0, 0.0);
	// Build Head
	glPushMatrix();
	glColor3f(1,0,0);
	glScalef(0.4 * robotBodyWidth, 0.4 * robotBodyWidth, 0.4 * robotBodyWidth);
	glutSolidSphere(0.1, 100, 100);
	glPopMatrix();

	glPopMatrix();
}

void drawLowerBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();
	// Position stanchion and base with respect to body
	glTranslatef(0, -1.5*robotBodyLength, 0.0); // this will be done last
	// stanchion
	

	// base
	glPushMatrix();
	// Position base with respect to parent stanchion
	glTranslatef(0.0, -0.25*stanchionLength, 0.0);
	// Build base
	glScalef(baseWidth, baseLength, baseWidth);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawLeftConnector()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
	
	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(2.5, 4, 0);
	glTranslatef(-(0.2 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	glTranslatef((0.2 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);
	// build arm
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	gluCylinder(gluNewQuadric(), 0.8, 0.8, 2, 10, 10);
	glPopMatrix();
	glPopMatrix();
}

void drawRightConnector()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(-4, 4, 0);
	glTranslatef((0.5 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	// build arm
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	gluCylinder(gluNewQuadric(), 0.8, 0.8, 3, 10, 10);
	glPopMatrix();
	glPopMatrix();
}

void drawLeftShoulder()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	// Position shoulder beside the body
	glTranslatef(-5.75, 4, 0);
	glTranslatef((0.6 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);
	glTranslatef(-(0.6 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	// build shoulder
	glPushMatrix();
	glutSolidSphere(2, 100, 100);
	glPopMatrix();

	glPopMatrix();
}

void drawLeftArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
    // Position arm with respect to parent body
	glTranslatef(5.5,4,0); // this will be done last
	glTranslatef(-(0.6 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	glTranslatef((0.6 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);
	glRotatef(shoulderAngle, 1.0, 0.0, 0.0);
	// build arm
	glPushMatrix();
	glRotatef(90.0, 0.0, 0.0, 1.0);
	gluCylinder(gluNewQuadric(), 1, 1.5, 7, 10, 10);
	glPopMatrix();

	glPopMatrix();
}

void drawRightShoulder()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	// Position shoulder beside the body
	glTranslatef(5.75, 4, 0);
	glTranslatef(-(0.6 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	glTranslatef((0.6 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);
	// build shoulder
	glPushMatrix();
	glutSolidSphere(2, 100, 100);
	glPopMatrix();

	glPopMatrix();
}

void drawRightArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(-5.5, 4, 0); // this will be done last
	glTranslatef((0.6 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);
	glTranslatef(-(0.6 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	// build arm
	glPushMatrix();
	glRotatef(90.0, 0.0, 0.0, 1.0);
	gluCylinder(gluNewQuadric(), 1, 1, 7, 10, 10);
	glPopMatrix();

	glPopMatrix();
}

void drawWheel()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(-0.5, -10, 0);

	// build arm
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glutSolidCylinder(4, 1, 100, 100);
	glPopMatrix();
	glPopMatrix();
}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
		shoulderAngle += 2.0;
		if (shoulderAngle < -30)
		{
			shoulderAngle = -30;
		}
		if (shoulderAngle > 20)
		{
			shoulderAngle = 20;
		}
		break;
	case 'w':
		shoulderAngle -= 2.0;
		if (shoulderAngle < -30)
		{
			shoulderAngle = -30;
		}
		if (shoulderAngle > 20)
		{
			shoulderAngle = 20;
		}
		break;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}



void animationHandler(int param)
{
	if (!stop)
	{
		shoulderAngle += 1.0;
		cubeAngle += 2.0;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}



// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("============================INSTRUCTIONS=========================== \n");
		printf("Use LEFT and RIGHT arrows to rotate the entire robot left and right \n");
		printf("Use UP and DOWN arrows are used to move the robot forward and back \n");
		printf("Use W and S to rotate the cannon up and down \n");
	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}
	else if (key == GLUT_KEY_DOWN or key == GLUT_KEY_UP or key == GLUT_KEY_RIGHT or key == GLUT_KEY_LEFT)
	{
		switch (key)
		{
		case GLUT_KEY_UP:
			moveForward += 2.0;
			break;
		case GLUT_KEY_DOWN:
			moveForward -= 2.0;
			break;
		case GLUT_KEY_LEFT:
			robotAngle -= 2.0;
			break;
		case GLUT_KEY_RIGHT:
			robotAngle += 2.0;
			break;
		}
	}
	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

