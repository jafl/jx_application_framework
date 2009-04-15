/******************************************************************************
 J3DCone.cpp

	A cone is a cylinder with topRadius = 0.

	BASE CLASS = J3DCylinder

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <J3DCone.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DCone::J3DCone
	(
	J3DObject*		parent,
	const JFloat	x,
	const JFloat	y,
	const JFloat	z,
	const JFloat	baseR,
	const JFloat	height
	)
	:
	J3DCylinder(parent, x,y,z, baseR, 0.0, height)
{
}

J3DCone::J3DCone
	(
	J3DObject*		parent,
	const JVector&	position,
	const JFloat	baseR,
	const JFloat	height
	)
	:
	J3DCylinder(parent, position, baseR, 0.0, height)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DCone::~J3DCone()
{
}
