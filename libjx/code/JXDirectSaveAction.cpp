/******************************************************************************
 JXDirectSaveAction.cpp

	Base class for object that completes a Direct Save transaction.

	JXDSSSelection will delete the object if it does *not* call Save().
	If Save() is called, the object must dispose of itself.

	BASE CLASS = none

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDirectSaveAction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDirectSaveAction::JXDirectSaveAction()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDirectSaveAction::~JXDirectSaveAction()
{
}
