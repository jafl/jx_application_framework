/******************************************************************************
 JXHintManager.cpp

	BASE CLASS = none

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXHintManager.h"
#include "JXHintDirector.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <JString.h>
#include <jAssert.h>

const Time kHintWaitDelay = 500;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHintManager::JXHintManager
	(
	JXContainer*	widget,
	const JString&	text
	)
	:
	itsWidget(widget),
	itsText(text),
	itsActiveFlag(kJTrue),
	itsStartTime(0),
	itsDirOwner(nullptr),
	itsDirector(nullptr)
{
	assert( itsWidget != nullptr );
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
	itsStartTime = JXGetApplication()->GetCurrentTime();
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
		const Time t = JXGetApplication()->GetCurrentTime();
		if (itsDirector == nullptr && pt == itsPrevPt &&
			t - itsStartTime > kHintWaitDelay)
			{
			CreateHintWindow(rect);
			}
		else if (itsDirector == nullptr && pt != itsPrevPt)
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
	if (itsDirOwner == nullptr)
		{
		itsDirOwner = jnew JXDirector(nullptr);
		assert( itsDirOwner != nullptr );
		}

	if (itsDirector == nullptr)
		{
		itsDirector = jnew JXHintDirector(itsDirOwner, itsWidget, itsText);
		assert( itsDirector != nullptr );
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
	if (itsDirOwner == nullptr)
		{
		itsDirOwner = jnew JXDirector(nullptr);
		assert( itsDirOwner != nullptr );
		}

	if (itsDirector == nullptr)
		{
		itsDirector = jnew JXHintDirector(itsDirOwner, itsWidget, rect, itsText);
		assert( itsDirector != nullptr );
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
	if (itsDirOwner != nullptr)
		{
		itsDirOwner->Close();
		itsDirOwner = nullptr;
		itsDirector = nullptr;

		(itsWidget->GetWindow())->SetCurrentHintManager(nullptr);
		}
}
