/******************************************************************************
 JXTable.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTable
#define _H_JXTable

#include "JXScrollableWidget.h"
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

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	virtual void		TableRefresh() override;
	virtual void		TableRefreshRect(const JRect& rect) override;
	virtual void		TableSetGUIBounds(const JCoordinate w, const JCoordinate h) override;
	virtual void		TableSetScrollSteps(const JCoordinate hStep,
											const JCoordinate vStep) override;
	virtual void		TableHeightChanged(const JCoordinate y, const JCoordinate delta) override;
	virtual void		TableHeightScaled(const JFloat scaleFactor) override;
	virtual void		TableRowMoved(const JCoordinate origY, const JSize height,
									  const JCoordinate newY) override;
	virtual void		TableWidthChanged(const JCoordinate x, const JCoordinate delta) override;
	virtual void		TableWidthScaled(const JFloat scaleFactor) override;
	virtual void		TableColMoved(const JCoordinate origX, const JSize width,
									  const JCoordinate newX) override;
	virtual JBoolean	TableScrollToCellRect(const JRect& cellRect,
											  const JBoolean centerInDisplay = kJFalse) override;
	virtual JCoordinate	TableGetApertureWidth() const override;

	void		BeginSelectionDrag(const JPoint& cell, const JXMouseButton button,
								   const JXKeyModifiers& modifiers);
	void		ContinueSelectionDrag(const JPoint& pt, const JXKeyModifiers& modifiers);
	JBoolean	HandleSelectionKeyPress(const int key,
										const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// not allowed

	JXTable(const JXTable& source);
	const JXTable& operator=(const JXTable& source);
};

#endif
