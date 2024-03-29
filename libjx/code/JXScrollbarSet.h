/******************************************************************************
 JXScrollbarSet.h

	Interface for the JXScrollbarSet class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrollbarSet
#define _H_JXScrollbarSet

#include "JXWidgetSet.h"

class JXScrollbar;

class JXScrollbarSet : public JXWidgetSet
{
public:

	JXScrollbarSet(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~JXScrollbarSet() override;

	JXWidgetSet*	GetScrollEnclosure();
	JXScrollbar*	GetHScrollbar();
	JXScrollbar*	GetVScrollbar();

	void	ShowHScrollbar();
	void	HideHScrollbar();

	void	ShowVScrollbar();
	void	HideVScrollbar();

	void	ShowScrollbars(const bool showH, const bool showV);

private:

	JXWidgetSet*	itsScrollEnclosure;
	JXScrollbar*	itsHScrollbar;
	JXScrollbar*	itsVScrollbar;
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
