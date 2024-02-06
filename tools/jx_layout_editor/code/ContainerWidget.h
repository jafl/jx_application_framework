/******************************************************************************
 ContainerWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ContainerWidget
#define _H_ContainerWidget

#include "LayoutWidget.h"

class ContainerWidget : public LayoutWidget
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

	bool	GetLayoutContainer(const JIndex index, LayoutContainer** layout) const override;
	bool	GetLayoutContainerIndex(const LayoutWidget* widget, JIndex* index) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	DrawOver(JXWindowPainter& p, const JRect& rect) override;

	LayoutContainer*	GetLayoutContainer() const;

private:

	LayoutContainer*	itsLayout;

private:

	void	ContainerWidgetX(LayoutContainer* layout);
};


/******************************************************************************
 GetLayoutContainer (protected)

 ******************************************************************************/

inline LayoutContainer*
ContainerWidget::GetLayoutContainer()
	const
{
	return itsLayout;
}

#endif
