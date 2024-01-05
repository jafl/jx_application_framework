/******************************************************************************
 BaseWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_BaseWidget
#define _H_BaseWidget

#include <jx-af/jx/JXWidget.h>

class LayoutContainer;

class BaseWidget : public JXWidget
{
public:

	BaseWidget(LayoutContainer* layout, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	BaseWidget(LayoutContainer* layout, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~BaseWidget() override;

	virtual void	StreamOut(std::ostream& output) const;

	LayoutContainer*	GetLayoutContainer() const;

	const JString&	GetVarName(bool* isMemberData) const;
	void			SetVarName(const JString& name, const bool member);

	bool	IsSelected() const;
	void	SetSelected(const bool on);

	JPoint	GetDragStartPointGlobal() const;

protected:

	void	DrawSelection(JXWindowPainter& p, const JRect& rect);
	void	DrawOver(JXWindowPainter& p, const JRect& rect) override;

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;
	void	DNDFinish(const bool isDrop, const JXContainer* target) override;

private:

	enum
	{
		kTopLeftHandle,
		kTopHandle,
		kTopRightHandle,
		kRightHandle,
		kBottomRightHandle,
		kBottomHandle,
		kBottomLeftHandle,
		kLeftHandle,

		kHandleCount
	};

private:

	LayoutContainer*	itsLayout;
	JString				itsVarName;		// can be empty
	bool				itsMemberVarFlag;
	bool				itsSelectedFlag;

	// used during dragging

	JPoint	itsStartPtG;
	JPoint	itsPrevPtG;
	JSize	itsLastClickCount;
	bool	itsWaitingForDragFlag;
	bool	itsClearIfNotDNDFlag;

	// used during resize drag

	bool			itsIsResizingFlag;
	JIndex			itsResizeDragType;
	JRect			itsHandles[ kHandleCount ];
	JCursorIndex	itsCursors[ kHandleCount ];

private:

	void	BaseWidgetX();
};


/******************************************************************************
 GetLayoutContainer

 ******************************************************************************/

inline LayoutContainer*
BaseWidget::GetLayoutContainer()
	const
{
	return itsLayout;
}

/******************************************************************************
 Variable name

 ******************************************************************************/

inline const JString&
BaseWidget::GetVarName
	(
	bool* isMemberData
	)
	const
{
	*isMemberData = itsMemberVarFlag;
	return itsVarName;
}

inline void
BaseWidget::SetVarName
	(
	const JString&	name,
	const bool		member
	)
{
	itsVarName       = name;
	itsMemberVarFlag = member;
}

/******************************************************************************
 IsSelected

 ******************************************************************************/

inline bool
BaseWidget::IsSelected()
	const
{
	return itsSelectedFlag;
}

/******************************************************************************
 GetDragStartPointGlobal

 ******************************************************************************/

inline JPoint
BaseWidget::GetDragStartPointGlobal()
	const
{
	return itsStartPtG;
}

#endif
