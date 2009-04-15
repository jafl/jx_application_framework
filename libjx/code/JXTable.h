/******************************************************************************
 JXTable.h

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTable
#define _H_JXTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>
#include <JTable.h>

class JXTable : public JXScrollableWidget, public JTable
{
public:

	JXTable(const JCoordinate defRowHeight, const JCoordinate defColWidth,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	virtual ~JXTable();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual void		TableRefresh();
	virtual void		TableRefreshRect(const JRect& rect);
	virtual void		TableSetGUIBounds(const JCoordinate w, const JCoordinate h);
	virtual void		TableSetScrollSteps(const JCoordinate hStep,
											const JCoordinate vStep);
	virtual void		TableHeightChanged(const JCoordinate y, const JCoordinate delta);
	virtual void		TableHeightScaled(const JFloat scaleFactor);
	virtual void		TableRowMoved(const JCoordinate origY, const JSize height,
									  const JCoordinate newY);
	virtual void		TableWidthChanged(const JCoordinate x, const JCoordinate delta);
	virtual void		TableWidthScaled(const JFloat scaleFactor);
	virtual void		TableColMoved(const JCoordinate origX, const JSize width,
									  const JCoordinate newX);
	virtual JBoolean	TableScrollToCellRect(const JRect& cellRect,
											  const JBoolean centerInDisplay = kJFalse);
	virtual JCoordinate	TableGetApertureWidth() const;

	void		BeginSelectionDrag(const JPoint& cell, const JXMouseButton button,
								   const JXKeyModifiers& modifiers);
	void		ContinueSelectionDrag(const JPoint& pt, const JXKeyModifiers& modifiers);
	JBoolean	HandleSelectionKeyPress(const int key,
										const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	// not allowed

	JXTable(const JXTable& source);
	const JXTable& operator=(const JXTable& source);
};

#endif
