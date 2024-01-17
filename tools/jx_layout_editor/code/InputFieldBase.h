/******************************************************************************
 InputFieldBase.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_InputFieldBase
#define _H_InputFieldBase

#include "BaseWidget.h"

class InputFieldBase : public BaseWidget
{
public:

	InputFieldBase(LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	InputFieldBase(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~InputFieldBase() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	void	InputFieldBaseX();
};

#endif
