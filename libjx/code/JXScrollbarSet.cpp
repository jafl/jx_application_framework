/******************************************************************************
 JXScrollbarSet.cpp

	Maintains the scroll bars for a scrollable widget.  The default is to
	show only the scroll bars that are necessary.  Use AlwaysShowScrollbars()
	to change this behavior.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXScrollbarSet.h"
#include "JXScrollableWidget.h"
#include "JXScrollbar.h"
#include "JXWindow.h"
#include <jx-af/jcore/jAssert.h>

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
		jnew JXWidgetSet(this, kHElastic, kVElastic, 0,0, w,h);
	itsScrollEnclosure->ClearNeedsInternalFTC();

	itsHScrollbar =
		jnew JXScrollbar(this, kHElastic, kFixedBottom,
						0,h-kScrollBarWidth, w-kScrollBarWidth,kScrollBarWidth);
	itsHScrollbar->Hide();

	itsVScrollbar =
		jnew JXScrollbar(this, kFixedRight, kVElastic,
						w-kScrollBarWidth,0, kScrollBarWidth,h-kScrollBarWidth);
	itsVScrollbar->Hide();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXScrollbarSet::~JXScrollbarSet()
{
	itsHScrollbar = itsVScrollbar = nullptr;	// don't give out deleted pointers
}

/******************************************************************************
 Show/Hide scrollbars

 ******************************************************************************/

void
JXScrollbarSet::ShowHScrollbar()
{
	ShowScrollbars(true, itsVScrollbar->WouldBeVisible());
}

void
JXScrollbarSet::HideHScrollbar()
{
	ShowScrollbars(false, itsVScrollbar->WouldBeVisible());
}

void
JXScrollbarSet::ShowVScrollbar()
{
	ShowScrollbars(itsHScrollbar->WouldBeVisible(), true);
}

void
JXScrollbarSet::HideVScrollbar()
{
	ShowScrollbars(itsHScrollbar->WouldBeVisible(), false);
}

/******************************************************************************
 ShowScrollbars

 ******************************************************************************/

void
JXScrollbarSet::ShowScrollbars
	(
	const bool showH,
	const bool showV
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
