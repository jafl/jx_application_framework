/******************************************************************************
 JExprRenderer.cpp

	Abstract base class to render a JFunction.  Refer to JRect.cc
	for the coordinate system.

	All derived classes must implement Render.  It is not defined here
	because it requires passing in system specific information about the
	display that is going to be drawn to.

	Regarding font sizes:
		It is best to store a lookup table of font sizes so one will only
		use the sizes that the system supports, and so one will not go below
		some smallest font size.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JExprRenderer.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JExprRenderer::JExprRenderer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JExprRenderer::~JExprRenderer()
{
}
