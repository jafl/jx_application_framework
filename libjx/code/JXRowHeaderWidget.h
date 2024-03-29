/******************************************************************************
 JXRowHeaderWidget.h

	Interface for the JXRowHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRowHeaderWidget
#define _H_JXRowHeaderWidget

#include "JXEditTable.h"

class JXScrollbar;

class JXRowHeaderWidget : public JXEditTable
{
public:

	JXRowHeaderWidget(JXTable* table,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~JXRowHeaderWidget() override;

	void	TurnOnRowResizing(const JCoordinate minRowHeight = 20);
	void	TurnOffRowResizing();

	bool	GetRowTitle(const JIndex index, JString* title) const;
	void	SetRowTitle(const JIndex index, const JString& title);
	void	ClearRowTitle(const JIndex index);

	JCoordinate	GetMinMarginWidth() const;
	void		SetMinMarginWidth(const JCoordinate w);

protected:

	enum DragType
	{
		kInvalidDrag,
		kDragOneRow,
		kDragAllRows
	};

protected:

	void	TablePrepareToDrawCol(const JIndex colIndex,
								  const JIndex firstRow, const JIndex lastRow) override;
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	void		AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	bool		InDragRegion(const JPoint& pt, JPoint* cell) const;
	DragType	GetDragType() const;

	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	void			PrepareDeleteXInputField() override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTable*			itsTable;		// we don't own this
	const JXScrollbar*	itsVScrollbar;	// we don't own this
	JPtrArray<JString>*	itsTitles;		// can be nullptr; elements can be nullptr

	bool		itsAllowRowResizingFlag;
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

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNeedsToBeWidened;

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
	itsAllowRowResizingFlag = false;
}

/******************************************************************************
 GetDragType (protected)

 ******************************************************************************/

inline JXRowHeaderWidget::DragType
JXRowHeaderWidget::GetDragType()
	const
{
	return itsDragType;
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
