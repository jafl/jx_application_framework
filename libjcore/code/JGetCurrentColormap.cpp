/******************************************************************************
 JGetCurrentColormap.cpp

	This class exists only because ParseFunction() needs to be able to
	create JUserInputFunctions without being passed a JFontManager*

	Derived classes must implement the following routine:

		GetCurrColormap
			Return the current colormap.

	BASE CLASS = none

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JGetCurrentColormap.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JGetCurrentColormap::JGetCurrentColormap()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JGetCurrentColormap::~JGetCurrentColormap()
{
}
