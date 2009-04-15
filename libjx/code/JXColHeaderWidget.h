/******************************************************************************
 JXColHeaderWidget.h

	Interface for the JXColHeaderWidget class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXColHeaderWidget
#define _H_JXColHeaderWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>

class JString;
class JXScrollbar;

class JXColHeaderWidget : public JXEditTable
{
public:

	JXColHeaderWidget(JXTable* table, JXScrollbarSet* scrollbarSet,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXColHeaderWidget();

	void	TurnOnColResizing(const JCoordinate minColWidth);
	void	TurnOffColResizing();

	JBoolean	GetColTitle(const JIndex index, JString* title) const;
	void		SetColTitle(const JIndex index, const JCharacter* title);
	void		ClearColTitle(const JIndex index);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);
	JBoolean		InDragRegion(const JPoint& pt, JPoint* cell) const;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum DragType
	{
		kInvalidDrag,
		kDragOneCol,
		kDragAllCols
	};

private:

	JXTable*			itsTable;		// we don't own this
	const JXScrollbar*	itsHScrollbar;	// we don't own this
	JPtrArray<JString>*	itsTitles;		// can be NULL; elements can be NULL

	JBoolean	itsAllowColResizingFlag;
	JCoordinate	itsMinColWidth;

	JCursorIndex	itsDragLineCursor;
	JCursorIndex	itsDragAllLineCursor;

	// used during drag

	DragType	itsDragType;
	JPoint		itsDragCell;
	JRect		itsDragCellRect;
	JPoint		itsPrevPt;

private:

	void	AdjustToTable();

	// not allowed

	JXColHeaderWidget(const JXColHeaderWidget& source);
	const JXColHeaderWidget& operator=(const JXColHeaderWidget& source);
};

/******************************************************************************
 TurnOffColResizing

 ******************************************************************************/

inline void
JXColHeaderWidget::TurnOffColResizing()
{
	itsAllowColResizingFlag = kJFalse;
}

#endif
