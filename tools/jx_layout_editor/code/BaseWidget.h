/******************************************************************************
 BaseWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_BaseWidget
#define _H_BaseWidget

#include <jx-af/jx/JXWidget.h>

class LayoutDirector;

class BaseWidget : public JXWidget
{
public:

	BaseWidget(LayoutDirector* dir, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	BaseWidget(LayoutDirector* dir, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~BaseWidget() override;

	virtual void	StreamOut(std::ostream& output) const;

	LayoutDirector*	GetLayoutDirector();

	const JString&	GetVarName(bool* isMemberData) const;
	void			SetVarName(const JString& name, const bool member);

	bool	IsSelected() const;
	void	SetSelected(const bool on);

protected:

	void	DrawSelection(JXWindowPainter& p, const JRect& rect);

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

private:

	LayoutDirector*	itsLayoutDir;
	JString			itsVarName;		// can be empty
	bool			itsMemberVarFlag;
	bool			itsSelectedFlag;
};


/******************************************************************************
 GetLayoutDirector

 ******************************************************************************/

inline LayoutDirector*
BaseWidget::GetLayoutDirector()
{
	return itsLayoutDir;
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
 Selection

 ******************************************************************************/

inline bool
BaseWidget::IsSelected()
	const
{
	return itsSelectedFlag;
}

inline void
BaseWidget::SetSelected
	(
	const bool on
	)
{
	if (on != itsSelectedFlag)
	{
		itsSelectedFlag = on;
		Refresh();
	}
}

#endif
