/************************************************************************
     File:        ControlPoint.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Data structure for control points

						These are not just points in space (see Point3D), 
						because they may have
						an orientation associated with them. For most people, 
						the orientation will
						just be ignored. However, if you try some advanced 
						features, you might find the orientation vectors handy.

						I assume the orientation points UP 
						(the positive Y axis), so that's the default.
						When things get drawn, the point "points" in that 
						direction

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <windows.h>
//#include <GL/gl.h>
#include <math.h>

#include "ControlPoint.H"
#include "Utilities/3dUtils.h"

//****************************************************************************
//
// * Default contructor
//============================================================================
ControlPoint::
ControlPoint() 
	: pos(0,0,0), orient(0,1,0)
//============================================================================
{
}

//****************************************************************************
//
// * Set up the position and set orientation to default (0, 1, 0)
//============================================================================
ControlPoint::
ControlPoint(const Pnt3f &_pos) 
	: pos(_pos), orient(0,1,0)
//============================================================================
{
}

//****************************************************************************
//
// * Set up the position and orientation
//============================================================================
ControlPoint::
ControlPoint(const Pnt3f &_pos, const Pnt3f &_orient) 
	: pos(_pos), orient(_orient)
//============================================================================
{
	orient.normalize();
}

//****************************************************************************
//
// * Draw the control point
//============================================================================
void ControlPoint::
draw(bool onlyPost, int s)
//============================================================================
{
	if (onlyPost == false)
	{
		float size = 2.0;

		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		float theta1 = -radiansToDegrees(atan2(orient.z, orient.x));
		glRotatef(theta1, 0, 1, 0);
		float theta2 = -radiansToDegrees(acos(orient.y));
		glRotatef(theta2, 0, 0, 1);

		glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glVertex3f(size, size, size);
		glVertex3f(-size, size, size);
		glVertex3f(-size, -size, size);
		glVertex3f(size, -size, size);

		glNormal3f(0, 0, -1);
		glVertex3f(size, size, -size);
		glVertex3f(size, -size, -size);
		glVertex3f(-size, -size, -size);
		glVertex3f(-size, size, -size);

		// no top - it will be the point

		glNormal3f(0, -1, 0);
		glVertex3f(size, -size, size);
		glVertex3f(-size, -size, size);
		glVertex3f(-size, -size, -size);
		glVertex3f(size, -size, -size);

		glNormal3f(1, 0, 0);
		glVertex3f(size, size, size);
		glVertex3f(size, -size, size);
		glVertex3f(size, -size, -size);
		glVertex3f(size, size, -size);

		glNormal3f(-1, 0, 0);
		glVertex3f(-size, size, size);
		glVertex3f(-size, size, -size);
		glVertex3f(-size, -size, -size);
		glVertex3f(-size, -size, size);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0, 1.0f, 0);
		glVertex3f(0, 3.0f * size, 0);
		glNormal3f(1.0f, 0.0f, 1.0f);
		glVertex3f(size, size, size);
		glNormal3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(-size, size, size);
		glNormal3f(-1.0f, 0.0f, -1.0f);
		glVertex3f(-size, size, -size);
		glNormal3f(1.0f, 0.0f, -1.0f);
		glVertex3f(size, size, -size);
		glNormal3f(1.0f, 0.0f, 1.0f);
		glVertex3f(size, size, size);
		glEnd();
		glPopMatrix();
		//drawPost(Pnt3f(pos.x, 0, pos.z + 0.1), pos, 1);
	}
	else
	{
		drawPost(Pnt3f(pos.x, 0, pos.z + 0.1), pos, 8, s);
	}

}

//****************************************************************************
//
// * Draw post of control point
//============================================================================
void ControlPoint::drawPost(const Pnt3f& a, const Pnt3f& b, GLdouble radius, int slices)
{
	Pnt3f dir = b - a;
	GLdouble  bone_length = dir.length();
	static GLUquadricObj* quad_obj = NULL;
	if (quad_obj == NULL)
	{
		quad_obj = gluNewQuadric();
	}
	gluQuadricDrawStyle(quad_obj, GLU_FILL);
	gluQuadricNormals(quad_obj, GLU_SMOOTH);
	glPushMatrix();
	glTranslated(a.x, a.y, a.z);
	dir.normalize();
	Pnt3f up(0, 1, 0), side = up * dir;
	side.normalize();
	up = dir * side;
	GLdouble  m[16] = { side.x, side.y, side.z, 0.0, up.x,  up.y,  up.z, 0.0, dir.x, dir.y,  dir.z,  0.0,0.0,   0.0,  0.0, 1.0 };
	glMultMatrixd(m);
	//GLdouble slices = 8.0;
	GLdouble stack = 3.0;
	glColor3d(0.5, 0.5, 0.5);
	gluCylinder(quad_obj, radius, radius, bone_length, slices, stack);
	glPopMatrix();
}