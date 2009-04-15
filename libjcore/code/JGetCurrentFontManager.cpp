/******************************************************************************
 JGetCurrentFontManager.cpp

	This class exists only because ParseFunction() needs to be able to
	create JUserInputFunctions without being passed a JFontManager*

	Derived classes must implement the following routine:

		GetCurrFontManager
			Return the current font manager.

	BASE CLASS = none

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JGetCurrentFontManager.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JGetCurrentFontManager::JGetCurrentFontManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JGetCurrentFontManager::~JGetCurrentFontManager()
{
}
