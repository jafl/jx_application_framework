/******************************************************************************
 J3DTetrahedron.cpp

	All 6 edges have length 1.
	The origin is at the centroid of the base triangle.
	The front vertex is on the +x axis.
	The top vertex is on the +z axis.

	BASE CLASS = J3DObject

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <J3DTetrahedron.h>
#include <GL/gl.h>
#include <jMath.h>
#include <jAssert.h>

const JFloat kCos30  = sqrt(3.0)/2.0;
const JFloat kXFront = 0.5 / kCos30;
const JFloat kXBack  = kCos30 - kXFront;
const JFloat kYBack  = 0.5;
const JFloat kZTop   = sqrt(1.0 - kXFront * kXFront);

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DTetrahedron::J3DTetrahedron
	(
	J3DObject*		parent,
	const JFloat	x,
	const JFloat	y,
	const JFloat	z
	)
	:
	J3DObject(parent, x,y,z)
{
	J3DTetrahedronX();
}

J3DTetrahedron::J3DTetrahedron
	(
	J3DObject*		parent,
	const JVector&	position
	)
	:
	J3DObject(parent, position)
{
	J3DTetrahedronX();
}

// private

void
J3DTetrahedron::J3DTetrahedronX()
{
	itsVertexColor[0] = itsVertexColor[1] =
	itsVertexColor[2] = itsVertexColor[3] = GetDefaultColor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DTetrahedron::~J3DTetrahedron()
{
}

/******************************************************************************
 Vertex color

 ******************************************************************************/

JColorIndex
J3DTetrahedron::GetVertexColor
	(
	const VertexIndex vertex
	)
	const
{
	assert( vertex < 4 );
	return itsVertexColor [ vertex ];
}

void
J3DTetrahedron::SetVertexColor
	(
	const VertexIndex	vertex,
	const JColorIndex	color
	)
{
	assert( vertex < 4 );
	itsVertexColor [ vertex ] = color;
	Refresh();
}

/******************************************************************************
 Render (virtual protected)

	front -> top -> back right -> back left -> front -> top

 ******************************************************************************/

void
J3DTetrahedron::Render
	(
	J3DPainter& p
	)
{
	glBegin(GL_TRIANGLE_STRIP);

	p.SetVertexColor(itsVertexColor [ kFrontVertex ]);
	glVertex3f(kXFront, 0.0, 0.0);

	p.SetVertexColor(itsVertexColor [ kTopVertex ]);
	glVertex3f(0.0, 0.0, kZTop);

	p.SetVertexColor(itsVertexColor [ kBackRightVertex ]);
	glVertex3f(-kXBack, kYBack, 0.0);

	p.SetVertexColor(itsVertexColor [ kBackLeftVertex ]);
	glVertex3f(-kXBack, -kYBack, 0.0);

	p.SetVertexColor(itsVertexColor [ kFrontVertex ]);
	glVertex3f(kXFront, 0.0, 0.0);

	p.SetVertexColor(itsVertexColor [ kTopVertex ]);
	glVertex3f(0.0, 0.0, kZTop);

	glEnd();
}
