/******************************************************************************
 FloatInput.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_FloatInput
#define _H_FloatInput

#include "InputFieldBase.h"

class FloatInputPanel;

class FloatInput : public InputFieldBase
{
public:

	FloatInput(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	FloatInput(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~FloatInput() override;

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

	bool				itsIsRequiredFlag;
	bool				itsHasMinValue;
	JFloat				itsMinValue;
	bool				itsHasMaxValue;
	JFloat				itsMaxValue;
	FloatInputPanel*	itsPanel;	// nullptr unless editing
};

#endif
