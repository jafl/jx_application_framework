/******************************************************************************
 JXHintManager.cpp

	BASE CLASS = none

	Copyright © 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXHintManager.h>
#include <JXHintDirector.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

const Time kHintWaitDelay = 500;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHintManager::JXHintManager
	(
	JXContainer*		widget,
	const JCharacter*	text
	)
	:
	itsWidget(widget),
	itsText(text),
	itsActiveFlag(kJTrue),
	itsStartTime(0),
	itsDirOwner(NULL),
	itsDirector(NULL)
{
	assert( itsWidget != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHintManager::~JXHintManager()
{
	DestroyWindow();
}

/******************************************************************************
 HandleMouseEnter

 ******************************************************************************/

void
JXHintManager::HandleMouseEnter()
{
	itsStartTime = (JXGetApplication())->GetCurrentTime();
	itsPrevPt.Set(0,0);
	DestroyWindow();
}

/******************************************************************************
 HandleMouseHere

	The second version allows a hint for part of the widget.

 ******************************************************************************/

void
JXHintManager::HandleMouseHere
	(
	const JPoint& pt
	)
{
	HandleMouseHere(pt, itsWidget->GetFrameLocal());
}

void
JXHintManager::HandleMouseHere
	(
	const JPoint&	pt,
	const JRect&	rect
	)
{
	if (itsActiveFlag)
		{
		const Time t = (JXGetApplication())->GetCurrentTime();
		if (itsDirector == NULL && pt == itsPrevPt &&
			t - itsStartTime > kHintWaitDelay)
			{
			CreateHintWindow(rect);
			}
		else if (itsDirector == NULL && pt != itsPrevPt)
			{
			itsStartTime = t;
			itsPrevPt    = pt;
			}
		}
}

/******************************************************************************
 HandleMouseLeave

 ******************************************************************************/

void
JXHintManager::HandleMouseLeave()
{
	itsStartTime = 0;
	itsPrevPt.Set(0,0);
	DestroyWindow();
}

/******************************************************************************
 CreateHintWindow

	The second version allows a hint for part of the widget.

	We use a dummy director so we don't have to worry about somebody
	else closing itsDirector.

	We can't call it "CreateWindow" because Windows #define's this.

 ******************************************************************************/

void
JXHintManager::CreateHintWindow()
{
	if (itsDirOwner == NULL)
		{
		itsDirOwner = new JXDirector(NULL);
		assert( itsDirOwner != NULL );
		}

	if (itsDirector == NULL)
		{
		itsDirector = new JXHintDirector(itsDirOwner, itsWidget, itsText);
		assert( itsDirector != NULL );
		itsDirector->Activate();
		}

	(itsWidget->GetWindow())->SetCurrentHintManager(this);
}

void
JXHintManager::CreateHintWindow
	(
	const JRect& rect
	)
{
	if (itsDirOwner == NULL)
		{
		itsDirOwner = new JXDirector(NULL);
		assert( itsDirOwner != NULL );
		}

	if (itsDirector == NULL)
		{
		itsDirector = new JXHintDirector(itsDirOwner, itsWidget, rect, itsText);
		assert( itsDirector != NULL );
		itsDirector->Activate();
		}

	(itsWidget->GetWindow())->SetCurrentHintManager(this);
}

/******************************************************************************
 DestroyWindow

 ******************************************************************************/

void
JXHintManager::DestroyWindow()
{
	if (itsDirOwner != NULL)
		{
		itsDirOwner->Close();
		itsDirOwner = NULL;
		itsDirector = NULL;

		(itsWidget->GetWindow())->SetCurrentHintManager(NULL);
		}
}
