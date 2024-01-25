/******************************************************************************
 ContainerWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ContainerWidget
#define _H_ContainerWidget

#include "BaseWidget.h"

class ContainerWidget : public BaseWidget
{
public:

	ContainerWidget(const bool wantsInput, LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	ContainerWidget(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~ContainerWidget() override;

	bool	GetLayoutContainer(LayoutContainer** layout) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	DrawOver(JXWindowPainter& p, const JRect& rect) override;

private:

	LayoutContainer*	itsLayout;

private:

	void	ContainerWidgetX(LayoutContainer* layout);
};

#endif
