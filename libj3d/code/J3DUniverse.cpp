/******************************************************************************
 J3DUniverse.cpp

	This class defines the concept of a universe, the space within which
	J3DObjects exist.  It is itself a J3DObject because it is the parent
	of all objects in the space.

	One views the universe through a J3DCamera.  There can be multiple
	cameras looking at the same universe, usually at different positions.
	All cameras must be deleted before the universe can be deleted.

	BASE CLASS = J3DObject

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <J3DUniverse.h>
#include <J3DCamera.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DUniverse::J3DUniverse
	(
	const JColormap& colormap
	)
	:
	J3DObject(NULL, 0,0,0),
	itsColormap(colormap)
{
	itsCameraList = new JPtrArray<J3DCamera>(JPtrArrayT::kForgetAll);
	assert( itsCameraList != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DUniverse::~J3DUniverse()
{
	assert( itsCameraList->IsEmpty() );
	delete itsCameraList;
}

/******************************************************************************
 Render (virtual protected)

 ******************************************************************************/

void
J3DUniverse::Render
	(
	J3DPainter& p
	)
{
}

/******************************************************************************
 Refresh (virtual protected)

	Tell all our cameras to redraw when convenient.

 ******************************************************************************/

void
J3DUniverse::Refresh()
{
	const JSize count = itsCameraList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsCameraList->NthElement(i))->Refresh();
		}
}

/******************************************************************************
 Redraw (virtual protected)

	Tell all our cameras to redraw immediately.

 ******************************************************************************/

void
J3DUniverse::Redraw()
{
	const JSize count = itsCameraList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsCameraList->NthElement(i))->Redraw();
		}
}

/******************************************************************************
 GetDefaultColor (virtual protected)

 ******************************************************************************/

JColorIndex
J3DUniverse::GetDefaultColor()
	const
{
	return itsColormap.GetWhiteColor();
}

/******************************************************************************
 Add/Remove Camera

 ******************************************************************************/

void
J3DUniverse::AddCamera
	(
	J3DCamera* camera
	)
{
	if (!itsCameraList->Includes(camera))
		{
		itsCameraList->Append(camera);
		}
}

void
J3DUniverse::RemoveCamera
	(
	J3DCamera* camera
	)
{
	itsCameraList->Remove(camera);
}
