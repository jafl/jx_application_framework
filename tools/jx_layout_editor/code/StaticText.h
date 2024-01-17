/******************************************************************************
 StaticText.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_StaticText
#define _H_StaticText

#include "CoreWidget.h"

class JXStaticText;
class StaticTextPanel;

class StaticText : public CoreWidget
{
public:

	StaticText(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	StaticText(const JString& text,
				LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	StaticText(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~StaticText() override;

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

	JXStaticText*		itsText;
	bool				itsIsLabelFlag;
	bool				itsCenterHorizFlag;
	bool				itsWordWrapFlag;
	bool				itsSelectableFlag;
	bool				itsStyleableFlag;
	StaticTextPanel*	itsPanel;	// nullptr unless editing

private:

	void	StaticTextX(const JString& text,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);
};

#endif
