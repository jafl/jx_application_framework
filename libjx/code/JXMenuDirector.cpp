/******************************************************************************
 JXMenuDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXMenuDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenu.h>
#include <JXMenuTable.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuDirector::JXMenuDirector
	(
	JXDirector*	supervisor,
	JXMenu*		menu
	)
	:
	JXWindowDirector(supervisor)
{
	JXWindow* window = jnew JXWindow(this, 10,10, JString::empty, kJTrue);
	assert( window != nullptr );

	itsMenuTable = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMenuDirector::~JXMenuDirector()
{
}

/******************************************************************************
 BuildWindow

	We have to do this after the constructor is finished because
	CreateMenuTable() is pure virtual.

	If sourceHeight > 0, considers possibility of placing the window
	above (*PtR.y - sourceHeight)

 ******************************************************************************/

void
JXMenuDirector::BuildWindow
	(
	const JPoint&		leftPtR,
	const JPoint&		rightPtR,
	const JCoordinate	sourceHeight
	)
{
	GetDisplay()->ShrinkDisplayBoundsToActiveScreen();

	itsMenuTable = CreateMenuTable();
	itsMenuTable->FitToEnclosure();
	itsMenuTable->SetSingleFocusWidget();

	const JCoordinate bw = itsMenuTable->GetBorderWidth();
	const JRect bounds   = itsMenuTable->GetBoundsGlobal();
	const JCoordinate w  = bounds.width()  + 2*bw;
	const JCoordinate h  = bounds.height() + 2*bw;

	const JRect rootBounds = GetDisplay()->GetBounds();

	JCoordinate windowX, windowWidth = w;
	JPoint usedPtR = rightPtR;
	if (rightPtR.x + w <= rootBounds.right)
		{
		windowX = rightPtR.x;
		}
	else if (leftPtR.x - w >= rootBounds.left)
		{
		windowX = leftPtR.x - w;
		usedPtR = leftPtR;
		}
	else
		{
		windowX = rootBounds.right - w;
		}

	if (windowX + w > rootBounds.right)
		{
		windowX = rootBounds.right - w;
		}
	if (windowX < rootBounds.left)
		{
		windowX = rootBounds.left;
		if (windowX + w > rootBounds.right)
			{
			windowWidth = rootBounds.width();
			}
		}

	JCoordinate windowY, windowHeight = h;
	JBoolean scrollBottom = kJFalse;
	if (usedPtR.y + h <= rootBounds.bottom)
		{
		windowY = usedPtR.y;
		}
	else if (sourceHeight > 0 && rootBounds.top <= usedPtR.y - sourceHeight - h)
		{
		windowY = usedPtR.y - sourceHeight - h;
		}
	else if (sourceHeight > 0 &&
			 rootBounds.bottom - usedPtR.y > usedPtR.y - sourceHeight - rootBounds.top)
		{
		windowY      = usedPtR.y;
		windowHeight = rootBounds.bottom - usedPtR.y;
		}
	else if (sourceHeight > 0)
		{
		windowY      = rootBounds.top;
		windowHeight = usedPtR.y - sourceHeight - rootBounds.top;
		scrollBottom = kJTrue;
		}
	else
		{
		windowY = rootBounds.bottom - h;
		if (windowY < rootBounds.top)
			{
			windowY      = rootBounds.top;
			windowHeight = rootBounds.height();
			}
		}

	JXWindow* window = GetWindow();
	window->Place(windowX, windowY);
	window->SetSize(windowWidth, windowHeight);

	// if menu will scroll, might as well start at the bottom

	if (scrollBottom)
		{
		itsMenuTable->TableScrollToCell(JPoint(1, itsMenuTable->GetRowCount()));
		}

	GetDisplay()->RestoreDisplayBounds();
}

/******************************************************************************
 GrabKeyboard

	We can't grab the keyboard until after the window is visible.

 ******************************************************************************/

void
JXMenuDirector::GrabKeyboard()
{
	itsMenuTable->GrabKeyboard();
}

/******************************************************************************
 GrabPointer

	We can't grab the pointer until after the window is visible.
	This should only be called when the menu is activated by a shortcut.

 ******************************************************************************/

void
JXMenuDirector::GrabPointer()
{
	GetWindow()->GrabPointer(itsMenuTable);
}
