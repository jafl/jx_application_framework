/******************************************************************************
 TextCheckbox.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextCheckbox
#define _H_TextCheckbox

#include "TextButtonBase.h"

class JXTextCheckbox;
class WidgetLabelPanel;

class TextCheckbox : public TextButtonBase
{
public:

	TextCheckbox(LayoutContainer* layout, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	TextCheckbox(LayoutContainer* layout, const JString& label,
				 const JString& shortcuts, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	TextCheckbox(LayoutContainer* layout, std::istream& input, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~TextCheckbox() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

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

	JXTextCheckbox*		itsCheckbox;
	JString				itsShortcuts;
	WidgetLabelPanel*	itsPanel;	// nullptr unless editing

private:

	void	TextCheckboxX(const JString& label,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);
};

#endif
