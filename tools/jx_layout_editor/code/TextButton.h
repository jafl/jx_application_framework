/******************************************************************************
 TextButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextButton
#define _H_TextButton

#include "TextButtonBase.h"

class JXTextButton;
class WidgetLabelPanel;

class TextButton : public TextButtonBase
{
public:

	TextButton(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(const JString& label, const JString& shortcuts,
				LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(std::istream& input, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TextButton() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	JString	GetClassName() const override;
	JRect	GetFrameForCode() const override;
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

	JXTextButton*		itsButton;
	JString				itsShortcuts;
	WidgetLabelPanel*	itsPanel;	// nullptr unless editing

private:

	void	TextButtonX(const JString& label,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);
};

#endif
