/******************************************************************************
 WidgetSet.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetSet
#define _H_WidgetSet

#include "BaseWidget.h"

class WidgetSet : public BaseWidget
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
	bool	GetLayoutContainer(LayoutContainer** layout) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;

private:

	LayoutContainer*	itsLayout;

private:

	void	WidgetSetX(LayoutContainer* layout);
};

#endif
