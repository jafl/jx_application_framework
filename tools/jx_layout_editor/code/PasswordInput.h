/******************************************************************************
 PasswordInput.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_PasswordInput
#define _H_PasswordInput

#include "InputFieldBase.h"

class PasswordInputPanel;

class PasswordInput : public InputFieldBase
{
public:

	PasswordInput(LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	PasswordInput(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~PasswordInput() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	JString	GetClassName() const override;
};

#endif
