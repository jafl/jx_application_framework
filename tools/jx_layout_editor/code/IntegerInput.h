/******************************************************************************
 IntegerInput.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_IntegerInput
#define _H_IntegerInput

#include "InputFieldBase.h"

class IntegerInputPanel;

class IntegerInput : public InputFieldBase
{
public:

	IntegerInput(LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	IntegerInput(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~IntegerInput() override;

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
	JInteger			itsMinValue;
	bool				itsHasMaxValue;
	JInteger			itsMaxValue;
	IntegerInputPanel*	itsPanel;	// nullptr unless editing
};

#endif
