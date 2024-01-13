/******************************************************************************
 WidgetSet.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetSet
#define _H_WidgetSet

#include "BaseWidget.h"

class WidgetSetPanel;

class WidgetSet : public BaseWidget
{
public:

	WidgetSet(LayoutContainer* layout, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	WidgetSet(LayoutContainer* layout, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~WidgetSet() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
};

#endif
