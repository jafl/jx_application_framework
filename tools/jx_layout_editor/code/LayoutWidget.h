/******************************************************************************
 LayoutWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutWidget
#define _H_LayoutWidget

#include <jx-af/jx/JXWidget.h>

class JStringManager;
class LayoutContainer;
class WidgetParametersDialog;

class LayoutWidget : public JXWidget
{
public:

	LayoutWidget(const bool wantsInput, LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	LayoutWidget(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~LayoutWidget() override;

	virtual void	StreamOut(std::ostream& output) const;

	LayoutContainer*	GetParentContainer() const;
	virtual bool		GetLayoutContainer(const JIndex index, LayoutContainer** layout) const;
	virtual bool		GetLayoutContainerIndex(const LayoutWidget* widget, JIndex* index) const;

	const JString&	GetVarName(bool* isMemberData, bool* isPredeclared) const;
	void			SetVarName(const JString& name, const bool isMember,
								const bool isPredeclared);

	bool	IsSelected() const;
	void	SetSelected(const bool on);

	bool	WantsInput() const;
	bool	GetTabIndex(JIndex* i) const;
	void	SetTabIndex(const JIndex i);

	JPoint	GetDragStartPointGlobal() const;

	void	EditConfiguration(const bool createUndo = true);
	bool	GenerateCode(std::ostream& output, const JString& indent,
						 JPtrArray<JString>* objTypes,
						 JPtrArray<JString>* objNames,
						 JArray<bool>* isMemberVar,
						 JStringManager* stringdb) const;

	virtual void	PrepareToGenerateCode(std::ostream& output, const JString& indent,
										  JStringManager* stringdb) const;
	virtual void	GenerateCodeFinished(std::ostream& output, const JString& indent,
										 JStringManager* stringdb) const;

	void	PrepareToAcceptDrag();

	virtual JString	GetEnclosureName(const LayoutWidget* widget) const;
	JString	ToString() const override;

protected:

	void	SetWantsInput(const bool wantsInput);

	virtual JString	GetClassName() const = 0;
	virtual JString	GetCtor() const;
	virtual void	PrintCtorArgsWithComma(std::ostream& output,
										   const JString& varName,
										   JStringManager* stringdb) const;
	virtual void	PrintConfiguration(std::ostream& output,
									   const JString& indent,
									   const JString& varName,
									   JStringManager* stringdb) const;
	virtual bool	WaitForCodeDependency(const JPtrArray<JString>& objNames) const;

	void	PrintStringForArg(const JString& text, const JString& baseID,
							  JStringManager* stringdb, std::ostream& output) const;
	void	PrintStringIDForArg(const JString& text, const JString& baseID,
								JStringManager* stringdb, std::ostream& output) const;

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

	virtual bool	IsDNDLayoutTarget() const;

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
	bool				itsIsPredeclaredVarFlag;
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

	void	LayoutWidgetX();
};


/******************************************************************************
 GetParentContainer

 ******************************************************************************/

inline LayoutContainer*
LayoutWidget::GetParentContainer()
	const
{
	return itsParent;
}

/******************************************************************************
 Variable name

 ******************************************************************************/

inline const JString&
LayoutWidget::GetVarName
	(
	bool* isMemberData,
	bool* isPredeclared
	)
	const
{
	*isMemberData  = itsIsMemberVarFlag;
	*isPredeclared = itsIsPredeclaredVarFlag;
	return itsVarName;
}

inline void
LayoutWidget::SetVarName
	(
	const JString&	name,
	const bool		isMember,
	const bool		isPredeclared
	)
{
	itsVarName              = name;
	itsIsMemberVarFlag      = isMember;
	itsIsPredeclaredVarFlag = isPredeclared;
}

/******************************************************************************
 IsSelected

 ******************************************************************************/

inline bool
LayoutWidget::IsSelected()
	const
{
	return itsSelectedFlag;
}

/******************************************************************************
 WantsInput

 ******************************************************************************/

inline bool
LayoutWidget::WantsInput()
	const
{
	return itsTabIndex > 0;
}

/******************************************************************************
 GetTabIndex

 ******************************************************************************/

inline bool
LayoutWidget::GetTabIndex
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
LayoutWidget::GetDragStartPointGlobal()
	const
{
	return itsStartPtG;
}

#endif
