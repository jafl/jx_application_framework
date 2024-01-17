/******************************************************************************
 CharInput.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CharInput
#define _H_CharInput

#include "InputFieldBase.h"

class CharInputPanel;

class CharInput : public InputFieldBase
{
public:

	CharInput(LayoutContainer* layout,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);
	CharInput(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	~CharInput() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	JString	GetClassName() const override;
};

#endif
