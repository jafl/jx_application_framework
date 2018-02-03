/******************************************************************************
 J3DPainter.cpp

	Pure virtual base class for use by J3DObjects.  Derived classes
	must implement the following functions:

		HWSetVertexColor
			Call a hardware dependent function to set the vertex color.

	BASE CLASS = NONE

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <J3DPainter.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DPainter::J3DPainter
	(
	const JColormap& colormap
	)
	:
	itsColormap(colormap)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DPainter::~J3DPainter()
{
}
