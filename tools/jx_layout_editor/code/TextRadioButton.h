/******************************************************************************
 TextRadioButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextRadioButton
#define _H_TextRadioButton

#include "BaseWidget.h"

class RadioButtonIDPanel;
class WidgetLabelPanel;

class TextRadioButton : public BaseWidget
{
public:

	TextRadioButton(LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	TextRadioButton(const JString& id, const JString& label, const JString& shortcuts,
					LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	TextRadioButton(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~TextRadioButton() override;

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

	JString				itsID;
	JString				itsLabel;
	JString				itsShortcuts;
	RadioButtonIDPanel*	itsIDPanel;		// nullptr unless editing
	WidgetLabelPanel*	itsLabelPanel;	// nullptr unless editing
};

#endif
