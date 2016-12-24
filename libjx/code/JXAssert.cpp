/******************************************************************************
 JXAssert.cpp

	We ensure that all keyboard and mouse grabs are released.

	BASE CLASS = JAssertBase

	Copyright (C) 1997 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JXAssert.h>
#include <JXApplication.h>
#include <JXDisplay.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAssert::JXAssert()
	:
	JAssertBase()
{
	itsIsOperatingFlag = kJTrue;

	itsDisplayList = jnew JPtrArray<JXDisplay>(JPtrArrayT::kForgetAll);
	assert( itsDisplayList != NULL );

	itsIsOperatingFlag = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAssert::~JXAssert()
{
	itsIsOperatingFlag = kJTrue;

	JPtrArray<JXDisplay>* list = itsDisplayList;
	itsDisplayList = NULL;
	jdelete list;

	itsIsOperatingFlag = kJFalse;
}

/******************************************************************************
 Assert (virtual)

 ******************************************************************************/

int
JXAssert::Assert
	(
	const JCharacter*	expr,
	const JCharacter*	file,
	const int			line
	)
{
	const JBoolean wasOperating = itsIsOperatingFlag;
	itsIsOperatingFlag = kJTrue;

	if (wasOperating || itsDisplayList == NULL)		// prevents infinite recursion
		{
		fprintf(stderr, "\nError inside fatal error handler!\n\n");
		}
	else
		{
		UnlockDisplays();
		}

	const int result = JAssertBase::DefaultAssert(expr, file, line);

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
	JXApplication::Abort(JXDocumentManager::kAssertFired, kJTrue);
}

/******************************************************************************
 UnlockDisplays

 ******************************************************************************/

void
JXAssert::UnlockDisplays()
{
	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDisplay* display = itsDisplayList->NthElement(i);
		Display* xDisplay  = display->GetXDisplay();
		XUngrabServer(xDisplay);
		XUngrabPointer(xDisplay, CurrentTime);
		XUngrabKeyboard(xDisplay, CurrentTime);
		display->Flush();
		}
}
