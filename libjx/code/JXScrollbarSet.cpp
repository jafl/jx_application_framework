/******************************************************************************
 JXScrollbarSet.cpp

	Maintains the scroll bars for a scrollable widget.  The default is to
	show only the scroll bars that are necessary.  Use AlwaysShowScrollbars()
	to change this behavior.

	BASE CLASS = JXWidgetSet

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXScrollbarSet.h>
#include <JXScrollableWidget.h>
#include <JXScrollbar.h>
#include <JXWindow.h>
#include <jAssert.h>

const JSize	kScrollBarWidth = 15;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXScrollbarSet::JXScrollbarSet
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsScrollEnclosure =
		new JXWidgetSet(this, kHElastic, kVElastic, 0,0, w,h);
	assert( itsScrollEnclosure != NULL );

	itsHScrollbar =
		new JXScrollbar(this, kHElastic, kFixedBottom,
						0,h-kScrollBarWidth, w-kScrollBarWidth,kScrollBarWidth);
	assert( itsHScrollbar != NULL );
	itsHScrollbar->Hide();

	itsVScrollbar =
		new JXScrollbar(this, kFixedRight, kVElastic,
						w-kScrollBarWidth,0, kScrollBarWidth,h-kScrollBarWidth);
	assert( itsVScrollbar != NULL );
	itsVScrollbar->Hide();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXScrollbarSet::~JXScrollbarSet()
{
	itsHScrollbar = itsVScrollbar = NULL;	// don't give out deleted pointers
}

/******************************************************************************
 Show/Hide scrollbars

 ******************************************************************************/

void
JXScrollbarSet::ShowHScrollbar()
{
	ShowScrollbars(kJTrue, itsVScrollbar->WouldBeVisible());
}

void
JXScrollbarSet::HideHScrollbar()
{
	ShowScrollbars(kJFalse, itsVScrollbar->WouldBeVisible());
}

void
JXScrollbarSet::ShowVScrollbar()
{
	ShowScrollbars(itsHScrollbar->WouldBeVisible(), kJTrue);
}

void
JXScrollbarSet::HideVScrollbar()
{
	ShowScrollbars(itsHScrollbar->WouldBeVisible(), kJFalse);
}

/******************************************************************************
 ShowScrollbars

 ******************************************************************************/

void
JXScrollbarSet::ShowScrollbars
	(
	const JBoolean showH,
	const JBoolean showV
	)
{
	if (itsHScrollbar->WouldBeVisible() == showH &&
		itsVScrollbar->WouldBeVisible() == showV)
		{
		return;
		}

	JCoordinate sEnclW = GetBoundsWidth();
	JCoordinate sEnclH = GetBoundsHeight();

	if (showH)
		{
		itsHScrollbar->Show();
		sEnclH -= kScrollBarWidth;
		}
	else
		{
		itsHScrollbar->Hide();
		}

	if (showV)
		{
		itsVScrollbar->Show();
		sEnclW -= kScrollBarWidth;
		}
	else
		{
		itsVScrollbar->Hide();
		}

	itsHScrollbar->SetSize(sEnclW, kScrollBarWidth);
	itsVScrollbar->SetSize(kScrollBarWidth, sEnclH);

	// We do this last because JXScrollableWidget
	// will call us again.

	itsScrollEnclosure->SetSize(sEnclW, sEnclH);
}
