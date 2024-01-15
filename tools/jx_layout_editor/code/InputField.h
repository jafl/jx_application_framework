/******************************************************************************
 InputField.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_InputField
#define _H_InputField

#include "BaseWidget.h"

class InputFieldPanel;

class InputField : public BaseWidget
{
public:

	InputField(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	InputField(std::istream& input, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~InputField() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	bool				itsIsRequiredFlag;
	JInteger			itsMinLength;
	JInteger			itsMaxLength;
	JString				itsValidationPattern;
	JString				itsRegexErrorMsg;
	bool				itsWordWrapFlag;
	bool				itsAcceptNewlineFlag;
	InputFieldPanel*	itsPanel;	// nullptr unless editing

private:

	void	InputFieldX();
};

#endif
