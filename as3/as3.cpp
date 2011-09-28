// The template code for Assignment 2 
//

#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#define ON 1
#define OFF 0

int MOUSE = -1;
enum {LEFTM,MIDDLEM,RIGHTM};
enum {UP,RIGHT,DOWN,LEFT};
double Rotx = 0.0;
double Roty = 0.0;
double Rotz = 0.0;
double Roty2 = 0.0;
double Rotx2 = 0.0;
double Rotz2 = 0.0;
// Global variables
int window_width, window_height;    // Window dimensions
int PERSPECTIVE = OFF;
int VERTICAL = -1;
int HORIZONTAL = -1;
int previousX=150,previousY=150;

// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

class wcPt3D {
public:
	wcPt3D (GLfloat,GLfloat,GLfloat);
	GLfloat x,y,z;
};
wcPt3D::wcPt3D (GLfloat ix,GLfloat iy,GLfloat iz) {
	x=ix;
	y=iy;
	z=iz;
}

 typedef GLfloat WorldMatrix[4][4];
 WorldMatrix MotionMatrix;
 WorldMatrix TheBegining;
 wcPt3D WorldOrigin (0,0,0);
 wcPt3D camera (0,0,0);

 void rotateX(GLfloat, WorldMatrix);
 void rotateY(GLfloat, WorldMatrix, WorldMatrix);
 void rotateZ(GLfloat, WorldMatrix);
 void scale3D(GLfloat, GLfloat, GLfloat, wcPt3D, WorldMatrix);
 void myLookAt(wcPt3D,wcPt3D,wcPt3D);

 void matrix4x4SetIdentity (WorldMatrix matIdent4x4)
 {
	 GLint row, col;
	
		for(row =0; row<4; row++)
			for(col = 0; col < 4; col++)
				matIdent4x4 [row][col] = 0;

		matIdent4x4[0][0] = 1;
		matIdent4x4[1][1] = 1;
		matIdent4x4[2][2] = 1;
		matIdent4x4[3][3] = 1;
 }


typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List

// The mesh reader itself
// It can read *very* simple obj files
void meshReader (char *filename,int sign)
{
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) { 
	printf("Cannot open %s\n!", filename);
	exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
	{
	  fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
	  if (letter == 'v')
	{
	  verts++;
	}
	  else
	{
	  faces++;
	}
	}

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = (point *)malloc(sizeof(point)*verts);
  normList = (point *)malloc(sizeof(point)*verts);

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < verts;i++)
	{
	  fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
	  vertList[i].x = x;
	  vertList[i].y = y;
	  vertList[i].z = z;
	}

  // Read the faces
  for(i = 0;i < faces;i++)
	{
	  fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
	  faceList[i].v1 = ix - 1;
	  faceList[i].v2 = iy - 1;
	  faceList[i].v3 = iz - 1;
	}
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++)
	{
	  normList[i].x = normList[i].y = normList[i].z = 0.0;
	  normCount[i] = 0;
	}

  for(i = 0;i < faces;i++)
	{
	  v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
	  v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
	  v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
	  v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
	  v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
	  v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

	  crossP.x = v1.y*v2.z - v1.z*v2.y;
	  crossP.y = v1.z*v2.x - v1.x*v2.z;
	  crossP.z = v1.x*v2.y - v1.y*v2.x;

	  len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

	  crossP.x = -crossP.x/len;
	  crossP.y = -crossP.y/len;
	  crossP.z = -crossP.z/len;

	  normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
	  normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
	  normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
	  normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
	  normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
	  normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
	  normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
	  normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
	  normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
	  normCount[faceList[i].v1]++;
	  normCount[faceList[i].v2]++;
	  normCount[faceList[i].v3]++;
	}
  for (i = 0;i < verts;i++)
	{
	  normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
	  normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
	  normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
	}

}

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void)
{
	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
	if (PERSPECTIVE) {
		glLoadIdentity();
		// Set the camera position, orientation and target
		//myLookAt(0,0,5, 0,0,0, 0,1,0);
		myLookAt(wcPt3D(0,0,5),wcPt3D(0,0,0),wcPt3D(0,1,0));
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(MotionMatrix[0][3],MotionMatrix[1][3],MotionMatrix[2][3]);
		glVertex3f(MotionMatrix[0][0]+MotionMatrix[0][3],MotionMatrix[1][0] + MotionMatrix[1][3], 1 + MotionMatrix[2][0]+ MotionMatrix[2][3]);
	glEnd();

	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(MotionMatrix[0][3],MotionMatrix[1][3],MotionMatrix[2][3]);
		glVertex3f(MotionMatrix[0][1]+MotionMatrix[0][3], 1 + MotionMatrix[1][1]+ MotionMatrix[1][3],MotionMatrix[2][1]+ MotionMatrix[2][3]);
	glEnd();

	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(MotionMatrix[0][3],MotionMatrix[1][3],MotionMatrix[2][3]);
		glVertex3f(1 + MotionMatrix[0][2]+MotionMatrix[0][3],MotionMatrix[1][2]+ MotionMatrix[1][3],MotionMatrix[2][2]+ MotionMatrix[2][3]);
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(WorldOrigin.x,WorldOrigin.y,WorldOrigin.z);
		glVertex3f(WorldOrigin.x+3,WorldOrigin.y,WorldOrigin.z);
	glEnd();

	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(WorldOrigin.x,WorldOrigin.y,WorldOrigin.z);
		glVertex3f(WorldOrigin.x,WorldOrigin.y+3,WorldOrigin.z);
	glEnd();

	// Draw a green line
	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(WorldOrigin.x,WorldOrigin.y,WorldOrigin.z);
		glVertex3f(WorldOrigin.x,WorldOrigin.y,WorldOrigin.z+3);
	glEnd();


	for(int i = 0; i < faces; i++)
		{
			glBegin(GL_TRIANGLES);	
			glColor3f(1.0f,0.0f,0.0f);
			glVertex3f(vertList[faceList[i].v1].x * MotionMatrix [0][0] + vertList[faceList[i].v1].x * MotionMatrix [0][1] + vertList[faceList[i].v1].x * MotionMatrix[0][2] + MotionMatrix[0][3], vertList[faceList[i].v1].y * MotionMatrix[1][0] + vertList[faceList[i].v1].y * MotionMatrix[1][1] + vertList[faceList[i].v1].y * MotionMatrix[1][2] + MotionMatrix[1][3] ,vertList[faceList[i].v1].z * MotionMatrix[2][0] + vertList[faceList[i].v1].z * MotionMatrix[2][1] + vertList[faceList[i].v1].z * MotionMatrix[2][2] + MotionMatrix[2][3]);
				glColor3f(1.0f,0.0f,0.0f);
			glVertex3f(vertList[faceList[i].v2].x * MotionMatrix [0][0] + vertList[faceList[i].v2].x * MotionMatrix[0][1] + vertList[faceList[i].v2].x * MotionMatrix[0][2]+ MotionMatrix[0][3],vertList[faceList[i].v2].y * MotionMatrix[1][0] + vertList[faceList[i].v2].y * MotionMatrix[1][1] + vertList[faceList[i].v2].y * MotionMatrix[1][2] + MotionMatrix[1][3], vertList[faceList[i].v2].z * MotionMatrix[2][0] + vertList[faceList[i].v2].z * MotionMatrix[2][1] + vertList[faceList[i].v2].z * MotionMatrix[2][2] + MotionMatrix[2][3]);
				glColor3f(1.0f,0.0f,0.0f);
			glVertex3f(vertList[faceList[i].v3].x * MotionMatrix [0][0] + vertList[faceList[i].v3].x * MotionMatrix[0][1] + vertList[faceList[i].v3].x * MotionMatrix[0][2]+ MotionMatrix[0][3],vertList[faceList[i].v3].y * MotionMatrix[1][0] + vertList[faceList[i].v3].y * MotionMatrix[1][1] + vertList[faceList[i].v3].y * MotionMatrix[1][2] + MotionMatrix[1][3], vertList[faceList[i].v3].z * MotionMatrix[2][0] + vertList[faceList[i].v3].z * MotionMatrix[2][1] + vertList[faceList[i].v3].z * MotionMatrix[2][2] + MotionMatrix[2][3]);
			glEnd();
		}

	// (Note that the origin is lower left corner)
	// (Note also that the window spans (0,1) )
	// Finish drawing, update the frame buffer, and swap buffers
	glutSwapBuffers();
}

// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y)
{
	glViewport(0,0,x,y);
	window_width = x;
	window_height = y;
	if (PERSPECTIVE) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	printf("Resized to %d %d\n",x,y);
}

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
	printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);
	if (button == LEFTM) MOUSE = LEFTM;
	if (button == MIDDLEM) MOUSE = MIDDLEM;
	if (button == RIGHTM) MOUSE = RIGHTM;
}

//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	wcPt3D up (0,1,0);
	if (previousY-y>0) VERTICAL = UP; else VERTICAL = DOWN;
	if (previousX-x>0) HORIZONTAL = LEFT; else HORIZONTAL = RIGHT;
	printf("Mouse is at %d, %d with button %d in direction %d,%d\n", x,y,MOUSE,VERTICAL,HORIZONTAL);
	previousX=x;
	previousY=y;

	//left mouse   - rotate around up axis(viewing yaxis)
	//middle mouse - translate along xy plane
	//right mouse  - translate along -z
	if (MOUSE==RIGHTM){
		/*if(VERTICAL==UP) 
		else if(VERTICAL==DOWN) */
	}
	else if (MOUSE==LEFTM){
		if(HORIZONTAL==RIGHT) rotate3D(camera, up, .1745, MotionMatrix);
		else if(HORIZONTAL==LEFT) rotate3D(camera, up, -.1745, MotionMatrix);
	}
	else if (MOUSE==MIDDLEM){
	/*	if (VERTICAL==UP) 
		else if (VERTICAL==DOWN) 
		if (HORIZONTAL==RIGHT) 
		else if (HORIZONTAL==LEFT) */
	}
}

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
	wcPt3D pt3(MotionMatrix[0][3],MotionMatrix[1][3],MotionMatrix[2][3]);
	switch(key) {
	case '':                           /* Quit */
		exit(1);
		break;
	case '4':
		MotionMatrix[0][3]--;
		break;
	case '6':
		MotionMatrix[0][3]++;
		break;
	case '2':
		MotionMatrix[1][3]--;
		break;
	case '8':
		MotionMatrix[1][3]++;
		break;
	case '9':
		MotionMatrix[2][3]++;
		break;
	case '1':
		MotionMatrix[2][3]--;
		break;
	case '[':
		Roty = MotionMatrix[1][3];
		MotionMatrix[1][3] = MotionMatrix[1][3]*cos(.1745329) - MotionMatrix[2][3]*sin(.1745329);
		MotionMatrix[2][3] = Roty*sin(.1745329) + MotionMatrix[2][3]*cos(.1745329);
		break;
	case ']':
		Roty = MotionMatrix[1][3];
		MotionMatrix[1][3] = MotionMatrix[1][3]*cos(-.1745329) - MotionMatrix[2][3]*sin(-.1745329);
		MotionMatrix[2][3] = Roty*sin(-.1745329) + MotionMatrix[2][3]*cos(-.1745329);
		break;
	case ';':
		Rotz = MotionMatrix[2][3];
		MotionMatrix[2][3] = Rotz*cos(.1745329) - MotionMatrix[0][3]*sin(.1745329);
		MotionMatrix[0][3] = Rotz*sin(.1745329) + MotionMatrix[0][3]*cos(.1745329); 
		break;
	case '\'':
		Rotz = MotionMatrix[2][3];
		MotionMatrix[2][3] = Rotz*cos(-.1745329) - MotionMatrix[0][3]*sin(-.1745329);
		MotionMatrix[0][3] = Rotz*sin(-.1745329) + MotionMatrix[0][3]*cos(-.1745329); 
		break;
	case '.':
		Rotx = MotionMatrix[0][3];
		MotionMatrix[0][3] = Rotx*cos(.1745329) - MotionMatrix[1][3]*sin(.1745329);
		MotionMatrix[1][3] = Rotx*sin(.1745329) + MotionMatrix[1][3]*cos(.1745329);
		break;
	case '/':
		Rotx = MotionMatrix[0][3];
		MotionMatrix[0][3] = Rotx*cos(-.1745329) - MotionMatrix[1][3]*sin(-.1745329);
		MotionMatrix[1][3] = Rotx*sin(-.1745329) + MotionMatrix[1][3]*cos(-.1745329);
		break;

	case '=':		
		scale3D(1.25,1.25,1.25,pt3,MotionMatrix);
		break;
	case'-':
		pt3.x = MotionMatrix[0][3];
		pt3.y = MotionMatrix[1][3];
		pt3.z = MotionMatrix[2][3];
		scale3D(.75,.75 ,.75,pt3,MotionMatrix);
		break;
	case'i':
		Rotx = MotionMatrix[0][3];
		Roty = MotionMatrix[1][3];
		Rotz = MotionMatrix[2][3];
		MotionMatrix[0][3] = 0;
		MotionMatrix[1][3] = 0;
		MotionMatrix[2][3] = 0;
	rotateX(-.1745329, MotionMatrix);
		MotionMatrix[0][3] = Rotx;
		MotionMatrix[1][3] = Roty;
		MotionMatrix[2][3] = Rotz;
		break;
	case'o':
		Rotx = MotionMatrix[0][3];
		Roty = MotionMatrix[1][3];
		Rotz = MotionMatrix[2][3];
		MotionMatrix[0][3] = 0;
		MotionMatrix[1][3] = 0;
		MotionMatrix[2][3] = 0;
	rotateX(.1745329, MotionMatrix);
		MotionMatrix[0][3] = Rotx;
		MotionMatrix[1][3] = Roty;
		MotionMatrix[2][3] = Rotz;
		break;
	case'k':
		//Rotx = MotionMatrix[0][3];
		//Roty = MotionMatrix[1][3];
		//Rotz = MotionMatrix[2][3];
		//MotionMatrix[0][3] = 0;
		//MotionMatrix[1][3] = 0;
		//MotionMatrix[2][3] = 0;
	rotateY(-.1745329, MotionMatrix, TheBegining);
	matrix4x4SetIdentity(TheBegining);
		//MotionMatrix[0][3] = Rotx;
		//MotionMatrix[1][3] = Roty;
		//MotionMatrix[2][3] = Rotz;
		break;
	case'l':
		//Rotx = MotionMatrix[0][3];
		//Roty = MotionMatrix[1][3];
		//Rotz = MotionMatrix[2][3];
		//MotionMatrix[0][3] = 0;
		//MotionMatrix[1][3] = 0;
		//MotionMatrix[2][3] = 0;
	rotateY(.1745329, MotionMatrix, TheBegining);
	matrix4x4SetIdentity(TheBegining);
		//MotionMatrix[0][3] = Rotx;
		//MotionMatrix[1][3] = Roty;
		//MotionMatrix[2][3] = Rotz;
		break;
	case'm':
		Rotx = MotionMatrix[0][3];
		Roty = MotionMatrix[1][3];
		Rotz = MotionMatrix[2][3];
		MotionMatrix[0][3] = 0;
		MotionMatrix[1][3] = 0;
		MotionMatrix[2][3] = 0;
	rotateZ(.1745329, MotionMatrix);
		MotionMatrix[0][3] = Rotx;
		MotionMatrix[1][3] = Roty;
		MotionMatrix[2][3] = Rotz;
		break;
	case',':
		Rotx = MotionMatrix[0][3];
		Roty = MotionMatrix[1][3];
		Rotz = MotionMatrix[2][3];
		MotionMatrix[0][3] = 0;
		MotionMatrix[1][3] = 0;
		MotionMatrix[2][3] = 0;
	rotateZ(.1745329, MotionMatrix);
		MotionMatrix[0][3] = Rotx;
		MotionMatrix[1][3] = Roty;
		MotionMatrix[2][3] = Rotz;
		break;
	case 'P':
	// Toggle Projection Type (orthogonal, perspective)
		if(PERSPECTIVE) {
			// Orthogonal Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
			glutSetWindowTitle("Assignment 3 Template (orthogonal)");
			PERSPECTIVE = OFF;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		} else {
			// Perpective Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
			glutSetWindowTitle("Assignment 3 Template (perspective)");
			PERSPECTIVE = ON;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		break;
	default:
		break;
	}

	// Schedule a new display event
	glutPostRedisplay();
}

// Here's the main
int main(int argc, char* argv[])
{
	// Initialize GLUT
	meshReader("tetrahedron.obj",0);
	matrix4x4SetIdentity(MotionMatrix);
	matrix4x4SetIdentity(TheBegining);
	WorldOrigin.x = WorldOrigin.y = WorldOrigin.z = 0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Assignment 2 Template (orthogonal)");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);

	// Initialize GL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	// Switch to main loop
	glutMainLoop();
	return 0;        
}

WorldMatrix matComposite;

void matrix4x4Multiply(WorldMatrix m1, WorldMatrix m2){
	GLint row, col;
	WorldMatrix matTemp;

	for(row = 0;row<4;row++){
		for(col=0;col<4;col++){
			matTemp[row][col] = m1[row][0]*m2[0][col]+m1[row][1]*m2[1][col]+m1[row][2]*m2[2][col]+m1[row][3]*m2[3][col];
		}
	}
	for(row = 0;row<4;row++){
		for(col=0;col<4;col++){
			m2[row][col]=matTemp[row][col];
		}
	}
}
void translate3D(GLfloat tx,GLfloat ty,GLfloat tz, WorldMatrix matM) {
	WorldMatrix matTrans;

	matrix4x4SetIdentity(matTrans);

	matTrans[0][3]=tx;
	matTrans[1][3]=ty;
	matTrans[2][3]=tz;

	matrix4x4Multiply(matTrans,matM);
}
void scale3D(GLfloat sx, GLfloat sy, GLfloat sz, wcPt3D fixedPt, WorldMatrix matM){
	WorldMatrix matScale;
	matrix4x4SetIdentity(matScale);

	matScale[0][0] = sx;
	matScale[0][3] = (1-sx)*fixedPt.x;
	matScale[1][1] = sy;
	matScale[1][3] = (1-sy)*fixedPt.y;
	matScale[2][2] = sz;
	matScale[2][3] = (1-sz)*fixedPt.z;

	matrix4x4Multiply(matScale,matM);
}
void rotate3D(wcPt3D p1, wcPt3D p2, GLfloat radianAngle, WorldMatrix matM){
	WorldMatrix matRot;
	GLfloat axisVectLength = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) + (p2.z - p1.z)*(p2.z - p1.z));
	GLfloat cosA = cos(radianAngle);
	GLfloat oneC = 1 - cosA;
	GLfloat sinA = sin(radianAngle);
	GLfloat ux = (p2.x - p1.x) / axisVectLength;
	GLfloat uy = (p2.y - p1.y) / axisVectLength;
	GLfloat uz = (p2.z - p1.z) / axisVectLength;

	//set up translation matrix by moving p1 to origin
	translate3D(-p1.x, -p1.y, -p1.z, matM);

	//initialize matRot to identity
	matrix4x4SetIdentity(matRot);

	matRot[0][0] = ux*ux*oneC + cosA;
	matRot[0][1] = ux*uy*oneC - uz*sinA;
	matRot[0][2] = ux*uz*oneC + uy*sinA;
	matRot[1][0] = uy*ux*oneC + uz*sinA;
	matRot[1][1] = uy*uy*oneC + cosA;
	matRot[1][2] = uy*uz*oneC - ux*sinA;
	matRot[2][0] = uz*ux*oneC - uy*sinA;
	matRot[2][1] = uz*uy*oneC + ux*sinA;
	matRot[2][2] = uz*uz*oneC + sinA;

	matrix4x4Multiply(matRot,matM);

	//inverse initial translation and multiply with first 2
	translate3D(p1.x, p1.y, p1.z, matM);
}
void rotateX(GLfloat radians, WorldMatrix matJ)
{
	WorldMatrix Rx;
	matrix4x4SetIdentity(Rx);
	Rx[1][1] = cos(radians);
	Rx[1][2] = -sin(radians);
	Rx[2][1] = sin(radians);
	Rx[2][2] = cos(radians);

	matrix4x4Multiply(Rx, matJ);

}
void rotateY(GLfloat radians, WorldMatrix matK, WorldMatrix Original)
{
	WorldMatrix Ry;
	matrix4x4SetIdentity(Ry);
	Ry[0][0] = cos(radians);
	Ry[0][2] = sin(radians);
	Ry[2][0] = -sin(radians);
	Ry[2][2] = cos(radians);
	matrix4x4Multiply(Ry, Original);
	matrix4x4Multiply(Original, matK);
}
void rotateZ(GLfloat radians, WorldMatrix matI)
{
	WorldMatrix Rz;
	matrix4x4SetIdentity(Rz);
	Rz[0][0] = cos(radians);
	Rz[0][1] = -sin(radians);
	Rz[1][0] = sin(radians);
	Rz[1][1] = cos(radians);

	matrix4x4Multiply(Rz, matI);
}

//viewing shit


wcPt3D u (0,0,0);
wcPt3D v (0,0,0);
wcPt3D n (0,0,0);


void myLookAt(wcPt3D eye, wcPt3D center, wcPt3D up) { //eye is camera loc, center is look at pt
	WorldMatrix Mvw;
	matrix4x4SetIdentity(Mvw);

	// p2-p1 for n
	n.x = eye.x - center.x;
	n.y = eye.y - center.y;
	n.z = eye.z - center.z;
	
	// norm
	GLfloat N = sqrt(pow(n.x,2)+pow(n.y,2)+pow(n.z,2));

	// n/norm(n)
	n.x /= N;
	n.y /= N;
	n.z /= N;

	//p2-p1 for v
	v.x = up.x - eye.x;
	v.y = up.y - eye.y;
	v.z = up.z - eye.z;

	//init u 
	u.x = (v.y*(-n.z))-(v.z*(-n.y));
	u.y = (v.x*(-n.z))-(v.z*(-n.x));
	u.z = (v.x*(-n.y))-(v.y*(-n.x));

	//VxN
	GLfloat VN = sqrt(pow((v.y*(-n.z))-(v.z*(-n.y)),2)+pow((v.x*(-n.z))-(v.z*(-n.x)),2)+pow((v.x*(-n.y))-(v.y*(-n.x)),2));

	//VxN/norm(VxN)
	u.x /= VN;
	u.y /= VN;
	u.z /= VN;

	//nxu
	v.x = (n.y*u.z)-(n.z*u.y);
	v.y = (n.x*u.z)-(n.z*u.x);
	v.z = (n.x*u.y)-(n.y*u.x);

	//now for m_vw (world frame in viewing coords)
	Mvw[0][0] = u.x;
	Mvw[0][1] = u.y;
	Mvw[0][2] = u.z;
	Mvw[0][3] = eye.x*u.x+eye.y*u.y+eye.z*u.z;
	Mvw[1][0] = v.x;
	Mvw[1][1] = v.y;
	Mvw[1][2] = v.z;
	Mvw[1][3] = eye.x*v.x+eye.y*v.y+eye.z*v.z;
	Mvw[2][0] = n.x;
	Mvw[2][1] = n.y;
	Mvw[2][2] = n.z;
	Mvw[2][3] = eye.x*n.x+eye.y*n.y+eye.z*n.z;

	//multiply world motion by M_vw
	matrix4x4Multiply(Mvw,MotionMatrix);

	//move the origin to eye
	WorldOrigin.x = eye.x;
	WorldOrigin.y = eye.y;
	WorldOrigin.z = eye.z;
}
