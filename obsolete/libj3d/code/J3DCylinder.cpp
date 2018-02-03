/******************************************************************************
 J3DCylinder.cpp

	Encapsulates gluCylinder().  The base of the cylinder is at the origin
	and the axis of the cylinder is the +z axis.

	BASE CLASS = J3DObject

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <J3DCylinder.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DCylinder::J3DCylinder
	(
	J3DObject*		parent,
	const JFloat	x,
	const JFloat	y,
	const JFloat	z,
	const JFloat	baseR,
	const JFloat	topR,
	const JFloat	height
	)
	:
	J3DObject(parent, x,y,z)
{
	J3DCylinderX(baseR, topR, height);
}

J3DCylinder::J3DCylinder
	(
	J3DObject*		parent,
	const JVector&	position,
	const JFloat	baseR,
	const JFloat	topR,
	const JFloat	height
	)
	:
	J3DObject(parent, position)
{
	J3DCylinderX(baseR, topR, height);
}

// private

void
J3DCylinder::J3DCylinderX
	(
	const JFloat baseR,
	const JFloat topR,
	const JFloat height
	)
{
	itsBaseRadius = baseR;
	itsTopRadius  = topR;
	itsHeight     = height;

	itsColor = GetDefaultColor();

	itsQuadric = gluNewQuadric();
	assert( itsQuadric != NULL );

	gluQuadricDrawStyle(itsQuadric, (GLenum) GLU_FILL);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DCylinder::~J3DCylinder()
{
	gluDeleteQuadric(itsQuadric);
}

/******************************************************************************
 Render (virtual protected)

 ******************************************************************************/

void
J3DCylinder::Render
	(
	J3DPainter& p
	)
{
	p.SetVertexColor(itsColor);

	gluCylinder(itsQuadric, itsBaseRadius, itsTopRadius, itsHeight,
				32,		// 32 faces
				1);		// ???
}
