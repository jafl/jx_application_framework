/******************************************************************************
 JXScrollbarSet.h

	Interface for the JXScrollbarSet class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXScrollbarSet
#define _H_JXScrollbarSet

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JXScrollbar;

class JXScrollbarSet : public JXWidgetSet
{
public:

	JXScrollbarSet(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXScrollbarSet();

	JXWidgetSet*	GetScrollEnclosure();
	JXScrollbar*	GetHScrollbar();
	JXScrollbar*	GetVScrollbar();

	void	ShowHScrollbar();
	void	HideHScrollbar();

	void	ShowVScrollbar();
	void	HideVScrollbar();

	void	ShowScrollbars(const JBoolean showH, const JBoolean showV);

private:

	JXWidgetSet*	itsScrollEnclosure;
	JXScrollbar*	itsHScrollbar;
	JXScrollbar*	itsVScrollbar;

private:

	// not allowed

	JXScrollbarSet(const JXScrollbarSet& source);
	const JXScrollbarSet& operator=(const JXScrollbarSet& source);
};

/******************************************************************************
 GetScrollEnclosure

 ******************************************************************************/

inline JXWidgetSet*
JXScrollbarSet::GetScrollEnclosure()
{
	return itsScrollEnclosure;
}

/******************************************************************************
 Get scrollbars

 ******************************************************************************/

inline JXScrollbar*
JXScrollbarSet::GetHScrollbar()
{
	return itsHScrollbar;
}

inline JXScrollbar*
JXScrollbarSet::GetVScrollbar()
{
	return itsVScrollbar;
}

#endif
