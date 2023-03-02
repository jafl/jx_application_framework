/******************************************************************************
 JXColHeaderWidget.h

	Interface for the JXColHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXColHeaderWidget
#define _H_JXColHeaderWidget

#include "JXEditTable.h"

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

	~JXColHeaderWidget() override;

	void	TurnOnColResizing(const JCoordinate minColWidth = 20);
	void	TurnOffColResizing();

	bool	GetColTitle(const JIndex index, JString* title) const;
	void	SetColTitle(const JIndex index, const JString& title);
	void	ClearColTitle(const JIndex index);

	void	SetColumnTitles(const JUtf8Byte* className, const JSize count);

protected:

	enum DragType
	{
		kInvalidDrag,
		kDragOneCol,
		kDragAllCols
	};

protected:

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
	const JXScrollbar*	itsHScrollbar;	// we don't own this
	JPtrArray<JString>*	itsTitles;		// can be nullptr; elements can be nullptr

	bool		itsAllowColResizingFlag;
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
};


/******************************************************************************
 TurnOffColResizing

 ******************************************************************************/

inline void
JXColHeaderWidget::TurnOffColResizing()
{
	itsAllowColResizingFlag = false;
}

/******************************************************************************
 GetDragType (protected)

 ******************************************************************************/

inline JXColHeaderWidget::DragType
JXColHeaderWidget::GetDragType()
	const
{
	return itsDragType;
}

#endif
