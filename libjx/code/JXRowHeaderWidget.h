/******************************************************************************
 JXRowHeaderWidget.h

	Interface for the JXRowHeaderWidget class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXRowHeaderWidget
#define _H_JXRowHeaderWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>

class JXScrollbar;

class JXRowHeaderWidget : public JXEditTable
{
public:

	JXRowHeaderWidget(JXTable* table,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXRowHeaderWidget();

	void	TurnOnRowResizing(const JCoordinate minRowHeight);
	void	TurnOffRowResizing();

	JBoolean	GetRowTitle(const JIndex index, JString* title) const;
	void		SetRowTitle(const JIndex index, const JCharacter* title);
	void		ClearRowTitle(const JIndex index);

	JCoordinate	GetMinMarginWidth() const;
	void		SetMinMarginWidth(const JCoordinate w);

protected:

	virtual void	TablePrepareToDrawCol(const JIndex colIndex,
										  const JIndex firstRow, const JIndex lastRow);
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
		kDragOneRow,
		kDragAllRows
	};

private:

	JXTable*			itsTable;		// we don't own this
	const JXScrollbar*	itsVScrollbar;	// we don't own this
	JPtrArray<JString>*	itsTitles;		// can be NULL; elements can be NULL

	JBoolean	itsAllowRowResizingFlag;
	JCoordinate	itsMinRowHeight;
	JCoordinate	itsHMarginWidth;		// minimum margin on either side of number
	JCoordinate	itsMaxBcastWidth;		// largest min width that has broadcast

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

	JXRowHeaderWidget(const JXRowHeaderWidget& source);
	const JXRowHeaderWidget& operator=(const JXRowHeaderWidget& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNeedsToBeWidened;

	class NeedsToBeWidened : public JBroadcaster::Message
		{
		public:

			NeedsToBeWidened(const JCoordinate deltaWidth)
				:
				JBroadcaster::Message(kNeedsToBeWidened),
				itsDeltaWidth(deltaWidth)
				{ };

			JCoordinate
			GetDeltaWidth() const
			{
				return itsDeltaWidth;
			};

		private:

			JCoordinate itsDeltaWidth;
		};

};


/******************************************************************************
 TurnOffRowResizing

 ******************************************************************************/

inline void
JXRowHeaderWidget::TurnOffRowResizing()
{
	itsAllowRowResizingFlag = kJFalse;
}

/******************************************************************************
 GetMinMarginWidth

 ******************************************************************************/

inline JCoordinate
JXRowHeaderWidget::GetMinMarginWidth()
	const
{
	return itsHMarginWidth;
}

/******************************************************************************
 SetMinMarginWidth

	Remember to account for the width of the border drawn inside the cell.

 ******************************************************************************/

inline void
JXRowHeaderWidget::SetMinMarginWidth
	(
	const JCoordinate w
	)
{
	itsHMarginWidth  = w;
	itsMaxBcastWidth = 0;
	Refresh();		// force a check
}

#endif
