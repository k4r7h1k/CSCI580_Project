/*$T \main.cpp GC 1.150 2013-11-27 14:21:09 */


/*$6*/



#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <iostream>
#include "Character.h"

#define KEY_ESCAPE	27

using namespace std;

/************************************************************************
  Window
 ************************************************************************/
typedef struct
{
	int		width;
	int		height;
	char	*title;

	float	field_of_view_angle;
	float	z_near;
	float	z_far;
} glutWindow;
CharacterModel	obj;
float			g_rotation;
glutWindow		win;

/* */
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	GzCoord c1, c2, c3, n1, n2, n3;
	gluLookAt(0, 1, 5, 0, 0, 0, 0, 1, 0);
	glPushMatrix();
	glColor3f(.6, .6, .6);
	//glTranslatef(0, -obj.characterMesh.pY, -obj.characterMesh.pZ);
	glRotatef(g_rotation, 0, 1, 0);
	obj.moveCharacter();
	//glRotatef(90,0,1,0);
	//g_rotation += 5;
	obj.calculateMotionInverse();
	glBegin(GL_TRIANGLES);
	int trigNum=obj.getTriangleNumber();
	for(int i = 0; i < trigNum; i=i+1)
	{
		obj.getVertexes(i, c1, c2, c3, n1, n2, n3);
		glNormal3f(-n1[0], -n1[1], -n1[2]);
		glVertex3f(c1[0], c1[1], c1[2]);
		glNormal3f(-n2[0], -n2[1], -n2[2]);
		glVertex3f(c2[0], c2[1], c2[2]);
		glNormal3f(-n1[0], -n2[1], -n3[2]);
		glVertex3f(c3[0], c3[1], c3[2]);
	}

	glEnd();
	/*glBegin(GL_LINES);
	glVertex3f(-0.0881862,-0.223678,-0.929536); //1
	glVertex3f(-0.566275,-0.541542,-0.899938);

	glVertex3f(-0.566275,-0.541542,-0.899938);
	glVertex3f(-0.616166,-1.19613,-1.0551);

	glVertex3f(-0.616166,-1.19613,-1.0551);
	glVertex3f(-0.614267,-1.74914,-1.16717);
	
	glVertex3f(-0.614267,-1.74914,-1.16717);
	glVertex3f(-0.86033,-1.84008,-0.670307);
	
	glVertex3f(-0.0881862,-0.223678,-0.929536); //1
	glVertex3f(-0.0871545,0.0924213,-0.927642);

	glVertex3f(-0.0871545,0.0924213,-0.927642);
	glVertex3f(-0.0640778,0.577047,-0.846801);

	glVertex3f(-0.0640778,0.577047,-0.846801);
	glVertex3f(-0.113935,1.09478,-0.921801);
	
	glVertex3f(-0.0640778,0.577047,-0.846801);
	glVertex3f(0.267937,0.551273,-0.789762);

	glVertex3f(0.267937,0.551273,-0.789762);
	glVertex3f(0.578372,0.536381,-0.666565);

	glVertex3f(0.578372,0.536381,-0.666565);
	glVertex3f(0.824687,0.573724,-0.517252);

	glVertex3f(0.824687,0.573724,-0.517252);
	glVertex3f(1.07611,0.735785,-0.0939775);

	glVertex3f(1.07611,0.735785,-0.0939775);
	glVertex3f(1.21808,0.700675,0.158607);
	
	glVertex3f(-0.0640778,0.577047,-0.846801);
	glVertex3f(-0.370623,0.532531,-0.852921);

	glVertex3f(-0.370623,0.532531,-0.852921);
	glVertex3f(-0.675322,0.48007,-0.770522);

	glVertex3f(-0.675322,0.48007,-0.770522);
	glVertex3f(-0.936812,0.474334,-0.677436);

	glVertex3f(-0.936812,0.474334,-0.677436);
	glVertex3f(-1.26404,0.784372,-0.351252);

	glVertex3f(-1.26404,0.784372,-0.351252);
	glVertex3f(-1.45527,0.892911,0.0528238);

	glVertex3f(-0.0881862,-0.223678,-0.929536); //1
	glVertex3f(0.3686,-0.428869,-0.978191);

	glVertex3f(0.3686,-0.428869,-0.978191);
	glVertex3f(0.617329,-1.04685,-0.899962);

	glVertex3f(0.617329,-1.04685,-0.899962);
	glVertex3f(0.593214,-1.71593,-1.08845);

	glVertex3f(0.593214,-1.71593,-1.08845);
	glVertex3f(0.982818,-1.82164,-0.622992);
	glEnd();*/
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

/* */
void initialize()
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, win.width, win.height);

	GLfloat aspect = (GLfloat) win.width / win.height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);

	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.1f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLfloat amb_light[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

/* */
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case KEY_ESCAPE:	exit(0); break;
	default:			break;
	}
}

/* */
int main(int argc, char **argv)
{
	// set window values
	win.width = 640;
	win.height = 480;
	win.title = "Character Animation";
	obj.loadModel("arma2.obj", "skeleton2.out", "attachment2.out");

	float	fdist = obj.characterMesh.pR / 0.57735f;
	win.field_of_view_angle = 45;
	win.z_near = 1;			// fdist-obj.characterMesh.pR;
	win.z_far = 550;
	fdist + obj.characterMesh.pR;

	// initialize and run program
	glutInit(&argc, argv);	// GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// Display Mode
	glutInitWindowSize(win.width, win.height);					// set window size
	glutCreateWindow(win.title);	// create Window
	glutDisplayFunc(display);		// register Display Function
	glutIdleFunc(display);			// register Idle Function
	glutKeyboardFunc(keyboard);		// register Keyboard Handler

	initialize();
	//	obj.Load("arma2.obj");
	glutMainLoop();					// run GLUT mainloop
	return 0;
}
