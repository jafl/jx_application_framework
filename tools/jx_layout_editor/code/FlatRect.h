/******************************************************************************
 FlatRect.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_FlatRect
#define _H_FlatRect

#include "ContainerWidget.h"

class ColorChooserPanel;

class FlatRect : public ContainerWidget
{
public:

	FlatRect(LayoutContainer* layout,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);
	FlatRect(const JColorID color, LayoutContainer* layout,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);
	FlatRect(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	~FlatRect() override;

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

	JColorID			itsColor;
	ColorChooserPanel*	itsPanel;		// nullptr unless editing
};

#endif
