/******************************************************************************
 ComplexBorderRect.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ComplexBorderRect
#define _H_ComplexBorderRect

#include "ContainerWidget.h"

class ComplexBorderWidthPanel;

class ComplexBorderRect : public ContainerWidget
{
public:

	enum Type
	{
		kEngravedType,
		kEmbossedType
	};

public:

	ComplexBorderRect(const Type type, LayoutContainer* layout,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);
	ComplexBorderRect(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~ComplexBorderRect() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	Type						itsType;
	JSize						itsOutsideWidth;
	JSize						itsBetweenWidth;
	JSize						itsInsideWidth;
	ComplexBorderWidthPanel*	itsPanel;

private:

	void	ComplexBorderRectX();
};

#endif
