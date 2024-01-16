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

	CustomWidget(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CustomWidget(const JString& className, const JString& args, const bool create,
				LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CustomWidget(std::istream& input, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~CustomWidget() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
	JString	GetCtor() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;

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

#endif
