/******************************************************************************
 JXAssert.cpp

	We ensure that all keyboard and mouse grabs are released.

	BASE CLASS = JAssertBase

	Copyright (C) 1997 by John Lindal.

 *****************************************************************************/

#include "jx-af/jx/JXAssert.h"
#include "jx-af/jx/JXApplication.h"
#include "jx-af/jx/JXDisplay.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAssert::JXAssert()
	:
	JAssertBase()
{
	itsIsOperatingFlag = true;

	itsDisplayList = jnew JPtrArray<JXDisplay>(JPtrArrayT::kForgetAll);
	assert( itsDisplayList != nullptr );

	itsIsOperatingFlag = false;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAssert::~JXAssert()
{
	itsIsOperatingFlag = true;

	JPtrArray<JXDisplay>* list = itsDisplayList;
	itsDisplayList = nullptr;
	jdelete list;

	itsIsOperatingFlag = false;
}

/******************************************************************************
 Assert (virtual)

 ******************************************************************************/

int
JXAssert::Assert
	(
	const JUtf8Byte*	expr,
	const JUtf8Byte*	file,
	const int			line,
	const JUtf8Byte*	message
	)
{
	const bool wasOperating = itsIsOperatingFlag;
	itsIsOperatingFlag = true;

	if (wasOperating || itsDisplayList == nullptr)		// prevents infinite recursion
	{
		fprintf(stderr, "\nError inside fatal error handler!\n\n");
	}
	else
	{
		UnlockDisplays();
	}

	const int result = JAssertBase::DefaultAssert(expr, file, line, message);

	itsIsOperatingFlag = wasOperating;
	return result;
}

/******************************************************************************
 Abort (virtual)

	We call JXApplication::Abort().

 ******************************************************************************/

void
JXAssert::Abort()
{
	JXApplication::Abort(JXDocumentManager::kAssertFired, true);
}

/******************************************************************************
 UnlockDisplays

 ******************************************************************************/

void
JXAssert::UnlockDisplays()
{
	for (auto* display : *itsDisplayList)
	{
		Display* xDisplay = display->GetXDisplay();
		XUngrabServer(xDisplay);
		XUngrabPointer(xDisplay, CurrentTime);
		XUngrabKeyboard(xDisplay, CurrentTime);
		display->Flush();
	}
}
