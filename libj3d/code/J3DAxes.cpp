/******************************************************************************
 J3DAxes.cpp

	Draws XYZ axes in red,green,blue.  This is useful when debugging
	coordinate transforms.

	BASE CLASS = J3DComposite

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <J3DAxes.h>
#include <J3DLine.h>
#include <J3DCone.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DAxes::J3DAxes
	(
	const JColormap&	colormap,	// until J3DUniverse stores it
	J3DObject*			parent,
	const JFloat		scale
	)
	:
	J3DComposite(parent, 0,0,0)
{
	const JFloat baseR = scale/20.0;
	const JFloat h     = scale/10.0;

	// x axis

	J3DLine* axis = new J3DLine(this, -scale,0,0, +scale,0,0);
	assert( axis != NULL );
	axis->SetColor(colormap.GetRedColor());

	J3DCone* arrow = new J3DCone(this, +scale,0,0, baseR, h);
	assert( arrow != NULL );
	arrow->SetColor(colormap.GetRedColor());
	arrow->SetRotation(kJ3DUnitX, 0.0);

	// y axis

	axis = new J3DLine(this, 0,-scale,0, 0,+scale,0);
	assert( axis != NULL );
	axis->SetColor(colormap.GetGreenColor());

	arrow = new J3DCone(this, 0,+scale,0, baseR, h);
	assert( arrow != NULL );
	arrow->SetColor(colormap.GetGreenColor());
	arrow->SetRotation(kJ3DUnitY, 0.0);

	// z axis

	axis = new J3DLine(this, 0,0,-scale, 0,0,+scale);
	assert( axis != NULL );
	axis->SetColor(colormap.GetLightBlueColor());

	arrow = new J3DCone(this, 0,0,+scale, baseR, h);
	assert( arrow != NULL );
	arrow->SetColor(colormap.GetLightBlueColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DAxes::~J3DAxes()
{
}
