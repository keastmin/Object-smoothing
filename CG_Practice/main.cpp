#include<stdio.h>
#include<stdlib.h>
#include "GL/glut.h"
#include"Mesh.h"

Mesh* mesh = NULL;

float zoom = 15.0f;
float rot_x = 0;
float rot_y = 0;
float trans_x = 0;
float trans_y = 0;
int _last_x = 0;
int _last_y = 0;
unsigned char buttons[3] = { 0 };

int _renderMode = 3;

void Init() {
	glEnable(GL_DEPTH_TEST);
}

void Draw() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	switch (_renderMode) {
	case 0:
		mesh->drawPoint();
		break;
	case 1:
		mesh->drawWire();
		break;
	case 2:
		mesh->drawSurface();
		break;
	case 3:
		mesh->drawSurface(true);
		break;
	default:
		break;
	}
	glDisable(GL_LIGHTING);
}

void Display() {
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0, 0, -zoom);
	glTranslatef(trans_x, trans_y, 0);
	glRotatef(rot_x, 1, 0, 0);
	glRotatef(rot_y, 0, 1, 0);

	Draw();
	glutSwapBuffers();
}

void Reshape(int w, int h) {
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
	case 'p':
	case 'P':
		_renderMode = 0;
		break;
	case 'w':
	case 'W':
		_renderMode = 1;
		break;
	case 's':
	case 'S':
		_renderMode = 2;
		break;
	case 'a':
	case 'A':
		_renderMode = 3;
		break;
	case 'f':
	case 'F':
		mesh->smoothing(5);
		break;
	case 'l':
	case 'L':
		mesh->laplacianSmoothing(5, 0.6);		
		break;
	case 't':
	case 'T':
		//mesh->tauTest(5, -0.5);
		mesh->taubinSmoothing(5, 0.6, -0.5);
		break;
	case 'c':
	case 'C':
		mesh->cotanSmoothing(5);
		break;
	case 'r':
	case 'R':
		mesh->reset();
		break;
	default:
		break;
	}
	glutPostRedisplay(); // 화면갱신
}

void Mouse(int button, int state, int x, int y) {
	_last_x = x;
	_last_y = y;

	switch (button) {
	case GLUT_LEFT_BUTTON:
		buttons[0] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_MIDDLE_BUTTON:
		buttons[1] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_RIGHT_BUTTON:
		buttons[2] = state == GLUT_DOWN ? 1 : 0;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void Motion(int x, int y) {
	int diff_x = x - _last_x;
	int diff_y = y - _last_y;
	_last_x = x;
	_last_y = y;

	if (buttons[0]) {
		rot_x += (float)0.2f * diff_y;
		rot_y += (float)0.2f * diff_x;
	}
	else if (buttons[1]) {
		trans_x += (float)0.02f * diff_x;
		trans_y -= (float)0.02f * diff_y;
	}
	else if (buttons[2]) {
		zoom -= (float)0.02f * diff_x;
	}

	glutPostRedisplay();
}

void main(int argc, char** argv) {
	mesh = new Mesh("OBJ\\dragon.obj");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Practice1");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	Init();
	glutMainLoop();
}