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

	InputField(LayoutContainer* layout, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	InputField(LayoutContainer* layout, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~InputField() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	bool				itsIsRequiredFlag;
	JInteger			itsMinLength;
	JInteger			itsMaxLength;
	JString				itsValidationPattern;
	JString				itsRegexErrorMsg;
	InputFieldPanel*	itsPanel;	// nullptr unless editing

private:

	void	InputFieldX();
};

#endif
