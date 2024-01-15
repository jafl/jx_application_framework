/******************************************************************************
 CoreWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CoreWidget
#define _H_CoreWidget

#include "BaseWidget.h"

class CoreWidget : public BaseWidget
{
public:

	CoreWidget(const bool wantsInput, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	CoreWidget(std::istream& input, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~CoreWidget() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	bool	StealMouse(const int eventType, const JPoint& ptG,
					   const JXMouseButton button,
					   const unsigned int state) override;

	void	SetWidget(JXWidget* widget);

private:

	JXWidget*	itsWidget;
};


/******************************************************************************
 SetWidget

 ******************************************************************************/

inline void
CoreWidget::SetWidget
	(
	JXWidget* widget
	)
{
	itsWidget = widget;
}

#endif
