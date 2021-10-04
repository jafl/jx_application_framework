/******************************************************************************
 JXTable.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTable
#define _H_JXTable

#include "jx-af/jx/JXScrollableWidget.h"
#include <jx-af/jcore/JTable.h>

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

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	void		TableRefresh() override;
	void		TableRefreshRect(const JRect& rect) override;
	void		TableSetGUIBounds(const JCoordinate w, const JCoordinate h) override;
	void		TableSetScrollSteps(const JCoordinate hStep,
											const JCoordinate vStep) override;
	void		TableHeightChanged(const JCoordinate y, const JCoordinate delta) override;
	void		TableHeightScaled(const JFloat scaleFactor) override;
	void		TableRowMoved(const JCoordinate origY, const JSize height,
									  const JCoordinate newY) override;
	void		TableWidthChanged(const JCoordinate x, const JCoordinate delta) override;
	void		TableWidthScaled(const JFloat scaleFactor) override;
	void		TableColMoved(const JCoordinate origX, const JSize width,
									  const JCoordinate newX) override;
	bool	TableScrollToCellRect(const JRect& cellRect,
											  const bool centerInDisplay = false) override;
	JCoordinate	TableGetApertureWidth() const override;

	void		BeginSelectionDrag(const JPoint& cell, const JXMouseButton button,
								   const JXKeyModifiers& modifiers);
	void		ContinueSelectionDrag(const JPoint& pt, const JXKeyModifiers& modifiers);
	bool	HandleSelectionKeyPress(const JUtf8Character& c,
										const JXKeyModifiers& modifiers);

	void	Receive(JBroadcaster* sender, const Message& message) override;
};

#endif
