/******************************************************************************
 CustomWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CustomWidget
#define _H_CustomWidget

#include "BaseWidget.h"

class CustomWidget : public BaseWidget
{
	friend class JXRadioButton;

public:

	CustomWidget(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CustomWidget(const JString& className, const JString& args, const bool create,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CustomWidget(std::istream& input, JXContainer* enclosure,
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
	void	DrawBackground(JXWindowPainter& p, const JRect& frame) override;

private:

	JString	itsClassName;
	JString	itsCtorArgs;
	bool	itsCreateFlag;

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
