/******************************************************************************
 J3DComposite.cpp

	This class defines the concept of a composite object.  It implements
	Render() to do nothing since the children do the drawing.

	BASE CLASS = J3DObject

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <J3DComposite.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DComposite::J3DComposite
	(
	J3DObject*		parent,
	const JFloat	x,
	const JFloat	y,
	const JFloat	z
	)
	:
	J3DObject(parent, x,y,z)
{
}

J3DComposite::J3DComposite
	(
	J3DObject*		parent,
	const JVector&	position
	)
	:
	J3DObject(parent, position)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DComposite::~J3DComposite()
{
}

/******************************************************************************
 Render (virtual protected)

 ******************************************************************************/

void
J3DComposite::Render
	(
	J3DPainter& p
	)
{
}
