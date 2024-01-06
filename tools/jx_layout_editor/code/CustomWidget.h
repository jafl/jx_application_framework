/******************************************************************************
 CustomWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CustomWidget
#define _H_CustomWidget

#include "BaseWidget.h"

class CustomWidgetPanel;

class CustomWidget : public BaseWidget
{
public:

	CustomWidget(LayoutContainer* layout, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CustomWidget(LayoutContainer* layout,
				const JString& className, const JString& args, const bool create,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CustomWidget(LayoutContainer* layout, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~CustomWidget() override;

	void	StreamOut(std::ostream& output) const override;

	const JString&	GetClassName() const;
	bool			NeedsCreate() const;
	void			SetClassName(const JString& name, const bool create);

	const JString&	GetCtorArgs() const;
	void			SetCtorArgs(const JString& args);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JString	itsClassName;
	JString	itsCtorArgs;
	bool	itsCreateFlag;

	CustomWidgetPanel*	itsPanel;	// nullptr unless editing

private:

	void	CustomWidgetX();
};


/******************************************************************************
 Class name

 ******************************************************************************/

inline const JString&
CustomWidget::GetClassName()
	const
{
	return itsClassName;
}

inline bool
CustomWidget::NeedsCreate()
	const
{
	return itsCreateFlag;
}

inline void
CustomWidget::SetClassName
	(
	const JString&	name,
	const bool		create
	)
{
	itsClassName  = name;
	itsCreateFlag = create;
}

/******************************************************************************
 Constructor arguments

 ******************************************************************************/

inline const JString&
CustomWidget::GetCtorArgs()
	const
{
	return itsCtorArgs;
}

inline void
CustomWidget::SetCtorArgs
	(
	const JString& name
	)
{
	itsCtorArgs = name;
}

#endif
