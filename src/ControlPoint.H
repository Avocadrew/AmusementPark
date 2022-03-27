/************************************************************************
	 File:        ControlPoint.H

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
#pragma once

#include "Utilities/Pnt3f.H"
#include <Fl/fl.h>
#include <FL/fl_ask.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"
#include <math.h>

class ControlPoint {
public:
	// constructors
	// need a default constructor for making arrays
	ControlPoint();

	// create in a position
	ControlPoint(const Pnt3f& pos);

	// Create in a position and orientation
	ControlPoint(const Pnt3f& pos, const Pnt3f& orient);

	// draw the control point - assumes the color is correct
	void draw(bool onlyPost, int s);

	// draw post of control point
	void drawPost(const Pnt3f& a, const Pnt3f& b, GLdouble radius, int slices);

public:
	Pnt3f pos;         // Position of this control point
	Pnt3f orient;		 // Orientation of this control point
};
