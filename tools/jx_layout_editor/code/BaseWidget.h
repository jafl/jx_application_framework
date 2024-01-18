/******************************************************************************
 BaseWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_BaseWidget
#define _H_BaseWidget

#include <jx-af/jx/JXWidget.h>

class JStringManager;
class LayoutContainer;
class WidgetParametersDialog;

class BaseWidget : public JXWidget
{
public:

	BaseWidget(const bool wantsInput, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	BaseWidget(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~BaseWidget() override;

	virtual void	StreamOut(std::ostream& output) const;

	LayoutContainer*	GetParentContainer() const;
	virtual bool		GetLayoutContainer(LayoutContainer** layout) const;

	const JString&	GetVarName(bool* isMemberData) const;
	void			SetVarName(const JString& name, const bool isMember);

	bool	IsSelected() const;
	void	SetSelected(const bool on);

	bool	WantsInput() const;
	bool	GetTabIndex(JIndex* i) const;
	void	SetTabIndex(const JIndex i);

	JPoint	GetDragStartPointGlobal() const;

	void	EditConfiguration(const bool createUndo = true);
	void	GenerateCode(std::ostream& output, const JString& indent,
						 JPtrArray<JString>* objTypes,
						 JPtrArray<JString>* objNames,
						 JStringManager* stringdb) const;

	void	PrepareToAcceptDrag();

	virtual JString	GetEnclosureName() const;
	JString	ToString() const override;

protected:

	void	SetWantsInput(const bool wantsInput);

	virtual JString	GetClassName() const = 0;
	virtual JString	GetCtor() const;
	virtual JRect	GetFrameForCode() const;
	virtual void	PrintCtorArgsWithComma(std::ostream& output,
										   const JString& varName,
										   JStringManager* stringdb) const;
	virtual void	PrintConfiguration(std::ostream& output,
									   const JString& indent,
									   const JString& varName,
									   JStringManager* stringdb) const;

	virtual void	AddPanels(WidgetParametersDialog* dlog);
	virtual void	SavePanelData();

	void	DrawOver(JXWindowPainter& p, const JRect& rect) override;

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	bool	StealMouse(const int eventType, const JPoint& ptG,
					   const JXMouseButton button,
					   const unsigned int state) override;

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

	LayoutContainer*	itsParent;
	JString				itsVarName;		// can be empty
	bool				itsIsMemberVarFlag;
	bool				itsSelectedFlag;
	JIndex				itsTabIndex;

	// used during dragging

	JPoint	itsStartPtG;
	JPoint	itsPrevPtG;
	JSize	itsLastClickCount;
	bool	itsWaitingForDragFlag;
	bool	itsClearIfNotDNDFlag;

	bool	itsExpectingDragFlag;	// receive drag from child LayoutContainer
	time_t	itsLastExpectingTime;
	JSize	itsExpectingClickCount;

	// used during resize drag

	bool			itsIsResizingFlag;
	JIndex			itsResizeDragType;
	JRect			itsHandles[ kHandleCount ];
	JCursorIndex	itsCursors[ kHandleCount ];

private:

	void	BaseWidgetX();
};


/******************************************************************************
 GetParentContainer

 ******************************************************************************/

inline LayoutContainer*
BaseWidget::GetParentContainer()
	const
{
	return itsParent;
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
	*isMemberData = itsIsMemberVarFlag;
	return itsVarName;
}

inline void
BaseWidget::SetVarName
	(
	const JString&	name,
	const bool		isMember
	)
{
	itsVarName         = name;
	itsIsMemberVarFlag = isMember;
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
 WantsInput

 ******************************************************************************/

inline bool
BaseWidget::WantsInput()
	const
{
	return itsTabIndex > 0;
}

/******************************************************************************
 GetTabIndex

 ******************************************************************************/

inline bool
BaseWidget::GetTabIndex
	(
	JIndex* i
	)
	const
{
	*i = itsTabIndex;
	return itsTabIndex > 0;
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
