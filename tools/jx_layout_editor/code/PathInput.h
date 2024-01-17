/******************************************************************************
 PathInput.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_PathInput
#define _H_PathInput

#include "InputFieldBase.h"

class PathInputPanel;

class PathInput : public InputFieldBase
{
public:

	PathInput(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	PathInput(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~PathInput() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	JString	GetClassName() const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	bool			itsPathRequiredFlag;
	bool			itsAllowInvalidFlag;
	bool			itsWriteFlag;
	PathInputPanel*	itsPanel;	// nullptr unless editing
};

#endif
