/******************************************************************************
 WidgetSet.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetSet
#define _H_WidgetSet

#include "ContainerWidget.h"

class WidgetSet : public ContainerWidget
{
public:

	WidgetSet(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	WidgetSet(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~WidgetSet() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	JString	GetClassName() const override;
};

#endif
