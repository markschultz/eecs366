// The template code for Assignment 2 
// Jonathan, Mark

//use constants from math.h
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#define ON 1
#define OFF 0
//#define LEFTM 0
//#define MIDDLEM 1
//#define RIGHTM 2

enum {LEFTM,MIDDLEM,RIGHTM};
enum {UP,RIGHT,DOWN,LEFT};

typedef struct CameraPos{
	float x,y,z; //camera pos
	float lx,ly,lz; //looking at
	float ux,uy,uz; //up vertex
};
// Global variables
int window_width, window_height;    // Window dimensions
int PERSPECTIVE = ON;
int AXIS = ON;
int MOUSE = -1;

//change these to change starting orientation/view
float rho = 5.;
float phi = 0.;
float theta = 0.;
CameraPos c;

//simple helper func convert degrees to radians for maths
float toRads(float degrees)
{
	return degrees*(M_PI/180.);
}

// Vertex and Face data structure used in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

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
		//calculate carthesian coords from spherical coords
		c.x = rho*sin(toRads(phi))*cos(toRads(theta));
		c.y = rho*sin(toRads(phi))*sin(toRads(theta));
		c.z = rho*cos(toRads(phi));
		//always looking at origin
		c.lx = c.ly = c.lz = 0.;
		//adjust up
		c.ux = cos(toRads(theta));
		c.uy = sin(toRads(theta));
		c.uz = 1.;
		// Set the camera position, orientation and target
		gluLookAt(c.x,c.y,c.z,  c.lx,c.ly,c.lz,  c.ux,c.uy,c.uz);
		//gluLookAt(0,5,5, 0,-2,0, 0,1,0);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(AXIS){
    // Draw a red line
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,1.0);
		glEnd();

	//Draw a blue Line
		glColor3f(0,0,1);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,1.0,0.0);
		glEnd();

    // Draw a green line
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3f(1.0,0.0,0.0);
			glVertex3f(0.0,0.0,0.0);
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
	if (previousX-x>0) DIRECTION = DOWN; else DIRECTION = UP;
	if (previousY-y>0) DIRECTION = RIGHT; else DIRECTION = LEFT;
	
}

//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	printf("Mouse is at %d, %d with button %d\n", x,y,MOUSE);
	if (MOUSE==RIGHTM)
	{

}

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':                           /* Quit */
		exit(1);
		break;
    case 'a':
		if(AXIS)
			AXIS = OFF;
		else
			AXIS = ON;
		break;
    case 'P':
	// Toggle Projection Type (orthogonal, perspective)
        if(PERSPECTIVE) {
			// Orthogonal Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
			glutSetWindowTitle("Assignment 2 Template (orthogonal)");
			PERSPECTIVE = OFF;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		} else {
			// Perpective Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
			glutSetWindowTitle("Assignment 2 Template (perspective)");
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